#include <algorithm>

// This is a small hack to prevent including in tAlgorithm.h's swap template function
// that would have broken STL's existing swap template causing compiling errors upon
// usage of std::sort() within this source file. --Nathan Martin
#define _TALGORITHM_H_

// simSet needs this from tAlgorithm.h so we will have a copy of it here.
/// Finds the first matching value within the container
/// returning the the element or last if its not found.
template <class Iterator, class Value>
Iterator find(Iterator first, Iterator last, Value value)
{
   while (first != last && *first != value)
      ++first;
   return first;
}

#include "platform/platform.h"
#include "T3D/fx/particleEmitter.h"
#include "particle.h"

#include "scene/sceneManager.h"
#include "scene/sceneRenderState.h"
#include "console/consoleTypes.h"
#include "console/typeValidators.h"
#include "core/stream/bitStream.h"
#include "core/strings/stringUnit.h"
#include "math/mRandom.h"
#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "gfx/gfxStringEnumTranslate.h"
#include "renderInstance/renderPassManager.h"
#include "T3D/gameBase/gameProcess.h"
#include "lighting/lightInfo.h"
#include "console/engineAPI.h"
#include "materials/materialManager.h"
#include "T3D/gameBase/gameConnection.h"

#if defined(TORQUE_OS_XENON)
#  include "gfx/D3D9/360/gfx360MemVertexBuffer.h"
#endif

Point3F ParticleEmitter::mWindVelocity( 0.0, 0.0, 0.0 );
const F32 ParticleEmitter::AgedSpinToRadians = (1.0f/1000.0f) * (1.0f/360.0f) * M_PI_F * 2.0f;

static const float sgDefaultEjectionOffset = 0.f;

//-----------------------------------------------------------------------------
// ParticleEmitterData
//-----------------------------------------------------------------------------

ParticleEmitterData::ParticleEmitterData()
{
   VECTOR_SET_ASSOCIATION(particleDataBlocks);
   VECTOR_SET_ASSOCIATION(dataBlockIds);

   ejectionPeriodMS	= 100;   // 10 Particles Per second
   periodVarianceMS	= 0;     // exactly

   ejectionVelocity	= 2.0f;  // From 1.0 - 3.0 meters per sec
   velocityVariance	= 1.0f;
   ejectionOffset		= sgDefaultEjectionOffset;    // ejection from the emitter point

   ejectionLODStartDistance	= 15.0f;
   ejectionLODEndDistance	   = 100.0f;
   lodEjectionPeriod		= 400;
   mediumResDistance		= 10.0f;
   lowResDistance		   = 20.0f;
   SimulationLODBegin	= 10;
   SimulationLODEnd		= 100;

   softnessDistance		= 1.0f;
   ambientFactor			= 0.0f;

   lifetimeMS				= 0;
   lifetimeVarianceMS	= 0;

   overrideAdvance   = true;
   orientParticles	= false;
   orientOnVelocity	= true;
   mUseEmitterSizes	= false;
   mUseEmitterColors	= false;
   particleString		= NULL;
   partListInitSize	 = 0;

   // These members added for support of user defined blend factors
   // and optional particle sorting.
   blendStyle = ParticleRenderInst::BlendUndefined;
   sortParticles     = false;
   renderReflection  = true;
   reverseOrder      = false;
   textureName       = 0;
   textureHandle     = 0;
   highResOnly       = true;

   alignParticles = false;
   alignDirection = Point3F(0.0f, 1.0f, 0.0f);

   standAloneEmitter = false;

   for(int i = 0; i < ParticleBehaviourCount; i++)
      ParticleBHVs[i] = NULL;
}



// Enum tables used for fields blendStyle, srcBlendFactor, dstBlendFactor.
// Note that the enums for srcBlendFactor and dstBlendFactor are consistent
// with the blending enums used in Torque Game Builder.
typedef ParticleRenderInst::BlendStyle ParticleBlendStyle;
DefineEnumType( ParticleBlendStyle );

ImplementEnumType( ParticleBlendStyle,
                  "The type of visual blending style to apply to the particles.\n"
                  "@ingroup FX\n\n")
{ ParticleRenderInst::BlendNormal,         "NORMAL",        "No blending style.\n" },
{ ParticleRenderInst::BlendAdditive,       "ADDITIVE",      "Adds the color of the pixel to the frame buffer with full alpha for each pixel.\n" },
{ ParticleRenderInst::BlendSubtractive,    "SUBTRACTIVE",   "Subtractive Blending. Reverses the color model, causing dark colors to have a stronger visual effect.\n" },
{ ParticleRenderInst::BlendPremultAlpha,   "PREMULTALPHA",  "Color blends with the colors of the imagemap rather than the alpha.\n" },
EndImplementEnumType;

IRangeValidator ejectPeriodIValidator(1, 2047);
IRangeValidator periodVarianceIValidator(0, 2047);
FRangeValidator ejectionFValidator(0.f, 655.35f);
FRangeValidator velVarianceFValidator(0.f, 163.83f);

void ParticleEmitterData::initPersistFields()
{
   addGroup( "ParticleEmitterData" );

   addFieldV("ejectionPeriodMS", TYPEID< S32 >(), Offset(ejectionPeriodMS,   ParticleEmitterData), &ejectPeriodIValidator,
      "Time (in milliseconds) between each particle ejection." );

   addFieldV("periodVarianceMS", TYPEID< S32 >(), Offset(periodVarianceMS,   ParticleEmitterData), &periodVarianceIValidator,
      "Variance in ejection period, from 1 - ejectionPeriodMS." );

   addFieldV( "ejectionVelocity", TYPEID< F32 >(), Offset(ejectionVelocity, ParticleEmitterData), &ejectionFValidator,
      "Particle ejection velocity." );

   addFieldV( "velocityVariance", TYPEID< F32 >(), Offset(velocityVariance, ParticleEmitterData), &velVarianceFValidator,
      "Variance for ejection velocity, from 0 - ejectionVelocity." );

   addFieldV( "ejectionOffset", TYPEID< F32 >(), Offset(ejectionOffset, ParticleEmitterData), &ejectionFValidator,
      "Distance along ejection Z axis from which to eject particles." );

   addField( "softnessDistance", TYPEID< F32 >(), Offset(softnessDistance, ParticleEmitterData),
      "For soft particles, the distance (in meters) where particles will be "
      "faded based on the difference in depth between the particle and the "
      "scene geometry." );

   addField( "ambientFactor", TYPEID< F32 >(), Offset(ambientFactor, ParticleEmitterData),
      "Used to generate the final particle color by controlling interpolation "
      "between the particle color and the particle color multiplied by the "
      "ambient light color." );

   addField( "overrideAdvance", TYPEID< bool >(), Offset(overrideAdvance, ParticleEmitterData),
      "If false, particles emitted in the same frame have their positions "
      "adjusted. If true, adjustment is skipped and particles will clump "
      "together." );

   addField( "orientParticles", TYPEID< bool >(), Offset(orientParticles, ParticleEmitterData),
      "If true, Particles will always face the camera." );

   addField( "orientOnVelocity", TYPEID< bool >(), Offset(orientOnVelocity, ParticleEmitterData),
      "If true, particles will be oriented to face in the direction they are moving." );

   addField( "particles", TYPEID< StringTableEntry >(), Offset(particleString, ParticleEmitterData),
      "@brief List of space or TAB delimited ParticleData datablock names.\n\n"
      "A random one of these datablocks is selected each time a particle is "
      "emitted." );

   addField( "lifetimeMS", TYPEID< S32 >(), Offset(lifetimeMS, ParticleEmitterData),
      "Lifetime of emitted particles (in milliseconds)." );

   addField("lifetimeVarianceMS", TYPEID< S32 >(), Offset(lifetimeVarianceMS, ParticleEmitterData),
      "Variance in particle lifetime from 0 - lifetimeMS." );

   addField( "useEmitterSizes", TYPEID< bool >(), Offset(mUseEmitterSizes, ParticleEmitterData),
      "@brief If true, use emitter specified sizes instead of datablock sizes.\n"
      "Useful for Debris particle emitters that control the particle size." );

   addField( "useEmitterColors", TYPEID< bool >(), Offset(mUseEmitterColors, ParticleEmitterData),
      "@brief If true, use emitter specified colors instead of datablock colors.\n\n"
      "Useful for ShapeBase dust and WheeledVehicle wheel particle emitters that use "
      "the current material to control particle color." );

   /// These fields added for support of user defined blend factors and optional particle sorting.
   //@{
   // addField( "blendStyle", TYPEID< ParticleRenderInst::BlendStyle >(), Offset(blendStyle, ParticleEmitterData),
   // "String value that controls how emitted particles blend with the scene." );
   addField( "blendStyle", TYPEID< ParticleRenderInst::BlendStyle >(), Offset(blendStyle, ParticleEmitterData),
      "String value that controls how emitted particles blend with the scene." );

   addField( "sortParticles", TYPEID< bool >(), Offset(sortParticles, ParticleEmitterData),
      "If true, particles are sorted furthest to nearest.");

   addField( "reverseOrder", TYPEID< bool >(), Offset(reverseOrder, ParticleEmitterData),
      "@brief If true, reverses the normal draw order of particles.\n\n"
      "Particles are normally drawn from newest to oldest, or in Z order "
      "(furthest first) if sortParticles is true. Setting this field to "
      "true will reverse that order: oldest first, or nearest first if "
      "sortParticles is true." );

   addField( "textureName", TYPEID< StringTableEntry >(), Offset(textureName, ParticleEmitterData),
      "Optional texture to override ParticleData::textureName." );

   addField( "alignParticles", TYPEID< bool >(), Offset(alignParticles, ParticleEmitterData),
      "If true, particles always face along the axis defined by alignDirection." );

   addProtectedField( "alignDirection", TYPEID< Point3F>(), Offset(alignDirection, ParticleEmitterData), &ParticleEmitterData::_setAlignDirection, &defaultProtectedGetFn,
      "The direction aligned particles should face, only valid if alignParticles is true." );

   addField( "highResOnly", TYPEID< bool >(), Offset(highResOnly, ParticleEmitterData),
      "This particle system should not use the mixed-resolution renderer. "
      "If your particle system has large amounts of overdraw, consider "
      "disabling this option." );

   addField( "renderReflection", TYPEID< bool >(), Offset(renderReflection, ParticleEmitterData),
      "Controls whether particles are rendered onto reflective surfaces like water." );

   //@}

   endGroup( "ParticleEmitterData" );

   addGroup( "ParticleBehaviours" );

   addField("ParticleBehaviour", TYPEID<IParticleBehaviour>(), Offset(ParticleBHVs, ParticleEmitterData), ParticleBehaviourCount,
      "Null");

   endGroup( "ParticleBehaviours" );

   addGroup( "LevelOfDetail" );

   addField( "MediumResolutionDistance", TypeF32, Offset(mediumResDistance, ParticleEmitterData),
      "@brief How far the node must be from the camera before the emitter switches to the medium resolution texture.");

   addField( "LowResolutionDistance", TypeF32, Offset(lowResDistance, ParticleEmitterData),
      "@brief How far the node must be from the camera before the emitter switches to the low resolution texture.");

   addField( "EjectionLODStartDistance", TypeF32, Offset(ejectionLODStartDistance, ParticleEmitterData),
      "@brief How far the node must be from the camera before the emitter interpolates the ejectionPeriod towards LODEjectionPeriod.");

   addField( "EjectionLODEndDistance", TypeF32, Offset(ejectionLODEndDistance, ParticleEmitterData),
      "@brief How far the node must be from the camera before the emitter stops interpolating the ejection period and stops emitting particles.");

   addField( "LODEjectionPeriod", TypeS32, Offset(lodEjectionPeriod, ParticleEmitterData),
      "@brief What level the ejection period interpolates to based on distance.");

   addField( "SimulationLODBegin", TypeS32, Offset(SimulationLODBegin, ParticleEmitterData),
      "@brief How far the node must be from the camera before the emitter begins reducing the amount of update ticks per second it runs. ");

   addField( "SimulationLODEnd", TypeS32, Offset(SimulationLODEnd, ParticleEmitterData),
      "@brief How far the node must be from the camera before the emitter stops updating the particles. ");

   endGroup( "LevelOfDetail" );

   addField( "Standalone", TypeBool, Offset(standAloneEmitter, ParticleEmitterData),
      "@brief ");


   Parent::initPersistFields();
}

bool ParticleEmitterData::onAdd()
{
   if( !Parent::onAdd() )
      return false;
   // Validate the parameters...
   //
   if( ejectionPeriodMS < 1 )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) period < 1 ms", getName());
      ejectionPeriodMS = 1;
   }
   if( periodVarianceMS >= ejectionPeriodMS )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) periodVariance >= period", getName());
      periodVarianceMS = ejectionPeriodMS - 1;
   }
   if( ejectionVelocity < 0.0f )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) ejectionVelocity < 0.0f", getName());
      ejectionVelocity = 0.0f;
   }
   if( velocityVariance < 0.0f )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) velocityVariance < 0.0f", getName());
      velocityVariance = 0.0f;
   }
   if( velocityVariance > ejectionVelocity )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) velocityVariance > ejectionVelocity", getName());
      velocityVariance = ejectionVelocity;
   }
   if( ejectionOffset < 0.0f )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) ejectionOffset < 0", getName());
      ejectionOffset = 0.0f;
   }

   if ( softnessDistance < 0.0f )
   {
      Con::warnf( ConsoleLogEntry::General, "ParticleEmitterData(%s) invalid softnessDistance", getName() );
      softnessDistance = 0.0f;
   }

   if (particleString == NULL && dataBlockIds.size() == 0) 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) no particleString, invalid datablock", getName());
      return false;
   }
   if (particleString && particleString[0] == '\0') 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) no particleString, invalid datablock", getName());
      return false;
   }
   if (particleString && dStrlen(particleString) > 255) 
   {
      Con::errorf(ConsoleLogEntry::General, "ParticleEmitterData(%s) particle string too long [> 255 chars]", getName());
      return false;
   }

   if( lifetimeMS < 0 )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) lifetimeMS < 0.0f", getName());
      lifetimeMS = 0;
   }
   if( lifetimeVarianceMS > lifetimeMS )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) lifetimeVarianceMS >= lifetimeMS", getName());
      lifetimeVarianceMS = lifetimeMS;
   }

   // load the particle datablocks...
   //
   if( particleString != NULL )
   {
      //   particleString is once again a list of particle datablocks so it
      //   must be parsed to extract the particle references.

      // First we parse particleString into a list of particle name tokens 
      Vector<char*> dataBlocks(__FILE__, __LINE__);
      char* tokCopy = new char[dStrlen(particleString) + 1];
      dStrcpy(tokCopy, particleString);

      char* currTok = dStrtok(tokCopy, " \t");
      while (currTok != NULL) 
      {
         dataBlocks.push_back(currTok);
         currTok = dStrtok(NULL, " \t");
      }
      if (dataBlocks.size() == 0) 
      {
         Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) invalid particles string.  No datablocks found", getName());
         delete [] tokCopy;
         return false;
      }    

      // Now we convert the particle name tokens into particle datablocks and IDs 
      particleDataBlocks.clear();
      dataBlockIds.clear();

      for (U32 i = 0; i < dataBlocks.size(); i++) 
      {
         ParticleData* pData = NULL;
         if (Sim::findObject(dataBlocks[i], pData) == false) 
         {
            Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find particle datablock: %s", getName(), dataBlocks[i]);
         }
         else 
         {
            particleDataBlocks.push_back(pData);
            dataBlockIds.push_back(pData->getId());
         }
      }

      // cleanup
      delete [] tokCopy;

      // check that we actually found some particle datablocks
      if (particleDataBlocks.size() == 0) 
      {
         Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find any particle datablocks", getName());
         return false;
      }
   }

   return true;
}

bool ParticleEmitterData::preload(bool server, String &errorStr)
{
   if( Parent::preload(server, errorStr) == false )
      return false;

   particleDataBlocks.clear();
   for (U32 i = 0; i < dataBlockIds.size(); i++) 
   {
      ParticleData* pData = NULL;
      if (Sim::findObject(dataBlockIds[i], pData) == false)
         Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find particle datablock: %d", getName(), dataBlockIds[i]);
      else
         particleDataBlocks.push_back(pData);
   }
   U32 ParticleDataUpdateMask = 0;
   if (!server)
   {
      // load emitter texture if specified
      if (textureName && textureName[0])
      {
         textureHandle = GFXTexHandle(textureName, &GFXDefaultStaticDiffuseProfile, avar("%s() - textureHandle (line %d)", __FUNCTION__, __LINE__));
         if (!textureHandle)
         {
            errorStr = String::ToString("Missing particle emitter texture: %s", textureName);
            return false;
         }
      }
      // otherwise, check that all particles refer to the same texture
      else if (particleDataBlocks.size() > 1)
      {
         ParticleDataUpdateMask |= ParticleData::ParticleVectorPreloadFlags::ParticleTextures;
      }
   }
   
   // if blend-style is undefined check legacy useInvAlpha settings
   if (blendStyle == ParticleRenderInst::BlendUndefined && particleDataBlocks.size() > 0)
   {
      ParticleDataUpdateMask |= ParticleData::ParticleVectorPreloadFlags::InvAlpha;
   }

   particleDataBlocks[0]->preloadVector(particleDataBlocks, ParticleDataUpdateMask, this);
   
   if( !server )
   {
      allocPrimBuffer();
   }

   return true;
}

void ParticleEmitterData::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeInt(ejectionPeriodMS, 15);
   stream->writeInt(periodVarianceMS, 14);

   stream->writeInt((S32)(ejectionVelocity * 100), 16);
   stream->writeInt((S32)(velocityVariance * 100), 14);
   if( stream->writeFlag( ejectionOffset != sgDefaultEjectionOffset ) )
      stream->writeInt((S32)(ejectionOffset * 100), 16);

   stream->write( softnessDistance );
   stream->write( ambientFactor );

   stream->writeFlag(overrideAdvance);
   stream->writeFlag(orientParticles);
   stream->writeFlag(orientOnVelocity);
   stream->write( lifetimeMS );
   stream->write( lifetimeVarianceMS );
   stream->writeFlag(mUseEmitterSizes);
   stream->writeFlag(mUseEmitterColors);

   stream->write(dataBlockIds.size());
   for (U32 i = 0; i < dataBlockIds.size(); i++)
      stream->write(dataBlockIds[i]);
   stream->writeFlag(sortParticles);
   stream->writeFlag(reverseOrder);
   if (stream->writeFlag(textureName != 0))
      stream->writeString(textureName);

   if (stream->writeFlag(alignParticles))
   {
      stream->write(alignDirection.x);
      stream->write(alignDirection.y);
      stream->write(alignDirection.z);
   }
   stream->writeFlag(highResOnly);
   stream->writeFlag(renderReflection);
   stream->writeInt( blendStyle, 4 );

   stream->writeFlag( standAloneEmitter );

   for(int i = 0; i < ParticleBehaviourCount; i++)
   {
      if(stream->writeFlag(ParticleBHVs[i]))
      {
         stream->writeRangedU32( ParticleBHVs[i]->getId(),
            DataBlockObjectIdFirst,
            DataBlockObjectIdLast );
      }
   }

   stream->writeInt(mediumResDistance * 1000, 18); 
   stream->writeInt(lowResDistance * 1000, 18);
   stream->writeInt(ejectionLODStartDistance * 1000, 18);
   stream->writeInt(ejectionLODEndDistance * 1000, 18);
   stream->writeInt(lodEjectionPeriod, 11);

   stream->writeInt(SimulationLODBegin * 1000, 18);
   stream->writeInt(SimulationLODEnd * 1000, 18);
}

void ParticleEmitterData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   ejectionPeriodMS = stream->readInt(15);
   periodVarianceMS = stream->readInt(14);

   ejectionVelocity = stream->readInt(16) / 100.0f;
   velocityVariance = stream->readInt(14) / 100.0f;
   if( stream->readFlag() )
      ejectionOffset = stream->readInt(16) / 100.0f;
   else
      ejectionOffset = sgDefaultEjectionOffset;

   stream->read( &softnessDistance );
   stream->read( &ambientFactor );

   overrideAdvance = stream->readFlag();
   orientParticles = stream->readFlag();
   orientOnVelocity = stream->readFlag();
   stream->read( &lifetimeMS );
   stream->read( &lifetimeVarianceMS );
   mUseEmitterSizes = stream->readFlag();
   mUseEmitterColors = stream->readFlag();

   U32 size; stream->read(&size);
   dataBlockIds.setSize(size);
   for (U32 i = 0; i < dataBlockIds.size(); i++)
      stream->read(&dataBlockIds[i]);
   sortParticles = stream->readFlag();
   reverseOrder = stream->readFlag();
   textureName = (stream->readFlag()) ? stream->readSTString() : 0;

   alignParticles = stream->readFlag();
   if (alignParticles)
   {
      stream->read(&alignDirection.x);
      stream->read(&alignDirection.y);
      stream->read(&alignDirection.z);
   }
   highResOnly = stream->readFlag();
   renderReflection = stream->readFlag();
   blendStyle = stream->readInt( 4 );

   standAloneEmitter = stream->readFlag();

   // DataBlockMask
   for(int i = 0; i < ParticleBehaviourCount; i++)
   {
      if ( stream->readFlag() )
      {
         IParticleBehaviour *dptr = 0;
         SimObjectId id = stream->readRangedU32( DataBlockObjectIdFirst,
            DataBlockObjectIdLast );
         if ( !Sim::findObject( id, dptr ) )
            ParticleBHVs[i] = dptr;
      }
   }

   mediumResDistance			= stream->readInt(18) / 1000.0f;
   lowResDistance				= stream->readInt(18) / 1000.0f;
   ejectionLODStartDistance	= stream->readInt(18) / 1000.0f;
   ejectionLODEndDistance		= stream->readInt(18) / 1000.0f;
   lodEjectionPeriod			= stream->readInt(11);
   SimulationLODBegin			= stream->readInt(18) / 1000.0f;
   SimulationLODEnd			= stream->readInt(18) / 1000.0f;
}

bool ParticleEmitterData::_setAlignDirection( void *object, const char *index, const char *data )
{
   ParticleEmitterData *p = static_cast<ParticleEmitterData*>( object );

   Con::setData( TypePoint3F, &p->alignDirection, 0, 1, &data );
   p->alignDirection.normalizeSafe();

   // we already set the field
   return false;
}

//-----------------------------------------------------------------------------
// alloc PrimitiveBuffer
// The datablock allocates this static index buffer because it's the same
// for all of the emitters - each particle quad uses the same index ordering
//-----------------------------------------------------------------------------
void ParticleEmitterData::allocPrimBuffer( S32 overrideSize )
{
   // calculate particle list size
   AssertFatal(particleDataBlocks.size() > 0, "Error, no particles found." );
   U32 maxPartLife = particleDataBlocks[0]->lifetimeMS + particleDataBlocks[0]->lifetimeVarianceMS;
   for (S32 i = 1; i < particleDataBlocks.size(); i++)
   {
      U32 mpl = particleDataBlocks[i]->lifetimeMS + particleDataBlocks[i]->lifetimeVarianceMS;
      if (mpl > maxPartLife)
         maxPartLife = mpl;
   }

   partListInitSize = maxPartLife / (ejectionPeriodMS - periodVarianceMS);
   partListInitSize += 8; // add 8 as "fudge factor" to make sure it doesn't realloc if it goes over by 1

   // if override size is specified, then the emitter overran its buffer and needs a larger allocation
   if( overrideSize != -1 )
   {
      partListInitSize = overrideSize;
   }

   // create index buffer based on that size
   U32 indexListSize = partListInitSize * 6; // 6 indices per particle
   U16 *indices = new U16[ indexListSize ];

   for( U32 i=0; i<partListInitSize; i++ )
   {
      // this index ordering should be optimal (hopefully) for the vertex cache
      U16 *idx = &indices[i*6];
      volatile U32 offset = i * 4;  // set to volatile to fix VC6 Release mode compiler bug
      idx[0] = 0 + offset;
      idx[1] = 1 + offset;
      idx[2] = 3 + offset;
      idx[3] = 1 + offset;
      idx[4] = 3 + offset;
      idx[5] = 2 + offset;
   }


   U16 *ibIndices;
   GFXBufferType bufferType = GFXBufferTypeStatic;

#ifdef TORQUE_OS_XENON
   // Because of the way the volatile buffers work on Xenon this is the only
   // way to do this.
   bufferType = GFXBufferTypeVolatile;
#endif
   primBuff.set( GFX, indexListSize, 0, bufferType );
   primBuff.lock( &ibIndices );
   dMemcpy( ibIndices, indices, indexListSize * sizeof(U16) );
   primBuff.unlock();

   delete [] indices;
}

//-----------------------------------------------------------------------------
// ParticleEmitter
//-----------------------------------------------------------------------------
ParticleEmitter::ParticleEmitter()
{
   // ParticleEmitter should be allocated on the client only.
   mNetFlags.set( IsGhost );

   mDeleteWhenEmpty  = false;
   mDeleteOnTick     = false;

   mInternalClock    = 0;
   mNextParticleTime = 0;

   mLastPosition.set(0, 0, 0);
   mHasLastPosition = false;

   mLifetimeMS = 0;
   mElapsedTimeMS = 0;

   mDead = false;

   mDataBlock = NULL;
   mParticleManager = NULL;

   oldTime = 0;
   parentNodePos = Point3F(0);

   standAloneEmitter = false;
   mActive = true;

   sa_ejectionPeriodMS = 100;    // 10 Particles Per second
   sa_periodVarianceMS = 0;      // exactly
   RenderEjectionPeriodMS = sa_ejectionPeriodMS;

   sa_ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
   sa_velocityVariance = 1.0f;
   sa_ejectionOffset   = 0.0f;   // ejection from the emitter point

   for(int i = 0; i < ParticleBehaviourCount; i++)
      ParticleBHVs[i] = NULL;

   parentNodePos = Point3F(0);
   parentNodeBox = Box3F::Invalid;
   TSU = 0;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
ParticleEmitter::~ParticleEmitter()
{
   if(mParticleManager){
      delete mParticleManager->mEmitter;
      delete mParticleManager->mEmitterDatablock;
      delete mParticleManager;
   }
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool ParticleEmitter::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   // add to client side mission cleanup
   SimGroup *cleanup = dynamic_cast<SimGroup *>( Sim::findObject( "ClientMissionCleanup") );
   if( cleanup != NULL )
   {
      cleanup->addObject( this );
   }/*
   else
   {
      AssertFatal( false, "Error, could not find ClientMissionCleanup group" );
      return false;
   }*/

   removeFromProcessList();

   F32 radius = 5.0;
   mObjBox.minExtents = Point3F(-radius, -radius, -radius);
   mObjBox.maxExtents = Point3F(radius, radius, radius);
   resetWorldBox();

   return true;
}

//-----------------------------------------------------------------------------
// onRemove
//-----------------------------------------------------------------------------
void ParticleEmitter::onRemove()
{
   removeFromScene();
   Parent::onRemove();
}

bool ParticleEmitter::onNewDataBlock( GameBaseData *dptr, bool reload )
{
   mDataBlock = dynamic_cast<ParticleEmitterData*>( dptr );
   if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
      return false;

   standAloneEmitter = mDataBlock->standAloneEmitter;

   sa_ejectionPeriodMS = mDataBlock->ejectionPeriodMS;
   sa_periodVarianceMS = mDataBlock->periodVarianceMS;

   sa_ejectionVelocity = mDataBlock->ejectionVelocity;
   sa_velocityVariance = mDataBlock->velocityVariance;
   sa_ejectionOffset = mDataBlock->ejectionOffset;

   RenderEjectionPeriodMS = mDataBlock->ejectionPeriodMS;

   mLifetimeMS = mDataBlock->lifetimeMS;
   if( mDataBlock->lifetimeVarianceMS )
   {
      mLifetimeMS += S32( gRandGen.randI() % (2 * mDataBlock->lifetimeVarianceMS + 1)) - S32(mDataBlock->lifetimeVarianceMS );
   }
   if(mParticleManager){
      delete mParticleManager->mEmitter;
      delete mParticleManager->mEmitterDatablock;
      delete mParticleManager;
   }
   mParticleManager = mDataBlock->particleDataBlocks[0]->CreateManager();
   mParticleManager->mEmitter = new ParticleEmitterWrapper(this);
   mParticleManager->mEmitterDatablock = new ParticleEmitterDataWrapper(mDataBlock);
   mParticleManager->AllocateParticles();

   scriptOnNewDataBlock();
   return true;
}

//-----------------------------------------------------------------------------
// getCollectiveColor
//-----------------------------------------------------------------------------
ColorF ParticleEmitter::getCollectiveColor()
{
   return mParticleManager->getCollectiveColor();
}


//-----------------------------------------------------------------------------
// prepRenderImage
//-----------------------------------------------------------------------------
void ParticleEmitter::prepRenderImage(SceneRenderState* state)
{
   mParticleManager->RenderParticles(state);
}

//-----------------------------------------------------------------------------
// setSizes
//-----------------------------------------------------------------------------
void ParticleEmitter::setSizes( F32 *sizeList )
{
   for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
   {
      sizes[i] = sizeList[i];
   }
}

//-----------------------------------------------------------------------------
// setColors
//-----------------------------------------------------------------------------
void ParticleEmitter::setColors( ColorF *colorList )
{
   for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
   {
      colors[i] = colorList[i];
   }
}

//-----------------------------------------------------------------------------
// deleteWhenEmpty
//-----------------------------------------------------------------------------
void ParticleEmitter::deleteWhenEmpty()
{
   // if the following asserts fire, there is a reasonable chance that you are trying to delete a particle emitter
   // that has already been deleted (possibly by ClientMissionCleanup). If so, use a SimObjectPtr to the emitter and check it
   // for null before calling this function.
   AssertFatal(isProperlyAdded(), "ParticleEmitter must be registed before calling deleteWhenEmpty");
   AssertFatal(!mDead, "ParticleEmitter already deleted");
   AssertFatal(!isDeleted(), "ParticleEmitter already deleted");
   AssertFatal(!isRemoved(), "ParticleEmitter already removed");

   // this check is for non debug case, so that we don't write in to freed memory
   bool okToDelete = !mDead && isProperlyAdded() && !isDeleted() && !isRemoved();
   if (okToDelete)
   {
      mDeleteWhenEmpty = true;
      if( !mParticleManager->n_parts )
      {
         // We're already empty, so delete us now.
         
         mDead = true;
         deleteObject();
      }
      else
         AssertFatal( getSceneManager() != NULL, "ParticleEmitter not on process list and won't get ticked to death" );
   }
}

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void ParticleEmitter::emitParticles(const Point3F& point,
                                    const bool     useLastPosition,
                                    const Point3F& axis,
                                    const Point3F& velocity,
                                    const U32      numMilliseconds)
{
   if( mDead ) return;

   if( !mActive ) return;

   // lifetime over - no more particles
   if( mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS )
   {
      return;
   }

   Point3F realStart;
   if( useLastPosition && mHasLastPosition )
      realStart = mLastPosition;
   else
      realStart = point;

   emitParticles(realStart, point,
      axis,
      velocity,
      numMilliseconds);
}

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void ParticleEmitter::emitParticles(const Point3F& start,
                                    const Point3F& end,
                                    const Point3F& axis,
                                    const Point3F& velocity,
                                    const U32      numMilliseconds)
{
   if( mDead ) return;

   if( mDataBlock->particleDataBlocks.empty() )
      return;

   if( !mActive ) return;

   // lifetime over - no more particles
   if( mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS )
   {
      return;
   }

   U32 currTime = 0;
   bool particlesAdded = false;

   Point3F axisx;
   if( mFabs(axis.z) < 0.9f )
      mCross(axis, Point3F(0, 0, 1), &axisx);
   else
      mCross(axis, Point3F(0, 1, 0), &axisx);
   axisx.normalize();

   if( mNextParticleTime != 0 )
   {
      // Need to handle next particle
      //
      if( mNextParticleTime > numMilliseconds )
      {
         // Defer to next update
         //  (Note that this introduces a potential spatial irregularity if the owning
         //   object is accelerating, and updating at a low frequency)
         //
         mNextParticleTime -= numMilliseconds;
         mInternalClock += numMilliseconds;
         mLastPosition = end;
         mHasLastPosition = true;
         return;
      }
      else
      {
         currTime       += mNextParticleTime;
         mInternalClock += mNextParticleTime;
         // Emit particle at curr time

         // Create particle at the correct position
         Point3F pos;
         pos.interpolate(start, end, F32(currTime) / F32(numMilliseconds));
         parentNodePos = pos;
         parentNodeBox = Box3F(0);
         if(addParticle(pos, axis, velocity, axisx, getWorldTransform()))
         {
            particlesAdded = true;
            mNextParticleTime = 0;
         }
      }
   }

   while( currTime < numMilliseconds )
   {
      S32 nextTime = RenderEjectionPeriodMS;
      if( mDataBlock->periodVarianceMS != 0 )
      {
         nextTime += S32(gRandGen.randI() % (2 * mDataBlock->periodVarianceMS + 1)) -
            S32(mDataBlock->periodVarianceMS);
      }
      AssertFatal(nextTime > 0, "Error, next particle ejection time must always be greater than 0");

      if( currTime + nextTime > numMilliseconds )
      {
         mNextParticleTime = (currTime + nextTime) - numMilliseconds;
         mInternalClock   += numMilliseconds - currTime;
         AssertFatal(mNextParticleTime > 0, "Error, should not have deferred this particle!");
         break;
      }

      currTime       += nextTime;
      mInternalClock += nextTime;

      // Create particle at the correct position
      Point3F pos;
      pos.interpolate(start, end, F32(currTime) / F32(numMilliseconds));
      parentNodePos = pos;
      parentNodeBox = Box3F(0);
      if(addParticle(pos, axis, velocity, axisx, getTransform()))
      {
         particlesAdded = true;
      }

      //   This override-advance code is restored in order to correctly adjust
      //   animated parameters of particles allocated within the same frame
      //   update. Note that ordering is important and this code correctly 
      //   adds particles in the same newest-to-oldest ordering of the link-list.
      //
      // NOTE: We are assuming that the just added particle is at the head of our
      //  list.  If that changes, so must this...
      U32 advanceMS = numMilliseconds - currTime;
      if (mDataBlock->overrideAdvance == false && advanceMS != 0) 
      {
         Particle* last_part = mParticleManager->part_list_head.next;
         if (advanceMS > last_part->totalLifetime) 
         {
            mParticleManager->part_list_head.next = last_part->next;
            mParticleManager->n_parts--;
            last_part->next = mParticleManager->part_freelist;
            mParticleManager->part_freelist = last_part;
         } 
         else 
         {
            if (advanceMS != 0)
            {
               F32 t = F32(advanceMS) / 1000.0;

               Point3F a = last_part->acc;
               a -= last_part->vel * last_part->dataBlock->dragCoefficient;
               a -= mWindVelocity * last_part->dataBlock->windCoefficient;
               a += Point3F(0.0f, 0.0f, -9.81f) * last_part->dataBlock->gravityCoefficient;

               last_part->vel += a * t;
               last_part->pos = last_part->pos + last_part->vel * t;

               updateKeyData( last_part );
            }
         }
      }
   }

   // DMMFIX: Lame and slow...
   if( particlesAdded == true )
      updateBBox();


   if( mParticleManager->n_parts > 0 && getSceneManager() == NULL )
   {
      gClientSceneGraph->addObjectToScene(this);
      ClientProcessList::get()->addObject(this);
   }

   mLastPosition = end;
   mHasLastPosition = true;
}

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void ParticleEmitter::emitParticles( const U32 numMilliseconds, ParticleEmitterNode* node )
{
   if( mDead ) return;

   if( mDataBlock->particleDataBlocks.empty() )
      return;

   if( !mActive ) return;

   // lifetime over - no more particles
   if( mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS )
   {
      return;
   }

   Point3F start, end, axis, velocity;
   Point3F emitPoint, emitVelocity;
   Point3F emitAxis(0, 0, 1);
   node->getTransform().mulV(emitAxis);
   node->getTransform().getColumn(3, &emitPoint);
   emitVelocity = emitAxis * node->getVelocity();
   start = emitPoint;
   end = emitPoint;
   axis = emitAxis;
   velocity = emitVelocity;

   U32 currTime = 0;
   bool particlesAdded = false;

   Point3F axisx;
   if( mFabs(axis.z) < 0.9f )
      mCross(axis, Point3F(0, 0, 1), &axisx);
   else
      mCross(axis, Point3F(0, 1, 0), &axisx);
   axisx.normalize();

   if( mNextParticleTime != 0 )
   {
      // Need to handle next particle
      //
      if( mNextParticleTime > numMilliseconds )
      {
         // Defer to next update
         //  (Note that this introduces a potential spatial irregularity if the owning
         //   object is accelerating, and updating at a low frequency)
         //
         mNextParticleTime -= numMilliseconds;
         mInternalClock += numMilliseconds;
         mLastPosition = end;
         mHasLastPosition = true;
         return;
      }
      else
      {
         currTime       += mNextParticleTime;
         mInternalClock += mNextParticleTime;
         // Emit particle at curr time

         // Create particle at the correct position
         Point3F pos;
         pos.interpolate(start, end, F32(currTime) / F32(numMilliseconds));
         parentNodePos = pos;
         parentNodeBox = node->getWorldBox();
         if(addParticle(pos, axis, velocity, axisx, node))
         {
            particlesAdded = true;
            mNextParticleTime = 0;
         }
      }
   }

   while( currTime < numMilliseconds )
   {
      S32 nextTime;
      if(standAloneEmitter)
      {
         nextTime = RenderEjectionPeriodMS;
         if( sa_periodVarianceMS != 0 )
         {
            nextTime += S32(gRandGen.randI() % (2 * sa_periodVarianceMS + 1)) -
               S32(sa_periodVarianceMS);
         }
      }
      else
      {
         nextTime = RenderEjectionPeriodMS;
         if( mDataBlock->periodVarianceMS != 0 )
         {
            nextTime += S32(gRandGen.randI() % (2 * mDataBlock->periodVarianceMS + 1)) -
               S32(mDataBlock->periodVarianceMS);
         }
      }
      if(nextTime <= 0)
         AssertFatal(nextTime > 0, "Error, next particle ejection time must always be greater than 0");

      if( currTime + nextTime > numMilliseconds )
      {
         mNextParticleTime = (currTime + nextTime) - numMilliseconds;
         mInternalClock   += numMilliseconds - currTime;
         AssertFatal(mNextParticleTime > 0, "Error, should not have deferred this particle!");
         break;
      }

      currTime       += nextTime;
      mInternalClock += nextTime;

      // Create particle at the correct position
      Point3F pos;
      pos.interpolate(start, end, F32(currTime) / F32(numMilliseconds));
      parentNodePos = pos;
      parentNodeBox = node->getWorldBox();
      if(addParticle(pos, axis, velocity, axisx, node))
      {
         particlesAdded = true;
      }

      //   This override-advance code is restored in order to correctly adjust
      //   animated parameters of particles allocated within the same frame
      //   update. Note that ordering is important and this code correctly 
      //   adds particles in the same newest-to-oldest ordering of the link-list.
      //
      // NOTE: We are assuming that the just added particle is at the head of our
      //  list.  If that changes, so must this...
      U32 advanceMS = numMilliseconds - currTime;
      if (mDataBlock->overrideAdvance == false && advanceMS != 0) 
      {
         Particle* last_part = mParticleManager->part_list_head.next;
         if (advanceMS > last_part->totalLifetime) 
         {
            mParticleManager->part_list_head.next = last_part->next;
            mParticleManager->n_parts--;
            last_part->next = mParticleManager->part_freelist;
            mParticleManager->part_freelist = last_part;
         } 
         else 
         {
            if (advanceMS != 0)
            {
               F32 t = F32(advanceMS) / 1000.0;

               Point3F a = last_part->acc;
               a -= last_part->vel * last_part->dataBlock->dragCoefficient;
               a -= mWindVelocity * last_part->dataBlock->windCoefficient;
               a += Point3F(0.0f, 0.0f, -9.81f) * last_part->dataBlock->gravityCoefficient;

               last_part->vel += a * t;
               last_part->pos = last_part->pos + last_part->vel * t;

               updateKeyData( last_part );
            }
         }
      }
   }

   // DMMFIX: Lame and slow...
   if( particlesAdded == true )
      updateBBox();


   if( mParticleManager->n_parts > 0 && getSceneManager() == NULL )
   {
      gClientSceneGraph->addObjectToScene(this);
      ClientProcessList::get()->addObject(this);
   }

   mLastPosition = end;
   mHasLastPosition = true;
}

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void ParticleEmitter::emitParticles(const Point3F& rCenter,
                                    const Point3F& rNormal,
                                    const F32      radius,
                                    const Point3F& velocity,
                                    S32 count)
{
   if( mDead ) return;

   // lifetime over - no more particles
   if( mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS )
   {
      return;
   }


   Point3F axisx, axisy;
   Point3F axisz = rNormal;

   if( axisz.isZero() )
   {
      axisz.set( 0.0, 0.0, 1.0 );
   }

   if( mFabs(axisz.z) < 0.98 )
   {
      mCross(axisz, Point3F(0, 0, 1), &axisy);
      axisy.normalize();
   }
   else
   {
      mCross(axisz, Point3F(0, 1, 0), &axisy);
      axisy.normalize();
   }
   mCross(axisz, axisy, &axisx);
   axisx.normalize();

   // Should think of a better way to distribute the
   // particles within the hemisphere.
   for( S32 i = 0; i < count; i++ )
   {
      Point3F pos = axisx * (radius * (1 - (2 * gRandGen.randF())));
      pos        += axisy * (radius * (1 - (2 * gRandGen.randF())));
      pos        += axisz * (radius * gRandGen.randF());

      Point3F axis = pos;
      axis.normalize();
      pos += rCenter;

      addParticle(pos, axis, velocity, axisz, getTransform());
   }

   // Set world bounding box
   mObjBox.minExtents = rCenter - Point3F(radius, radius, radius);
   mObjBox.maxExtents = rCenter + Point3F(radius, radius, radius);
   resetWorldBox();

   // Make sure we're part of the world
   if( mParticleManager->n_parts > 0 && getSceneManager() == NULL )
   {
      gClientSceneGraph->addObjectToScene(this);
      ClientProcessList::get()->addObject(this);
   }

   mHasLastPosition = false;
}

//-----------------------------------------------------------------------------
// updateBBox - SLOW, bad news
//-----------------------------------------------------------------------------
void ParticleEmitter::updateBBox()
{
   Point3F minPt(1e10,   1e10,  1e10);
   Point3F maxPt(-1e10, -1e10, -1e10);

   for (Particle* part = mParticleManager->part_list_head.next; part != NULL; part = part->next)
   {
      Point3F particleSize(part->size * 0.5f, 0.0f, part->size * 0.5f);
      minPt.setMin( part->pos - particleSize );
      maxPt.setMax( part->pos + particleSize );
   }

   mObjBox = Box3F(minPt, maxPt);
   MatrixF temp = getTransform();
   setTransform(temp);

   mBBObjToWorld.identity();
   Point3F boxScale = mObjBox.getExtents();
   boxScale.x = getMax(boxScale.x, 1.0f);
   boxScale.y = getMax(boxScale.y, 1.0f);
   boxScale.z = getMax(boxScale.z, 1.0f);
   mBBObjToWorld.scale(boxScale);
   mBBObjToWorld.scale(getScale());
}

//-----------------------------------------------------------------------------
// processTick
//-----------------------------------------------------------------------------
void ParticleEmitter::processTick(const Move*)
{
   if( mDeleteOnTick == true )
   {
      mDead = true;
      deleteObject();
   }
   else{
      // LOD
      if(isClientObject() && !mDeleteOnTick && !mDead)
      {
         if(Con::getBoolVariable("$IPS::ParticleLOD"))
         {
            GameConnection* gConnection = GameConnection::getLocalClientConnection();
            MatrixF camTrans;
            gConnection->getControlCameraTransform(0, &camTrans);
            F32 dist = (parentNodePos - camTrans.getPosition()).len();
            if(parentNodeBox.isValidBox())
               dist = parentNodeBox.getDistanceToPoint(camTrans.getPosition());
            if(dist >= mDataBlock->SimulationLODBegin)
            {
               F32 lodCoeff = (dist - mDataBlock->SimulationLODBegin) / (mDataBlock->SimulationLODEnd - mDataBlock->SimulationLODBegin);
               MaxUpdateIndex = lodCoeff * 100;
               if(MaxUpdateIndex == 0)
                  MaxUpdateIndex = 1;
               if(MaxUpdateIndex > 100)
                  MaxUpdateIndex = U32_MAX;
            }
            if(dist >= mDataBlock->ejectionLODStartDistance)
            {
               S32 ejPeriod;
               standAloneEmitter ? ejPeriod = sa_ejectionPeriodMS : ejPeriod = mDataBlock->ejectionPeriodMS;
               if(dist >= mDataBlock->ejectionLODEndDistance)
                  mActive = false;
               else
                  mActive = true;
               F32 lodCoeff = (dist - mDataBlock->ejectionLODStartDistance) / (mDataBlock->ejectionLODEndDistance - mDataBlock->ejectionLODStartDistance);
               RenderEjectionPeriodMS = ejPeriod + ((mDataBlock->lodEjectionPeriod - ejPeriod) * lodCoeff);
            }
            else
               RenderEjectionPeriodMS = standAloneEmitter ? sa_ejectionPeriodMS : mDataBlock->ejectionPeriodMS;
         }
         else
            RenderEjectionPeriodMS = standAloneEmitter ? sa_ejectionPeriodMS : mDataBlock->ejectionPeriodMS;
      }
   }
}


//-----------------------------------------------------------------------------
// advanceTime
//-----------------------------------------------------------------------------
void ParticleEmitter::advanceTime(F32 dt)
{
   if( dt < 0.00001 ) return;

   Parent::advanceTime(dt);

   if( dt > 0.5 ) dt = 0.5;

   if( mDead ) return;

   mElapsedTimeMS += (S32)(dt * 1000.0f);

   U32 numMSToUpdate = (U32)(dt * 1000.0f);
   if( numMSToUpdate == 0 ) return;

   // TODO: Prefetch

   // remove dead particles
   mParticleManager->DeleteDeadParticles(numMSToUpdate);

   AssertFatal( mParticleManager->n_parts >= 0, "ParticleEmitter: negative part count!" );

   if (mParticleManager->n_parts < 1 && mDeleteWhenEmpty)
   {
      mDeleteOnTick = true;
      return;
   }

   if( numMSToUpdate != 0 && mParticleManager->n_parts > 0 )
   {
      update( numMSToUpdate );
   }
}

//-----------------------------------------------------------------------------
// Update key related particle data
//-----------------------------------------------------------------------------
void ParticleEmitter::updateKeyData( Particle *part, U32 numMilliseconds )
{
   mParticleManager->UpdateKeyData(part, numMilliseconds);
}

//-----------------------------------------------------------------------------
// Update particles
//-----------------------------------------------------------------------------
void ParticleEmitter::update( U32 ms )
{
   // TODO: Prefetch
   if(Con::getBoolVariable("$IPS::ParticleLOD"))
   {
      if(MaxUpdateIndex < 1000)
      {
         if(CurrentUpdateIndex == 0)
            CurrentUpdateIndex = 1;
         if(CurrentUpdateIndex < MaxUpdateIndex)
         {
            CurrentUpdateIndex++;
            TSU += ms;
            return;
         }
         if(CurrentUpdateIndex >= MaxUpdateIndex)
         {
            CurrentUpdateIndex = 1;
            ms += TSU;
            TSU = 0;
         }
      }
   }
   SimObjectPtr<IParticleBehaviour>* BHVs = standAloneEmitter == true ? ParticleBHVs : getDataBlock()->ParticleBHVs;
   //WHAT SO MANY SORTS PER SECOND? WTF DO A DIRTY FLAG!
   std::sort(BHVs, BHVs + ParticleBehaviourCount);

   for (Particle* part = mParticleManager->part_list_head.next; part != NULL; part = part->next)
   {
      F32 t = F32(ms) / 1000.0;
      part->acc.zero();

      for(int i = 0; i < ParticleBehaviourCount; i++)
      {
         IParticleBehaviour* bhv = BHVs[i];
         if(!bhv)
            continue;
         if(bhv->getType() == behaviourType::Acceleration)
            bhv->updateParticle(this, part, t);
      }

      Point3F a = part->acc;
      a -= part->vel * part->dataBlock->dragCoefficient;
      a -= mWindVelocity * part->dataBlock->windCoefficient;
      a += Point3F(0.0f, 0.0f, -9.81f) * part->dataBlock->gravityCoefficient;
      part->vel += a * t;

      for(int i = 0; i < ParticleBehaviourCount; i++)
      {
         IParticleBehaviour* bhv = BHVs[i];
         if(!bhv)
            continue;
         if(bhv->getType() == behaviourType::Velocity)
            bhv->updateParticle(this, part, t);
      }
      part->relPos += part->vel * t;
      part->pos = part->pos + part->vel * t;
      for(int i = 0; i < ParticleBehaviourCount; i++)
      {
         IParticleBehaviour* bhv = BHVs[i];
         if(!bhv)
            continue;
         if(bhv->getType() == behaviourType::Position)
            bhv->updateParticle(this, part, t);
      }

      updateKeyData( part, ms );
   }
}

bool ParticleEmitterData::reload()
{
   // Clear out current particle data.

   dataBlockIds.clear();
   particleDataBlocks.clear();

   // Parse out particle string.

   U32 numUnits = 0;
   if( particleString )
      numUnits = StringUnit::getUnitCount( particleString, " \t" );
   if( !particleString || !particleString[ 0 ] || !numUnits )
   {
      Con::errorf( "ParticleEmitterData(%s) has an empty particles string.", getName() );
      mReloadSignal.trigger();
      return false;
   }

   for( U32 i = 0; i < numUnits; ++ i )
   {
      const char* dbName = StringUnit::getUnit( particleString, i, " \t" );

      ParticleData* data = NULL;
      if( !Sim::findObject( dbName, data ) )
      {
         Con::errorf( ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find particle datablock: %s", getName(), dbName );
         continue;
      }

      particleDataBlocks.push_back( data );
      dataBlockIds.push_back( data->getId() );
   }

   // Check that we actually found some particle datablocks.

   if( particleDataBlocks.empty() )
   {
      Con::errorf( ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find any particle datablocks", getName() );
      mReloadSignal.trigger();
      return false;
   }

   // Trigger reload.

   mReloadSignal.trigger();

   return true;
}

void ParticleEmitter::onStaticModified(const char* slotName, const char*newValue)
{
   if(strcmp(slotName, "sa_ejectionOffset") == 0 ||
      strcmp(slotName, "sa_ejectionPeriodMS") == 0 ||
      strcmp(slotName, "sa_periodVarianceMS") == 0 ||
      strcmp(slotName, "sa_ejectionOffset") == 0 ||
      strcmp(slotName, "sa_ejectionVelocity") == 0 ||
      strcmp(slotName, "sa_velocityVariance") == 0 ||
      strcmp(slotName, "standAloneEmitter") == 0)
      setMaskBits(sa_Mask);

   Parent::onStaticModified(slotName, newValue);
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 ParticleEmitter::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate(con, mask, stream);

   if( stream->writeFlag( mask & sa_Mask ) )
   {
      stream->writeFlag( standAloneEmitter );
      stream->writeInt(sa_ejectionPeriodMS, 15);
      stream->writeInt(sa_periodVarianceMS, 14);

      stream->writeInt((S32)(sa_ejectionVelocity * 100), 16);
      stream->writeInt((S32)(sa_velocityVariance * 100), 14);
      stream->writeInt((S32)(sa_ejectionOffset * 100), 16);
   }

   for(int i = 0; i < ParticleBehaviourCount; i++)
   {
      if(stream->writeFlag(!ParticleBHVs[i].isNull()))
      {
         stream->writeRangedU32(ParticleBHVs[i]->getId(), 
                                 DataBlockObjectIdFirst,
                                 DataBlockObjectIdLast);
      }
   }

   return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void ParticleEmitter::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

   if(stream->readFlag())
   {
      sa_ejectionPeriodMS = stream->readInt(15);
      sa_periodVarianceMS = stream->readInt(14);

      sa_ejectionVelocity = stream->readInt(16) / 100.0f;
      sa_velocityVariance = stream->readInt(14) / 100.0f;
      sa_ejectionOffset = stream->readInt(16) / 100.0f;
   }

   for(int i = 0; i < ParticleBehaviourCount; i++)
   {
      if ( stream->readFlag() )
      {
         SimDataBlock *dptr = 0;
         SimObjectId id = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
         if(	Sim::findObject( id, dptr )	)
            ParticleBHVs[i] = (IParticleBehaviour*)dptr;
      }
   }
}

DefineEngineMethod(ParticleEmitterData, reload, void,(),,
                   "Reloads the ParticleData datablocks and other fields used by this emitter.\n"
                   "@tsexample\n"
                   "// Get the editor's current particle emitter\n"
                   "%emitter = PE_EmitterEditor.currEmitter\n\n"
                   "// Change a field value\n"
                   "%emitter.setFieldValue( %propertyField, %value );\n\n"
                   "// Reload this emitter\n"
                   "%emitter.reload();\n"
                   "@endtsexample\n")
{
   object->reload();
}