//-----------------------------------------------------------------------------
// IPS Pro
// Copyright Lukas Jørgensen 2012 - FuzzyVoidStudio
// License:
// 'the product' refers to the IPS Pro.
// If you have not paid for the product you are not allowed to use it
//  - personally nor commercially.
// Assuming you have paid for the product you are allowed to
//  - include this in any commercial product as long as the IPS Pro
//  - source code is never released along with the commercial product.
// You may not resell or redistribute the product.
// You'll need one license for each man on your team, unless you fall under
//  - the indie license.
// You may not use the code to create new products in the same category.
//  - e.g. you may not use this code to construct a mathEmitter and sell that
//  - as a new product. Not if it is released for another GameEngine either.
// The indie license: As long as your products using IPS Pro has a revenue of
//  - less than 40.000$ annually you fall under this license.
// Email me at LukasPJ@FuzzyVoidStudio.com 
//  - if you have further questions regarding license
// http://fuzzyvoidstudio.com
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "RadiusMeshEmitter.h"

#include "scene/sceneManager.h"
#include "scene/sceneRenderState.h"
#include "console/consoleTypes.h"
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
#include "gfx/gfxDrawUtil.h"

#include "T3D\gameBase\gameConnection.h"

#include "math/mathIO.h"
#include <time.h>

#include "gfx/sim/debugDraw.h"

#include "ts\tsShapeInstance.h"
#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "T3D\staticShape.h"
#include "ts\TSMeshIntrinsics.h"

#include "IPSCore.h"
#include "IPSDebugRenderHelper.h"
#include "IPSEmitterHelper.h"

#include <string>
#include <vector>

#if defined(TORQUE_OS_XENON)
#  include "gfx/D3D9/360/gfx360MemVertexBuffer.h"
#endif

Point3F RadiusMeshEmitter::mWindVelocity( 0.0, 0.0, 0.0 );
const F32 RadiusMeshEmitter::AgedSpinToRadians = (1.0f/1000.0f) * (1.0f/360.0f) * M_PI_F * 2.0f;

IMPLEMENT_CO_DATABLOCK_V1(RadiusMeshEmitterData);
//IMPLEMENT_CONOBJECT
IMPLEMENT_CO_NETOBJECT_V1(RadiusMeshEmitter);

ConsoleDocClass( RadiusMeshEmitter,
	"@brief This object is responsible for spawning particles.\n\n"

	"@note This class is not normally instantiated directly - to place a simple "
	"particle emitting object in the scene, use a RadiusMeshEmitterNode instead.\n\n"

	"This class is the main interface for creating particles - though it is "
	"usually only accessed from within another object like RadiusMeshEmitterNode "
	"or WheeledVehicle. If using this object class (via C++) directly, be aware "
	"that it does <b>not</b> track changes in source axis or velocity over the "
	"course of a single update, so emitParticles should be called at a fairly "
	"fine grain.  The emitter will potentially track the last particle to be "
	"created into the next call to this function in order to create a uniformly "
	"random time distribution of the particles.\n\n"

	"If the object to which the emitter is attached is in motion, it should try "
	"to ensure that for call (n+1) to this function, start is equal to the end "
	"from call (n). This will ensure a uniform spatial distribution.\n\n"

	"@ingroup FX\n"
	"@see RadiusMeshEmitterData\n"
	"@see RadiusMeshEmitterNode\n"
	);

ConsoleDocClass( RadiusMeshEmitterData,
	"@brief Defines particle emission properties such as ejection angle, period "
	"and velocity for a RadiusMeshEmitter.\n\n"

	"@tsexample\n"
	"datablock RadiusMeshEmitterData( GrenadeExpDustEmitter )\n"
	"{\n"
	"   ejectionPeriodMS = 1;\n"
	"   periodVarianceMS = 0;\n"
	"   ejectionVelocity = 15;\n"
	"   velocityVariance = 0.0;\n"
	"   ejectionOffset = 0.0;\n"
	"   thetaMin = 85;\n"
	"   thetaMax = 85;\n"
	"   phiReferenceVel = 0;\n"
	"   phiVariance = 360;\n"
	"   overrideAdvance = false;\n"
	"   lifetimeMS = 200;\n"
	"   particles = \"GrenadeExpDust\";\n"
	"};\n"
	"@endtsexample\n\n"

	"@ingroup FX\n"
	"@see RadiusMeshEmitter\n"
	"@see ParticleData\n"
	"@see RadiusMeshEmitterNode\n"
	);

static const float sgDefaultEjectionOffset = 0.f;
static const float sgDefaultPhiReferenceVel = 0.f;
static const float sgDefaultPhiVariance = 360.f;

//-----------------------------------------------------------------------------
// RadiusMeshEmitterData
// Changed
//-----------------------------------------------------------------------------
RadiusMeshEmitterData::RadiusMeshEmitterData()
{
	VECTOR_SET_ASSOCIATION(particleDataBlocks);
	VECTOR_SET_ASSOCIATION(dataBlockIds);

	ejectionPeriodMS = 100;    // 10 Particles Per second
	periodVarianceMS = 0;      // exactly

	ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	velocityVariance = 1.0f;
	ejectionOffset   = sgDefaultEjectionOffset;   // ejection from the emitter point

	softnessDistance = 1.0f;
	ambientFactor = 0.0f;

	lifetimeMS           = 0;
	lifetimeVarianceMS   = 0;

	overrideAdvance  = true;
	orientParticles  = false;
	orientOnVelocity = true;
	useEmitterSizes  = false;
	useEmitterColors = false;
	particleString   = NULL;
	partListInitSize = 0;

	// These members added for support of user defined blend factors
	// and optional particle sorting.
	blendStyle = ParticleRenderInst::BlendUndefined;
	sortParticles = false;
	renderReflection = true;
	reverseOrder = false;
	textureName = 0;
	textureHandle = 0;
	highResOnly = true;

	alignParticles = false;
	alignDirection = Point3F(0.0f, 1.0f, 0.0f);
}

// Enum tables used for fields blendStyle, srcBlendFactor, dstBlendFactor.
// Note that the enums for srcBlendFactor and dstBlendFactor are consistent
// with the blending enums used in Torque Game Builder.


typedef ParticleRenderInst::BlendStyle rmParticleBlendStyle;
DefineEnumType( rmParticleBlendStyle );

typedef RadiusMeshEmitter::EnumAttractionMode rmAttractionMode;
DefineEnumType( rmAttractionMode );

ImplementEnumType( rmAttractionMode,
	"The way the particles are interacting with specific objects.\n"
	"@ingroup FX\n\n")
{ RadiusMeshEmitter::none,		"Not attracted",        "The particles are not attracted to any object.\n" },
{ RadiusMeshEmitter::attract,		"Attract",				"The particles are attracted to the objects.\n" },
{ RadiusMeshEmitter::repulse,		"Repulse",				"The particles are repulsed by the object.\n" },
EndImplementEnumType;

//-----------------------------------------------------------------------------
// initPersistFields
// Changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitterData::initPersistFields()
{

	addGroup( "RadiusMeshEmitterData" );

	addField("ejectionPeriodMS", TYPEID< S32 >(), Offset(ejectionPeriodMS,   RadiusMeshEmitterData),
		"Time (in milliseconds) between each particle ejection." );

	addField("periodVarianceMS", TYPEID< S32 >(), Offset(periodVarianceMS,   RadiusMeshEmitterData),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "ejectionVelocity", TYPEID< F32 >(), Offset(ejectionVelocity, RadiusMeshEmitterData),
		"Particle ejection velocity." );

	addField( "velocityVariance", TYPEID< F32 >(), Offset(velocityVariance, RadiusMeshEmitterData),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "ejectionOffset", TYPEID< F32 >(), Offset(ejectionOffset, RadiusMeshEmitterData),
		"Distance along ejection Z axis from which to eject particles." );

	addField( "softnessDistance", TYPEID< F32 >(), Offset(softnessDistance, RadiusMeshEmitterData),
		"For soft particles, the distance (in meters) where particles will be "
		"faded based on the difference in depth between the particle and the "
		"scene geometry." );

	addField( "ambientFactor", TYPEID< F32 >(), Offset(ambientFactor, RadiusMeshEmitterData),
		"Used to generate the final particle color by controlling interpolation "
		"between the particle color and the particle color multiplied by the "
		"ambient light color." );

	addField( "overrideAdvance", TYPEID< bool >(), Offset(overrideAdvance, RadiusMeshEmitterData),
		"If false, particles emitted in the same frame have their positions "
		"adjusted. If true, adjustment is skipped and particles will clump "
		"together." );

	addField( "orientParticles", TYPEID< bool >(), Offset(orientParticles, RadiusMeshEmitterData),
		"If true, Particles will always face the camera." );

	addField( "orientOnVelocity", TYPEID< bool >(), Offset(orientOnVelocity, RadiusMeshEmitterData),
		"If true, particles will be oriented to face in the direction they are moving." );

	addField( "particles", TYPEID< StringTableEntry >(), Offset(particleString, RadiusMeshEmitterData),
		"@brief List of space or TAB delimited ParticleData datablock names.\n\n"
		"A random one of these datablocks is selected each time a particle is "
		"emitted." );

	addField( "lifetimeMS", TYPEID< S32 >(), Offset(lifetimeMS, RadiusMeshEmitterData),
		"Lifetime of emitted particles (in milliseconds)." );

	addField("lifetimeVarianceMS", TYPEID< S32 >(), Offset(lifetimeVarianceMS, RadiusMeshEmitterData),
		"Variance in particle lifetime from 0 - lifetimeMS." );

	addField( "useEmitterSizes", TYPEID< bool >(), Offset(useEmitterSizes, RadiusMeshEmitterData),
		"@brief If true, use emitter specified sizes instead of datablock sizes.\n"
		"Useful for Debris particle emitters that control the particle size." );

	addField( "useEmitterColors", TYPEID< bool >(), Offset(useEmitterColors, RadiusMeshEmitterData),
		"@brief If true, use emitter specified colors instead of datablock colors.\n\n"
		"Useful for ShapeBase dust and WheeledVehicle wheel particle emitters that use "
		"the current material to control particle color." );

	/// These fields added for support of user defined blend factors and optional particle sorting.
	//@{

	addField( "blendStyle", TYPEID< ParticleRenderInst::BlendStyle >(), Offset(blendStyle, RadiusMeshEmitterData),
		"String value that controls how emitted particles blend with the scene." );

	addField( "sortParticles", TYPEID< bool >(), Offset(sortParticles, RadiusMeshEmitterData),
		"If true, particles are sorted furthest to nearest.");

	addField( "reverseOrder", TYPEID< bool >(), Offset(reverseOrder, RadiusMeshEmitterData),
		"@brief If true, reverses the normal draw order of particles.\n\n"
		"Particles are normally drawn from newest to oldest, or in Z order "
		"(furthest first) if sortParticles is true. Setting this field to "
		"true will reverse that order: oldest first, or nearest first if "
		"sortParticles is true." );

	addField( "textureName", TYPEID< StringTableEntry >(), Offset(textureName, RadiusMeshEmitterData),
		"Optional texture to override ParticleData::textureName." );

	addField( "alignParticles", TYPEID< bool >(), Offset(alignParticles, RadiusMeshEmitterData),
		"If true, particles always face along the axis defined by alignDirection." );

	addProtectedField( "alignDirection", TYPEID< Point3F>(), Offset(alignDirection, RadiusMeshEmitterData), &RadiusMeshEmitterData::_setAlignDirection, &defaultProtectedGetFn,
		"The direction aligned particles should face, only valid if alignParticles is true." );

	addField( "highResOnly", TYPEID< bool >(), Offset(highResOnly, RadiusMeshEmitterData),
		"This particle system should not use the mixed-resolution renderer. "
		"If your particle system has large amounts of overdraw, consider "
		"disabling this option." );

	addField( "renderReflection", TYPEID< bool >(), Offset(renderReflection, RadiusMeshEmitterData),
		"Controls whether particles are rendered onto reflective surfaces like water." );

	//@}

	endGroup( "RadiusMeshEmitterData" );
	/*
	addField( "Sticky", TYPEID< bool >(), Offset(sticky, RadiusMeshEmitterData),
	"If true then bla." );*/

	Parent::initPersistFields();
}

bool RadiusMeshEmitterData::_setAlignDirection( void *object, const char *index, const char *data )
{
	RadiusMeshEmitterData *p = static_cast<RadiusMeshEmitterData*>( object );

	Con::setData( TypePoint3F, &p->alignDirection, 0, 1, &data );
	p->alignDirection.normalizeSafe();

	// we already set the field
	return false;
}

//-----------------------------------------------------------------------------
// packData
// Changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitterData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->writeInt(ejectionPeriodMS, 10);
	stream->writeInt(periodVarianceMS, 10);
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
	stream->writeFlag(useEmitterSizes);
	stream->writeFlag(useEmitterColors);

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
}

//-----------------------------------------------------------------------------
// unpackData
// Changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitterData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	ejectionPeriodMS = stream->readInt(10);
	periodVarianceMS = stream->readInt(10);
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
	useEmitterSizes = stream->readFlag();
	useEmitterColors = stream->readFlag();

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
}

//-----------------------------------------------------------------------------
// onAdd
// Not changed
//-----------------------------------------------------------------------------
bool RadiusMeshEmitterData::onAdd()
{
	if( Parent::onAdd() == false )
		return false;

	//   if (overrideAdvance == true) {
	//      Con::errorf(ConsoleLogEntry::General, "RadiusMeshEmitterData: Not going to work.  Fix it!");
	//      return false;
	//   }

	// Validate the parameters...
	//
	if( ejectionPeriodMS < 1 )
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) period < 1 ms", getName());
		ejectionPeriodMS = 1;
	}
	if( periodVarianceMS >= ejectionPeriodMS )
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) periodVariance >= period", getName());
		periodVarianceMS = ejectionPeriodMS - 1;
	}
	if( ejectionVelocity < 0.0f )
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) ejectionVelocity < 0.0f", getName());
		ejectionVelocity = 0.0f;
	}
	if( velocityVariance < 0.0f )
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) velocityVariance < 0.0f", getName());
		velocityVariance = 0.0f;
	}
	if( velocityVariance > ejectionVelocity )
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) velocityVariance > ejectionVelocity", getName());
		velocityVariance = ejectionVelocity;
	}
	if( ejectionOffset < 0.0f )
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) ejectionOffset < 0", getName());
		ejectionOffset = 0.0f;
	}

	if ( softnessDistance < 0.0f )
	{
		Con::warnf( ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) invalid softnessDistance", getName() );
		softnessDistance = 0.0f;
	}

	if (particleString == NULL && dataBlockIds.size() == 0) 
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) no particleString, invalid datablock", getName());
		return false;
	}
	if (particleString && particleString[0] == '\0') 
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) no particleString, invalid datablock", getName());
		return false;
	}
	if (particleString && dStrlen(particleString) > 255) 
	{
		Con::errorf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) particle string too long [> 255 chars]", getName());
		return false;
	}

	if( lifetimeMS < 0 )
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) lifetimeMS < 0.0f", getName());
		lifetimeMS = 0;
	}
	if( lifetimeVarianceMS > lifetimeMS )
	{
		Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) lifetimeVarianceMS >= lifetimeMS", getName());
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
			Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) invalid particles string.  No datablocks found", getName());
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
				Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) unable to find particle datablock: %s", getName(), dataBlocks[i]);
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
			Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) unable to find any particle datablocks", getName());
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// preload
// Not changed
//-----------------------------------------------------------------------------
bool RadiusMeshEmitterData::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;

	particleDataBlocks.clear();
	for (U32 i = 0; i < dataBlockIds.size(); i++) 
	{
		ParticleData* pData = NULL;
		if (Sim::findObject(dataBlockIds[i], pData) == false)
			Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) unable to find particle datablock: %d", getName(), dataBlockIds[i]);
		else
			particleDataBlocks.push_back(pData);
	}

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
			StringTableEntry txr_name = particleDataBlocks[0]->textureName;
			for (S32 i = 1; i < particleDataBlocks.size(); i++)
			{
				// warn if particle textures are inconsistent
				if (particleDataBlocks[i]->textureName != txr_name)
				{
					Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) particles reference different textures.", getName());
					break;
				}
			}
		}
	}

	// if blend-style is undefined check legacy useInvAlpha settings
	if (blendStyle == ParticleRenderInst::BlendUndefined && particleDataBlocks.size() > 0)
	{
		bool useInvAlpha = particleDataBlocks[0]->useInvAlpha;
		for (S32 i = 1; i < particleDataBlocks.size(); i++)
		{
			// warn if blend-style legacy useInvAlpha settings are inconsistent
			if (particleDataBlocks[i]->useInvAlpha != useInvAlpha)
			{
				Con::warnf(ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) particles have inconsistent useInvAlpha settings.", getName());
				break;
			}
		}
		blendStyle = (useInvAlpha) ? ParticleRenderInst::BlendNormal : ParticleRenderInst::BlendAdditive;
	}

	if( !server )
	{
		allocPrimBuffer();
	}

	return true;
}

//-----------------------------------------------------------------------------
// alloc PrimitiveBuffer
// The datablock allocates this static index buffer because it's the same
// for all of the emitters - each particle quad uses the same index ordering
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitterData::allocPrimBuffer( S32 overrideSize )
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
// RadiusMeshEmitter
// Changed
//-----------------------------------------------------------------------------
RadiusMeshEmitter::RadiusMeshEmitter()
{
	// RadiusMeshEmitter serves as both a node and an emitter
	// - Therefore it has to be on both server and client.
	mNetFlags.set(ScopeAlways | Ghostable);

	mTypeMask |= EnvironmentObjectType;

	mDeleteWhenEmpty  = false;
	mDeleteOnTick     = false;

	mInternalClock    = 0;
	mNextParticleTime = 0;

	mLastPosition.set(0, 0, 0);
	mHasLastPosition = false;

	mLifetimeMS = 0;
	mElapsedTimeMS = 0;

	part_store = 0;
	part_freelist = NULL;
	part_list_head.next = NULL;
	n_part_capacity = 0;
	n_parts = 0;

	mCurBuffSize = 0;

	mDead = false;

	mainTime = NULL;

	// Standard particle variables
	//  - These should maybe be removed her and handled in datablock only
	ejectionPeriodMS = 100;    // 10 Particles Per second
	periodVarianceMS = 0;      // exactly

	ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	velocityVariance = 1.0f;
	ejectionOffset   = sgDefaultEjectionOffset;   // ejection from the emitter point

	softnessDistance = 1.0f;
	ambientFactor = 0.0f;

	lifetimeMS           = 0;
	lifetimeVarianceMS   = 0;

	overrideAdvance  = true;
	orientParticles  = false;
	orientOnVelocity = true;
	useEmitterSizes  = false;
	useEmitterColors = false;

	// These members added for support of user defined blend factors
	// and optional particle sorting.
	blendStyle = ParticleRenderInst::BlendUndefined;
	sortParticles = false;
	renderReflection = true;
	reverseOrder = false;
	textureName = 0;
	textureHandle = 0;
	highResOnly = true;

	alignParticles = false;
	alignDirection = Point3F(0.0f, 1.0f, 0.0f);

	// RadiusMesh variables
	evenEmission = true;
	emitOnFaces = true;
	emitMesh = "";
	vertexCount = 0;
	center = Point3F(0,0,0);
	radius = 3;
	active = true;

	// Debug variables
	renderIntersections = false;
	renderEmittingFaces = false;
	shadeByCoverage = false;
	renderSphere = false;
	enableDebugRender = false;

	// Physics variables
	sticky = false;
	attractionrange = 50;
	attractionrange = 50;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = 0;
		Amount[i] = 1;
		Attraction_offset[i] = "0 0 0";
		attractedObjectID[i] = "";
	}
}

//-----------------------------------------------------------------------------
// destructor
// Not changed
//-----------------------------------------------------------------------------
RadiusMeshEmitter::~RadiusMeshEmitter()
{
	if(isClientObject())
	{
		for( S32 i = 0; i < part_store.size(); i++ )
		{
			delete [] part_store[i];
		}
	}
}

//-----------------------------------------------------------------------------
// initPersistFields
// Changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::initPersistFields()
{

	addGroup( "RadiusMeshEmitter" );

	addField( "active", TYPEID< bool >(), Offset(active, RadiusMeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	addField( "emitMesh", TYPEID< StringTableEntry >(), Offset(emitMesh, RadiusMeshEmitter),
		"The object that the emitter will use to emit particles on." );

	addField( "evenEmission", TYPEID< bool >(), Offset(evenEmission, RadiusMeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	addField( "emitOnFaces", TYPEID< bool >(), Offset(emitOnFaces, RadiusMeshEmitter),
		"If true, particles will be emitted along the faces of the RadiusMesh. "
		"If false, particles will be emitted along the vertices of RadiusMesh. " );

	addField( "center", TYPEID< Point3F >(), Offset(center, RadiusMeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	addField( "radius", TYPEID< F32 >(), Offset(radius, RadiusMeshEmitter),
		"If true, particles will be emitted along the faces of the RadiusMesh. "
		"If false, particles will be emitted along the vertices of RadiusMesh. " );

	endGroup( "RadiusMeshEmitter" );

	addGroup( "Debug" );

	addField( "renderIntersections", TYPEID< bool >(), Offset(renderIntersections, RadiusMeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	addField( "renderEmittingFaces", TYPEID< bool >(), Offset(renderEmittingFaces, RadiusMeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	addField( "renderSphere", TYPEID< bool >(), Offset(renderSphere, RadiusMeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	addField( "shadeByCoverage", TYPEID< bool >(), Offset(shadeByCoverage, RadiusMeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	addField( "enableDebugRender", TYPEID< bool >(), Offset(enableDebugRender, RadiusMeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	endGroup( "Debug" );

	addGroup( "Particles" );

	addField("ejectionPeriodMS", TYPEID< S32 >(), Offset(ejectionPeriodMS,   RadiusMeshEmitter),
		"Time (in milliseconds) between each particle ejection." );

	addField("periodVarianceMS", TYPEID< S32 >(), Offset(periodVarianceMS,   RadiusMeshEmitter),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "ejectionVelocity", TYPEID< F32 >(), Offset(ejectionVelocity, RadiusMeshEmitter),
		"Particle ejection velocity." );

	addField( "velocityVariance", TYPEID< F32 >(), Offset(velocityVariance, RadiusMeshEmitter),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "ejectionOffset", TYPEID< F32 >(), Offset(ejectionOffset, RadiusMeshEmitter),
		"Distance along ejection Z axis from which to eject particles." );

	addField( "softnessDistance", TYPEID< F32 >(), Offset(softnessDistance, RadiusMeshEmitter),
		"For soft particles, the distance (in meters) where particles will be "
		"faded based on the difference in depth between the particle and the "
		"scene geometry." );

	addField( "ambientFactor", TYPEID< F32 >(), Offset(ambientFactor, RadiusMeshEmitter),
		"Used to generate the final particle color by controlling interpolation "
		"between the particle color and the particle color multiplied by the "
		"ambient light color." );

	addField( "overrideAdvance", TYPEID< bool >(), Offset(overrideAdvance, RadiusMeshEmitter),
		"If false, particles emitted in the same frame have their positions "
		"adjusted. If true, adjustment is skipped and particles will clump "
		"together." );

	addField( "orientParticles", TYPEID< bool >(), Offset(orientParticles, RadiusMeshEmitter),
		"If true, Particles will always face the camera." );

	addField( "orientOnVelocity", TYPEID< bool >(), Offset(orientOnVelocity, RadiusMeshEmitter),
		"If true, particles will be oriented to face in the direction they are moving." );

	addField( "lifetimeMS", TYPEID< S32 >(), Offset(lifetimeMS, RadiusMeshEmitter),
		"Lifetime of emitted particles (in milliseconds)." );

	addField("lifetimeVarianceMS", TYPEID< S32 >(), Offset(lifetimeVarianceMS, RadiusMeshEmitter),
		"Variance in particle lifetime from 0 - lifetimeMS." );

	addField( "useEmitterSizes", TYPEID< bool >(), Offset(useEmitterSizes, RadiusMeshEmitter),
		"@brief If true, use emitter specified sizes instead of datablock sizes.\n"
		"Useful for Debris particle emitters that control the particle size." );

	addField( "useEmitterColors", TYPEID< bool >(), Offset(useEmitterColors, RadiusMeshEmitter),
		"@brief If true, use emitter specified colors instead of datablock colors.\n\n"
		"Useful for ShapeBase dust and WheeledVehicle wheel particle emitters that use "
		"the current material to control particle color." );

	/// These fields added for support of user defined blend factors and optional particle sorting.
	//@{

	addField( "blendStyle", TYPEID< ParticleRenderInst::BlendStyle >(), Offset(blendStyle, RadiusMeshEmitter),
		"String value that controls how emitted particles blend with the scene." );

	addField( "sortParticles", TYPEID< bool >(), Offset(sortParticles, RadiusMeshEmitter),
		"If true, particles are sorted furthest to nearest.");

	addField( "reverseOrder", TYPEID< bool >(), Offset(reverseOrder, RadiusMeshEmitter),
		"@brief If true, reverses the normal draw order of particles.\n\n"
		"Particles are normally drawn from newest to oldest, or in Z order "
		"(furthest first) if sortParticles is true. Setting this field to "
		"true will reverse that order: oldest first, or nearest first if "
		"sortParticles is true." );

	addField( "textureName", TYPEID< StringTableEntry >(), Offset(textureName, RadiusMeshEmitter),
		"Optional texture to override ParticleData::textureName." );

	addField( "alignParticles", TYPEID< bool >(), Offset(alignParticles, RadiusMeshEmitter),
		"If true, particles always face along the axis defined by alignDirection." );

	addProtectedField( "alignDirection", TYPEID< Point3F>(), Offset(alignDirection, RadiusMeshEmitter), &RadiusMeshEmitter::_setAlignDirection, &defaultProtectedGetFn,
		"The direction aligned particles should face, only valid if alignParticles is true." );

	addField( "highResOnly", TYPEID< bool >(), Offset(highResOnly, RadiusMeshEmitter),
		"This particle system should not use the mixed-resolution renderer. "
		"If your particle system has large amounts of overdraw, consider "
		"disabling this option." );

	addField( "renderReflection", TYPEID< bool >(), Offset(renderReflection, RadiusMeshEmitter),
		"Controls whether particles are rendered onto reflective surfaces like water." );

	endGroup( "Particles" );

	addGroup( "Physics" );

	addField( "attractedObjectID", TYPEID< StringTableEntry >(), Offset(attractedObjectID, RadiusMeshEmitter), attrobjectCount,
		"The ID or name of the object that the particles should interact with." );

	addField( "Attraction_offset", TYPEID< StringTableEntry >(), Offset(Attraction_offset, RadiusMeshEmitter), attrobjectCount,
		"Offset from the objects position the particles should be attracted to or repulsed from." );

	addField( "AttractionMode", TYPEID< rmAttractionMode >(), Offset(AttractionMode, RadiusMeshEmitter), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, RadiusMeshEmitter), attrobjectCount,
		"Amount of influence, combine with attraction range for the desired result." );

	addField( "attractionrange", TYPEID< F32 >(), Offset(attractionrange, RadiusMeshEmitter),
		"Range of influence, any objects further away than this length will not attract or repulse the particles." );

	addField( "sticky", TYPEID< bool >(), Offset(sticky, RadiusMeshEmitter),
		"If true, the particles will stick to their original spawn place relative to the emitter." );

	endGroup( "Physics" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// _setAlignDirection
// Not changed
//-----------------------------------------------------------------------------
bool RadiusMeshEmitter::_setAlignDirection( void *object, const char *index, const char *data )
{
	RadiusMeshEmitter *p = static_cast<RadiusMeshEmitter*>( object );

	Con::setData( TypePoint3F, &p->alignDirection, 0, 1, &data );
	p->alignDirection.normalizeSafe();

	// we already set the field
	return false;
}

//-----------------------------------------------------------------------------
// onAdd
// Not changed
//-----------------------------------------------------------------------------
bool RadiusMeshEmitter::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if(isServerObject())
		setMaskBits( StateMask );

	F32 radius = 0.5;
	mObjBox.minExtents = Point3F(-radius, -radius, -radius);
	mObjBox.maxExtents = Point3F(radius, radius, radius);
	resetWorldBox();
	addToScene();

	return true;
}


//-----------------------------------------------------------------------------
// onRemove
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::onRemove()
{
	removeFromScene();
	mDeleteWhenEmpty = true;
	Parent::onRemove();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
// Changed
//-----------------------------------------------------------------------------
bool RadiusMeshEmitter::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	mDataBlock = dynamic_cast<RadiusMeshEmitterData*>( dptr );
	if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	mLifetimeMS = lifetimeMS;
	if( lifetimeVarianceMS )
	{
		mLifetimeMS += S32( gRandGen.randI() % (2 * lifetimeVarianceMS + 1)) - S32(lifetimeVarianceMS );
	}

	//   Allocate particle structures and init the freelist. Member part_store
	//   is a Vector so that we can allocate more particles if partListInitSize
	//   turns out to be too small. 
	//
	if (mDataBlock->partListInitSize > 0)
	{
		for( S32 i = 0; i < part_store.size(); i++ )
		{
			delete [] part_store[i];
		}
		part_store.clear();
		n_part_capacity = mDataBlock->partListInitSize;
		Particle* store_block = new Particle[n_part_capacity];
		part_store.push_back(store_block);
		part_freelist = store_block;
		Particle* last_part = part_freelist;
		Particle* part = last_part+1;
		for( S32 i = 1; i < n_part_capacity; i++, part++, last_part++ )
		{
			last_part->next = part;
		}
		store_block[n_part_capacity-1].next = NULL;
		part_list_head.next = NULL;
		n_parts = 0;
	}

	// Copy values from DB -----
	ejectionPeriodMS = mDataBlock->ejectionPeriodMS;    // 10 Particles Per second
	periodVarianceMS = mDataBlock->periodVarianceMS;      // exactly

	ejectionVelocity = mDataBlock->ejectionVelocity;   // From 1.0 - 3.0 meters per sec
	velocityVariance = mDataBlock->velocityVariance;
	ejectionOffset   = mDataBlock->ejectionOffset;   // ejection from the emitter point

	softnessDistance = mDataBlock->softnessDistance;
	ambientFactor	 = mDataBlock->ambientFactor;

	lifetimeMS           = mDataBlock->lifetimeMS;
	lifetimeVarianceMS   = mDataBlock->lifetimeVarianceMS;

	overrideAdvance  = mDataBlock->overrideAdvance;
	orientParticles  = mDataBlock->orientParticles;
	orientOnVelocity = mDataBlock->orientOnVelocity;
	useEmitterSizes  = mDataBlock->useEmitterSizes;
	useEmitterColors = mDataBlock->useEmitterColors;

	// These members added for support of user defined blend factors
	// and optional particle sorting.
	blendStyle = mDataBlock->blendStyle;
	sortParticles = mDataBlock->sortParticles;
	renderReflection = mDataBlock->renderReflection;
	reverseOrder = mDataBlock->reverseOrder;
	textureName = mDataBlock->textureName;
	textureHandle = mDataBlock->textureHandle;
	highResOnly = mDataBlock->highResOnly;

	alignParticles = mDataBlock->alignParticles;
	alignDirection = mDataBlock->alignDirection;
	// ----- Copy values from DB

	for(int i = 0; i < initialValues.size(); i=i+2)
	{
		// Should these be datablock only values?
		if(strcmp("ejectionPeriodMS",initialValues[i].c_str()) == 0)
			ejectionPeriodMS = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("periodVarianceMS",initialValues[i].c_str()) == 0)
			periodVarianceMS = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("ejectionVelocity",initialValues[i].c_str()) == 0)
			ejectionVelocity = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("velocityVariance",initialValues[i].c_str()) == 0)
			velocityVariance = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("ejectionOffset",initialValues[i].c_str()) == 0)
			ejectionOffset = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("softnessDistance",initialValues[i].c_str()) == 0)
			softnessDistance = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("ambientFactor",initialValues[i].c_str()) == 0)
			ambientFactor = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("lifetimeMS",initialValues[i].c_str()) == 0)
			lifetimeMS = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("center",initialValues[i].c_str()) == 0){
			Point3F point;
			dSscanf(initialValues[i+1].c_str(), "%f %f %f", &point.x, &point.y, &point.z);
			center = point;
		}
		if(strcmp("radius",initialValues[i].c_str()) == 0)
			radius = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("emitMesh",initialValues[i].c_str()) == 0)
			emitMesh = const_cast<char*>(initialValues[i+1].c_str());
		if(strcmp("evenEmission",initialValues[i].c_str()) == 0)
			evenEmission = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("emitOnFaces",initialValues[i].c_str()) == 0)
			emitOnFaces = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("renderIntersections",initialValues[i].c_str()) == 0)
			renderIntersections = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("renderEmittingFaces",initialValues[i].c_str()) == 0)
			renderEmittingFaces = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("renderSphere",initialValues[i].c_str()) == 0)
			renderSphere = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("shadeByCoverage",initialValues[i].c_str()) == 0)
			shadeByCoverage = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("enableDebugRender",initialValues[i].c_str()) == 0)
			enableDebugRender = atoi(const_cast<char*>(initialValues[i+1].c_str()));

		// These values can de defined on a per-emitter basis and therefore
		//  - We have to reload them here or the datablock will override them.
		if(strcmp("attractionrange",initialValues[i].c_str()) == 0)
			attractionrange = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sticky",initialValues[i].c_str()) == 0)
			sticky = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Attraction_offset",initialValues[i].c_str()) == 0)
		{
			Attraction_offset[0] = const_cast<char*>(initialValues[i+1].c_str());
			Attraction_offset[1] = const_cast<char*>(initialValues[i+2].c_str());
			i++;
		}
		if(strcmp("attractedObjectID",initialValues[i].c_str()) == 0)
		{
			attractedObjectID[0] = const_cast<char*>(initialValues[i+1].c_str());
			attractedObjectID[1] = const_cast<char*>(initialValues[i+2].c_str());
			i++;
		}
		if(strcmp("AttractionMode",initialValues[i].c_str()) == 0)
		{
			AttractionMode[0] = atoi(const_cast<char*>(initialValues[i+1].c_str()));
			AttractionMode[1] = atoi(const_cast<char*>(initialValues[i+2].c_str()));
			i++;
		}
		if(strcmp("Amount",initialValues[i].c_str()) == 0)
		{
			Amount[0] = atoi(const_cast<char*>(initialValues[i+1].c_str()));
			Amount[1] = atoi(const_cast<char*>(initialValues[i+2].c_str()));
			i++;
		}
	}
	//initialValues.clear();
	scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// getCollectiveColor
// Not changed
//-----------------------------------------------------------------------------
ColorF RadiusMeshEmitter::getCollectiveColor()
{
	U32 count = 0;
	ColorF color = ColorF(0.0f, 0.0f, 0.0f);

	count = n_parts;
	for( Particle* part = part_list_head.next; part != NULL; part = part->next )
	{
		color += part->color;
	}

	if(count > 0)
	{
		color /= F32(count);
	}

	//if(color.red == 0.0f && color.green == 0.0f && color.blue == 0.0f)
	//	color = color;

	return color;
}

//-----------------------------------------------------------------------------
// prepRenderImage
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::prepRenderImage(SceneRenderState* state)
{
	if(enableDebugRender)
	{
		ObjectRenderInst *ori = state->getRenderPass()->allocInst<ObjectRenderInst>();
		ori->renderDelegate.bind( this, &RadiusMeshEmitter::debugRender );
		ori->type = RenderPassManager::RIT_Editor;
		state->getRenderPass()->addInst(ori);
	}

	if( state->isReflectPass() && !getDataBlock()->renderReflection )
		return;

	// Never render into shadows.
	if (state->isShadowPass())
		return;

	PROFILE_SCOPE(RadiusMeshEmitter_prepRenderImage);

	if (  mDead ||
		n_parts == 0 || 
		part_list_head.next == NULL )
		return;

	RenderPassManager *renderManager = state->getRenderPass();
	const Point3F &camPos = state->getCameraPosition();
	copyToVB( camPos, state->getAmbientLightColor() );

	if (!mVertBuff.isValid())
		return;

	ParticleRenderInst *ri = renderManager->allocInst<ParticleRenderInst>();

	ri->vertBuff = &mVertBuff;
	ri->primBuff = &getDataBlock()->primBuff;
	ri->translucentSort = true;
	ri->type = RenderPassManager::RIT_Particle;
	ri->sortDistSq = getRenderWorldBox().getSqDistanceToPoint( camPos );

	// Draw the system offscreen unless the highResOnly flag is set on the datablock
	ri->systemState = ( getDataBlock()->highResOnly ? PSS_AwaitingHighResDraw : PSS_AwaitingOffscreenDraw );

	ri->modelViewProj = renderManager->allocUniqueXform(  GFX->getProjectionMatrix() * 
		GFX->getViewMatrix() * 
		GFX->getWorldMatrix() );

	// Update position on the matrix before multiplying it
	mBBObjToWorld.setPosition(mLastPosition);

	ri->bbModelViewProj = renderManager->allocUniqueXform( *ri->modelViewProj * mBBObjToWorld );

	ri->count = n_parts;

	ri->blendStyle = blendStyle;

	// use first particle's texture unless there is an emitter texture to override it
	if (mDataBlock->textureHandle)
		ri->diffuseTex = &*(mDataBlock->textureHandle);
	else
		ri->diffuseTex = &*(part_list_head.next->dataBlock->textureHandle);

	ri->softnessDistance = softnessDistance; 

	// Sort by texture too.
	ri->defaultKey = ri->diffuseTex ? (U32)ri->diffuseTex : (U32)ri->vertBuff;

	renderManager->addInst( ri );

}

void RadiusMeshEmitter::debugRender(ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance* overrideMat)
{
	IPSDebugRenderHelper* DRH = new IPSDebugRenderHelper(emitMesh);
	if(renderSphere)
		DRH->drawSphere(center, radius);
	for(S32 faceIndex = 0; faceIndex < emitfaces.size(); faceIndex++)
	{
		IPSCore::face tris = emitfaces[faceIndex];
		DRH->drawFace(tris, renderIntersections, renderEmittingFaces, true, radius, center);
		if(tris.inboundSphere)
		{
			DRH->drawIntersections(tris);
		}
	}
}

//-----------------------------------------------------------------------------
// setSizes
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::setSizes( F32 *sizeList )
{
	for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
	{
		sizes[i] = sizeList[i];
	}
}

//-----------------------------------------------------------------------------
// setColors
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::setColors( ColorF *colorList )
{
	for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
	{
		colors[i] = colorList[i];
	}
}

//-----------------------------------------------------------------------------
// deleteWhenEmpty
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::deleteWhenEmpty()
{
	// if the following asserts fire, there is a reasonable chance that you are trying to delete a particle emitter
	// that has already been deleted (possibly by ClientMissionCleanup). If so, use a SimObjectPtr to the emitter and check it
	// for null before calling this function.
	AssertFatal(isProperlyAdded(), "RadiusMeshEmitter must be registed before calling deleteWhenEmpty");
	AssertFatal(!mDead, "RadiusMeshEmitter already deleted");
	AssertFatal(!isDeleted(), "RadiusMeshEmitter already deleted");
	AssertFatal(!isRemoved(), "RadiusMeshEmitter already removed");

	// this check is for non debug case, so that we don't write in to freed memory
	bool okToDelete = !mDead && isProperlyAdded() && !isDeleted() && !isRemoved() && isClientObject();
	if (okToDelete)
	{
		mDeleteWhenEmpty = true;
		if( !n_parts )
		{
			// We're already empty, so delete us now.
			NetConnection* conn = NetConnection::getConnectionToServer();
			if(conn)
				conn->postNetEvent(new rm_DeleteEvent(conn->getGhostIndex(this)));
			mDead = true;
			//deleteObject();
		}
		else
			AssertFatal( getSceneManager() != NULL, "RadiusMeshEmitter not on process list and won't get ticked to death" );
	}
}

//-----------------------------------------------------------------------------
// emitParticles
// Changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::emitParticles( 
	const F32 velocity,
	const U32      numMilliseconds)
{
	if( mDead ) return;

	if( mDataBlock->particleDataBlocks.empty() )
		return;

	// lifetime over - no more particles
	if( mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS )
	{
		return;
	}

	U32 currTime = 0;
	bool particlesAdded = false;

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
			getTransform().getColumn(3, &mLastPosition);
			mHasLastPosition = true;
			return;
		}
		else
		{
			currTime       += mNextParticleTime;
			mInternalClock += mNextParticleTime;
			// Emit particle at curr time
			// Also send the node on addParticle
			if(addParticle(velocity))
			{
				particlesAdded = true;
				mNextParticleTime = 0;
			}
		}
	}

	while( currTime < numMilliseconds )
	{
		S32 nextTime = ejectionPeriodMS;
		if( periodVarianceMS != 0 )
		{
			nextTime += S32(gRandGen.randI() % (2 * periodVarianceMS + 1)) -
				S32(periodVarianceMS);
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

		if(addParticle(velocity))
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
		if (overrideAdvance == false && advanceMS != 0) 
		{
			Particle* last_part = part_list_head.next;
			if (advanceMS > last_part->totalLifetime) 
			{
				part_list_head.next = last_part->next;
				n_parts--;
				last_part->next = part_freelist;
				part_freelist = last_part;
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
					last_part->pos += last_part->vel * t;

					updateKeyData( last_part );
				}
			}
		}
	}

	// DMMFIX: Lame and slow...
	if( particlesAdded == true )
		updateBBox();


	if( n_parts > 0 && getSceneManager() == NULL )
	{
		gClientSceneGraph->addObjectToScene(this);
		//ClientProcessList::get()->addObject(this);
	}

	getTransform().getColumn(3, &mLastPosition);
	mHasLastPosition = true;
}

//-----------------------------------------------------------------------------
// updateBBox - SLOW, bad news
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::updateBBox()
{
	Point3F minPt(1e10,   1e10,  1e10);
	Point3F maxPt(-1e10, -1e10, -1e10);

	for (Particle* part = part_list_head.next; part != NULL; part = part->next)
	{
		Point3F particleSize(part->size * 0.5f, 0.0f, part->size * 0.5f);
		minPt.setMin( part->pos - particleSize );
		maxPt.setMax( part->pos + particleSize );
	}

	/*ObjBox = Box3F(minPt, maxPt);
	MatrixF temp = getTransform();
	setTransform(temp);
	*/
	mBBObjToWorld.identity();
	Point3F boxScale = mObjBox.getExtents();
	boxScale.x = getMax(boxScale.x, 1.0f);
	boxScale.y = getMax(boxScale.y, 1.0f);
	boxScale.z = getMax(boxScale.z, 1.0f);
	mBBObjToWorld.scale(boxScale);
}

bool RadiusMeshEmitter::addParticle(const F32 &vel)
{
	// This should never happen
	//  - But if it happens it will slow down the server.
	if(isServerObject())
		return false;
	if(!active)
		return false;
	PROFILE_SCOPE(RadiusMeshEmitAddPart);

	F32 initialVel = ejectionVelocity;
	initialVel    += (velocityVariance * 2.0f * gRandGen.randF()) - velocityVariance;

	// Set the time since this code was last run
	U32 dt = mInternalClock - oldTime;
	oldTime = mInternalClock;

	// Check if the emitMesh matches a name
	SimObject* SB = dynamic_cast<SimObject*>(Sim::findObject(emitMesh));
	// If not then check if it matches an ID
	if(!SB)
		SB = dynamic_cast<SimObject*>(Sim::findObject(atoi(emitMesh)));
	// We define these here so we can reduce the amount of dynamic_cast 'calls'
	ShapeBase* SS;
	TSStatic* TS;
	if(SB){
		SS = dynamic_cast<ShapeBase*>(SB);
		TS = dynamic_cast<TSStatic*>(SB);
	}
	// Make sure that we are dealing with some proper objects
	if(SB && (SS || TS)){
		// Seed the random generator - this should maybe be swapped out in favor for the gRandGen.
		//  - Although they both work fine.
		srand(mainTime);
		// Throw out some trash results. Apparently the first 1-2 results fails to give proper random numbers.
		int trash = rand();
		trash = rand();
		// Set our count value to mainTime.
		U32 co = mainTime;
		// If evenEmission is on, set the co to a random number for the per vertex emission.
		if(evenEmission && vertexCount != 0)
			co = gRandGen.randI() % vertexCount;
		mainTime++;

		const TSShape* shape;
		if(SS)
			shape = SS->getShape();
		else{
			shape = TS->getShape();
		}
		bool coHandled = false;
		// -------------------------------------------------------------------------------------------------
		// -- Per triangle BEGIN ---------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------------

		TSSkinMesh* sMesh;
		TSMesh* Mesh;
		if(evenEmission)
		{
			if(emitfaces.size())
			{
				PROFILE_SCOPE(RadiusMeshEmitEven);
				// Get a random face from our emitfaces vector.
				//  - then follow basically the same procedure as above.
				//  - Just slightly simplified
				S32 faceIndex = gRandGen.randI() % emitfaces.size();
				IPSCore::face tris = emitfaces[faceIndex];
				Mesh = shape->meshes[tris.meshIndex];
				TSMesh::__TSMeshVertexBase v1, v2, v3;
				Point3F p1, p2, p3;
				if(tris.skinMesh)
				{
					sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
					U8 indiceBool = tris.triStart % 2;
					if(indiceBool = 0)
					{
						v1 = sMesh->mVertexData[sMesh->indices[tris.triStart]];
						v2 = sMesh->mVertexData[sMesh->indices[tris.triStart + 1]];
						v3 = sMesh->mVertexData[sMesh->indices[tris.triStart + 2]];
					}
					else
					{
						v1 = sMesh->mVertexData[sMesh->indices[tris.triStart]];
						v2 = sMesh->mVertexData[sMesh->indices[tris.triStart + 1]];
						v3 = sMesh->mVertexData[sMesh->indices[tris.triStart + 2]];
						/*v3 = sMesh->mVertexData[sMesh->indices[tris.triStart]];
						v2 = sMesh->mVertexData[sMesh->indices[tris.triStart + 1]];
						v1 = sMesh->mVertexData[sMesh->indices[tris.triStart + 2]];*/
					}
					p1 = v1.vert();
					p2 = v2.vert();
					p3 = v3.vert();
				}
				else{
					U8 indiceBool = tris.triStart % 2;
					if(indiceBool = 0)
					{
						v1 = Mesh->mVertexData[Mesh->indices[tris.triStart]];
						v2 = Mesh->mVertexData[Mesh->indices[tris.triStart + 1]];
						v3 = Mesh->mVertexData[Mesh->indices[tris.triStart + 2]];
					}
					else
					{
						v1 = Mesh->mVertexData[Mesh->indices[tris.triStart]];
						v2 = Mesh->mVertexData[Mesh->indices[tris.triStart + 1]];
						v3 = Mesh->mVertexData[Mesh->indices[tris.triStart + 2]];
						/*
						v3 = RadiusMesh->mVertexData[RadiusMesh->indices[tris.triStart]];
						v2 = RadiusMesh->mVertexData[RadiusMesh->indices[tris.triStart + 1]];
						v1 = RadiusMesh->mVertexData[RadiusMesh->indices[tris.triStart + 2]];*/
					}
					p1 = v1.vert();
					p2 = v2.vert();
					p3 = v3.vert();
				}
				Point3F scale;
				if(SS)
					scale = Point3F(1);
				else
					scale = TS->getScale();
				Point3F planeVec;
				Point3F pnewPosition;
				Point3F pnewrelPos;
				Point3F pnewVel;
				Point3F pneworientDir;

				if(tris.vertsInside == 3){
					IPSEmitterHelper::uniformPointInTriangle(p1,p2,p3,&planeVec);

					// Construct a vector from the 3 results
					const Point3F *vertPos = new const Point3F(planeVec);

					Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
					normalV->normalize();

					MatrixF trans;
					MatrixF nodetrans;
					MatrixF mat;
					if(SS)
					{
						trans = SS->getTransform();
					}
					else
					{
						trans = TS->getTransform();
						nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
						mat.mul(trans, nodetrans);
					}
					// Rotate our point by the rotation matrix
					Point3F* p = new Point3F();

					if(SS)
					{
						trans.mulV(*vertPos,p);
						pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
					}
					else{
						mat.mulV((*vertPos * TS->getScale()),p);
						//mat.mulV(*vertPos,p);
						mat.mulV(*normalV);
						pnewPosition = TS->getPosition() + *p + (*normalV * ejectionOffset);
					}
					pnewrelPos = *p +(*normalV * ejectionOffset);
					pnewVel = *normalV * initialVel;
					pneworientDir = *normalV;
					delete(*p);
					delete(*vertPos);
					delete(*normalV);
				}
				if(tris.vertsInside == 2)
				{
					IPSEmitterHelper::uniformPointInTrapezoid(tris, p1, p2, p3, &planeVec);


					// Construct a vector from the 3 results
					const Point3F *vertPos = new const Point3F(planeVec);

					Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
					normalV->normalize();

					MatrixF trans;
					MatrixF nodetrans;
					MatrixF mat;
					if(SS)
					{
						trans = SS->getTransform();
					}
					else
					{
						trans = TS->getTransform();
						nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
						mat.mul(trans, nodetrans);
					}
					// Rotate our point by the rotation matrix
					Point3F* p = new Point3F();

					if(SS)
					{
						trans.mulV(*vertPos,p);
						pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
					}
					else{
						mat.mulV((*vertPos * TS->getScale()),p);
						//mat.mulV(*vertPos,p);
						mat.mulV(*normalV);
						pnewPosition = TS->getPosition() + *p + (*normalV * ejectionOffset);
					}
					pnewrelPos = *p +(*normalV * ejectionOffset);
					pnewVel = *normalV * initialVel;
					pneworientDir = *normalV;
					delete(*p);
					delete(*vertPos);
					delete(*normalV);
				}
				if(tris.vertsInside == 1)
				{
					MatrixF trans;
					MatrixF nodetrans;
					MatrixF mat;
					if(SS)
					{
						trans = SS->getTransform();
						trans.mulV(p1);
						trans.mulV(p2);
						trans.mulV(p3);
					}
					else
					{
						trans = TS->getTransform();
						nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
						mat.mul(trans, nodetrans);
						mat.mulV(p1);
						mat.mulV(p2);
						mat.mulV(p3);

						p1 *= TS->getScale();
						p2 *= TS->getScale();
						p3 *= TS->getScale();
					}

					F32 coeff = gRandGen.randF();
					if(coeff < tris.arcArea1Coeff){
						PlaneF plane = PlaneF(p1,p2,p3);
						Point3F PlaneP = plane.project(center);
						Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
						Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
						Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
						Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
						Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
						Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));

						Point3F chordStart, chordEnd, mirrorP;
						if(tris.v1Inside && !tris.v2Inside && !tris.v3Inside)
						{
							chordStart = sideA1;
							chordEnd = sideC1;
							mirrorP = p1;
						}
						if(!tris.v1Inside && tris.v2Inside && !tris.v3Inside)
						{
							chordStart = sideB1;
							chordEnd = sideA1;
							mirrorP = p2;
						}
						if(!tris.v1Inside && !tris.v2Inside && tris.v3Inside)
						{
							chordStart = sideC1;
							chordEnd = sideB1;
							mirrorP = p3;
						}
						if(!IPSEmitterHelper::uniformPointInArc(plane, chordStart, chordEnd, center, radius, mirrorP, &planeVec))
							return false;
					}
					else
						IPSEmitterHelper::uniformPointInTriangle(tris, p1, p2, p3, &planeVec);

					// Construct a vector from the 3 results
					const Point3F *vertPos = new const Point3F(planeVec);

					Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
					normalV->normalize();

					// Rotate our point by the rotation matrix

					if(SS)
					{
						pnewPosition = SS->getPosition() + *vertPos + (*normalV * ejectionOffset);
					}
					else{
						mat.mulV(*normalV);
						pnewPosition = TS->getPosition() + *vertPos + (*normalV * ejectionOffset);
					}
					pnewrelPos = *vertPos +(*normalV * ejectionOffset);
					PlaneF plane = PlaneF(p1,p2,p3);
					pnewVel = *normalV * initialVel;
					pneworientDir = *normalV;
					pneworientDir = IPSCore::perpendicularVector(*normalV,plane);
					delete(*vertPos);
					delete(*normalV);
				}
				if(tris.vertsInside == 0)
				{
					Point3F pos;
					Point3F planeVec;
					if(SS){
						MatrixF trans;
						trans = SS->getTransform();
						trans.mulP(v1.vert(),&p1);
						trans.mulP(v2.vert(),&p2);
						trans.mulP(v3.vert(),&p3);
						pos = SS->getPosition();

						Point3F worldProjCenter;
						//trans.mulP(tris.sphereProjCenter,&worldProjCenter);
					}
					if(TS){
						pos = TS->getPosition();
						MatrixF trans, nodetrans, mat;
						trans = TS->getTransform();
						nodetrans = TS->getShapeInstance()->mNodeTransforms[0];

						mat.mul(trans,nodetrans);
						mat.mulV(v1.vert(),&p1);
						mat.mulV(v2.vert(),&p2);
						mat.mulV(v3.vert(),&p3);
						p1 *= TS->getScale();
						p2 *= TS->getScale();
						p3 *= TS->getScale();
					}

					//Implement inbound circle here
					if(tris.inboundSphere)
					{
						if(IPSEmitterHelper::uniformPointInInboundSphere(tris, p1, p2, p3, center, pos, radius, &planeVec))
						{
							pnewPosition = pos+planeVec;
							pnewrelPos = planeVec;
							pnewVel.set(0);
							//pneworientDir = plane;
							// Construct a vector from the 3 results
							const Point3F *vertPos = new const Point3F(planeVec);

							Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
							normalV->normalize();

							MatrixF trans;
							MatrixF nodetrans;
							MatrixF mat;
							if(SS)
							{
								trans = SS->getTransform();
							}
							else
							{
								trans = TS->getTransform();
								nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
								mat.mul(trans, nodetrans);
							}
							// Rotate our point by the rotation matrix
							Point3F* p = new Point3F();

							if(SS)
							{
								trans.mulV(*vertPos,p);
								pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
							}
							else{
								//mat.mulV((*vertPos * TS->getScale()),p);
								//mat.mulV(*vertPos,p);
								mat.mulV(*normalV);
								pnewPosition = TS->getPosition() + *vertPos + (*normalV * ejectionOffset);
							}
							pnewrelPos = *p +(*normalV * ejectionOffset);
							pnewVel = *normalV * initialVel;
							pneworientDir = *normalV;
							delete(*p);
							delete(*vertPos);
							delete(*normalV);
						}
					}

					if(((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count != 2) || 
						(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
						(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)) &&
						IPSEmitterHelper::uniformPointInArc(tris,p1,p2,p3,center,radius,&planeVec) && !tris.inboundSphere)
					{
						// Construct a vector from the 3 results
						const Point3F *vertPos = new const Point3F(planeVec);

						Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
						normalV->normalize();

						MatrixF trans;
						MatrixF nodetrans;
						MatrixF mat;
						if(SS)
						{
							trans = SS->getTransform();
						}
						else
						{
							trans = TS->getTransform();
							nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
							mat.mul(trans, nodetrans);
						}
						// Rotate our point by the rotation matrix
						Point3F* p = new Point3F();

						if(SS)
						{
							trans.mulV(*vertPos,p);
							pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
						}
						else{
							//mat.mulV((*vertPos * TS->getScale()),p);
							//mat.mulV(*vertPos,p);
							mat.mulV(*normalV);
							pnewPosition = TS->getPosition() + *vertPos + (*normalV * ejectionOffset);
						}
						pnewrelPos = *p +(*normalV * ejectionOffset);
						pnewVel = *normalV * initialVel;
						pneworientDir = *normalV;
						delete(*p);
						delete(*vertPos);
						delete(*normalV);
					}
					if(((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
						(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2) || 
						(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)) && !tris.inboundSphere)
					{
						F32 coeff = gRandGen.randF();
						if(coeff < tris.arcArea1Coeff)
							IPSEmitterHelper::uniformPointInArc(tris,p1,p2,p3,center,radius,false,&planeVec);
						else if(coeff < tris.arcArea1Coeff + tris.arcArea2Coeff)
							IPSEmitterHelper::uniformPointInArc(tris,p1,p2,p3,center,radius,true,&planeVec);
						else
							IPSEmitterHelper::uniformPointInTrapezoid(tris, p1, p2, p3, &planeVec);

						// Construct a vector from the 3 results
						const Point3F *vertPos = new const Point3F(planeVec);

						Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
						normalV->normalize();

						MatrixF trans;
						MatrixF nodetrans;
						MatrixF mat;
						if(SS)
						{
							trans = SS->getTransform();
						}
						else
						{
							trans = TS->getTransform();
							nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
							mat.mul(trans, nodetrans);
						}
						// Rotate our point by the rotation matrix
						Point3F* p = new Point3F();

						if(SS)
						{
							trans.mulV(*vertPos,p);
							pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
						}
						else{
							//mat.mulV((*vertPos * TS->getScale()),p);
							//mat.mulV(*vertPos,p);
							mat.mulV(*normalV);
							pnewPosition = TS->getPosition() + *vertPos + (*normalV * ejectionOffset);
						}
						pnewrelPos = *p +(*normalV * ejectionOffset);
						pnewVel = *normalV * initialVel;
						pneworientDir = *normalV;
						delete(*p);
						delete(*vertPos);
						delete(*normalV);
					}
				}

				n_parts++;
				if (n_parts > n_part_capacity || n_parts > mDataBlock->partListInitSize)
				{
					// In an emergency we allocate additional particles in blocks of 16.
					// This should happen rarely.
					Particle* store_block = new Particle[16];
					part_store.push_back(store_block);
					n_part_capacity += 16;
					for (S32 i = 0; i < 16; i++)
					{
						store_block[i].next = part_freelist;
						part_freelist = &store_block[i];
					}
					mDataBlock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
				}

				Particle* pNew = part_freelist;
				part_freelist = pNew->next;
				pNew->next = part_list_head.next;
				part_list_head.next = pNew;

				pNew->pos = pnewPosition;
				pNew->relPos = pnewrelPos;
				pNew->vel = pnewVel;
				pNew->orientDir = pneworientDir;
				pNew->acc.set(0, 0, 0);
				pNew->currentAge = 0;

				// Choose a new particle datablack randomly from the list
				U32 dBlockIndex = gRandGen.randI() % mDataBlock->particleDataBlocks.size();
				mDataBlock->particleDataBlocks[dBlockIndex]->initializeParticle(pNew, pNew->vel);
				updateKeyData( pNew );
			}
		}
#pragma endregion
		// -------------------------------------------------------------------------------------------------
		// -- Per triangle END -----------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------------
		if(evenEmission && mainTime == U32_MAX)
			mainTime = 0;
		return true;
		/**/
	}
	return false;
}

//-----------------------------------------------------------------------------
// processTick
// Changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::processTick(const Move* move)
{
	Parent::processTick(move);
	if(mDead)
		return;
	if( mDeleteOnTick == true && isClientObject() )
	{
		mDead = true;
		NetConnection* conn = NetConnection::getConnectionToServer();
		if(conn)
			conn->postNetEvent(new rm_DeleteEvent(conn->getGhostIndex(this)));
		//deleteObject();
		return;
	}

	// If the object has been culled out we don't want to render the particles. 
	// If it haven't, set the bounds to global so the particles will always be rendered regardless of whether the object is seen or not.
	if(isObjectCulled())
	{
		mGlobalBounds = false;
		setRenderEnabled(false);

		if( mSceneManager )
			mSceneManager->notifyObjectDirty( this );
	}
	else
	{
		setGlobalBounds();
		setRenderEnabled(true);
	}
}

//-----------------------------------------------------------------------------
// isObjectCulled
// Checks if the RadiusMesh which is being emitted on is getting culled out
// If the object is culled out, so is the emitter.
// This code is from guiShapeNameHud
// Needs to get better, should simply do a isRendered test but
// Didn't find any method for that.
// Custom
//-----------------------------------------------------------------------------
bool RadiusMeshEmitter::isObjectCulled()
{
	PROFILE_SCOPE(rm_isObjectCulled);
	// Must have a connection and control object
	GameConnection* conn = GameConnection::getConnectionToServer();
	if (!conn) return true;
	GameBase * control = dynamic_cast<GameBase*>(conn->getControlObject());
	if (!control) return true;

	// Get control camera info
	MatrixF cam;
	Point3F camPos;
	VectorF camDir;
	//cam = conn->getCameraObject()->getTransform();
	conn->getControlCameraTransform(0,&cam);
	cam.getColumn(3, &camPos);
	cam.getColumn(1, &camDir);

	F32 camFov;
	conn->getControlCameraFov(&camFov);
	camFov = mDegToRad(camFov) / 2;

	// Visible distance info
	F32 visDistance = gClientSceneGraph->getVisibleDistance();
	F32 visDistanceSqr = visDistance * visDistance;

	// Collision info. We're going to be running LOS tests and we
	// don't want to collide with the control object.
	static U32 losMask = TerrainObjectType | InteriorObjectType | ShapeBaseObjectType;

	SimObject* SB = dynamic_cast<SimObject*>(Sim::findObject(emitMesh));
	if(!SB)
		SB = dynamic_cast<SimObject*>(Sim::findObject(atoi(emitMesh)));

	// All ghosted objects are added to the server connection group,
	// so we can find all the shape base objects by iterating through
	// our current connection.
	ShapeBase* shape = dynamic_cast< ShapeBase* >(SB);
	//GameBase* GB = dynamic_cast< GameBase* >(SB);
	TSStatic* TSshape = dynamic_cast<TSStatic*>(SB);
	if ( shape ) {
		// Target pos to test, if it's a player run the LOS to his eye
		// point, otherwise we'll grab the generic box center.
		Point3F shapePos;
		if (shape->getTypeMask() & PlayerObjectType) 
		{
			MatrixF eye;

			// Use the render eye transform, otherwise we'll see jittering
			shape->getRenderEyeTransform(&eye);
			eye.getColumn(3, &shapePos);
		} 
		else 
		{
			// Use the render transform instead of the box center
			// otherwise it'll jitter.
			MatrixF srtMat = shape->getRenderTransform();
			srtMat.getColumn(3, &shapePos);
		}
		VectorF shapeDir = shapePos - camPos;

		// Test to see if it's in range
		F32 shapeDist = shapeDir.lenSquared();
		if (shapeDist == 0 || shapeDist > visDistanceSqr)
			return true;
		shapeDist = mSqrt(shapeDist);

		// Test to see if it's within our viewcone, this test doesn't
		// actually match the viewport very well, should consider
		// projection and box test.
		shapeDir.normalize();
		F32 dot = mDot(shapeDir, camDir);
		if (dot < camFov)
			return true;

		// Don't raytest, if the object is slightly below the terrain (houses etc)
		//  - then a ray test will exclude it, ruining the effect.

		// Test to see if it's behind something, and we want to
		// ignore anything it's mounted on when we run the LOS.
		/*RayInfo info;
		shape->disableCollision();
		control->disableCollision();
		SceneObject *mount = shape->getObjectMount();
		if (mount)
		mount->disableCollision();
		bool los = !gClientContainer.castRay(camPos, shapePos,losMask, &info);
		shape->enableCollision();
		control->enableCollision();
		if (mount)
		mount->enableCollision();
		if (!los)
		return true;*/

		// The object is not culled
		return false;
	}

	else if( TSshape )
	{
		Point3F shapePos;
		// Use the render transform instead of the box center
		// otherwise it'll jitter.
		MatrixF srtMat = TSshape->getRenderTransform();
		srtMat.getColumn(3, &shapePos);

		VectorF shapeDir = shapePos - camPos;

		// Test to see if it's in range
		F32 shapeDist = shapeDir.lenSquared();
		if (shapeDist == 0 || shapeDist > visDistanceSqr)
			return true;
		shapeDist = mSqrt(shapeDist);

		// Test to see if it's within our viewcone, this test doesn't
		// actually match the viewport very well, should consider
		// projection and box test.
		shapeDir.normalize();
		F32 dot = mDot(shapeDir, camDir);
		if (dot < camFov)
			return true;

		// Don't raytest, if the object is slightly below the terrain (houses etc)
		//  - then a ray test will exclude it, ruining the effect.

		// Test to see if it's behind something, and we want to
		// ignore anything it's mounted on when we run the LOS.
		/*RayInfo info;
		TSshape->disableCollision();
		control->disableCollision();
		SceneObject *mount = TSshape->getObjectMount();
		if (mount)
		mount->disableCollision();
		bool los = !gClientContainer.castRay(camPos, shapePos,losMask, &info);
		TSshape->enableCollision();
		control->enableCollision();
		if (mount)
		mount->enableCollision();
		if (!los)
		return true;*/

		// The object is not culled
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// advanceTime
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::advanceTime(F32 dt)
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
	Particle* last_part = &part_list_head;
	for (Particle* part = part_list_head.next; part != NULL; part = part->next)
	{
		part->currentAge += numMSToUpdate;
		if (part->currentAge > part->totalLifetime)
		{
			n_parts--;
			last_part->next = part->next;
			part->next = part_freelist;
			part_freelist = part;
			part = last_part;
		}
		else
		{
			last_part = part;
		}
	}

	AssertFatal( n_parts >= 0, "RadiusMeshEmitter: negative part count!" );

	if (n_parts < 1 && mDeleteWhenEmpty)
	{
		mDeleteOnTick = true;
		return;
	}

	if( numMSToUpdate != 0 && n_parts > 0 )
	{
		update( numMSToUpdate );
	}
	if(active)
		emitParticles(ejectionVelocity, (U32)(dt * 1000.0f));
}

//-----------------------------------------------------------------------------
// Update key related particle data
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::updateKeyData( Particle *part )
{
	//Ensure that our lifetime is never below 0
	if( part->totalLifetime < 1 )
		part->totalLifetime = 1;

	F32 t = F32(part->currentAge) / F32(part->totalLifetime);
	AssertFatal(t <= 1.0f, "Out out bounds filter function for particle.");

	for( U32 i = 1; i < ParticleData::PDC_NUM_KEYS; i++ )
	{
		if( part->dataBlock->times[i] >= t )
		{
			F32 firstPart = t - part->dataBlock->times[i-1];
			F32 total     = part->dataBlock->times[i] -
				part->dataBlock->times[i-1];

			firstPart /= total;

			if( useEmitterColors )
			{
				part->color.interpolate(colors[i-1], colors[i], firstPart);
			}
			else
			{
				part->color.interpolate(part->dataBlock->colors[i-1],
					part->dataBlock->colors[i],
					firstPart);
			}

			if( useEmitterSizes )
			{
				part->size = (sizes[i-1] * (1.0 - firstPart)) +
					(sizes[i]   * firstPart);
			}
			else
			{
				part->size = (part->dataBlock->sizes[i-1] * (1.0 - firstPart)) +
					(part->dataBlock->sizes[i]   * firstPart);
			}
			break;

		}
	}
}

//-----------------------------------------------------------------------------
// Update particles
// Changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::update( U32 ms )
{
	// TODO: Prefetch

	// Foreach particle
	for (Particle* part = part_list_head.next; part != NULL; part = part->next)
	{
		F32 t = F32(ms) / 1000.0;

		for(int i = 0; i < attrobjectCount; i++)
		{
			PROFILE_SCOPE(RadiusMeshEmitAttracted);
			part->acc.zero();

			// If particle is attracted
			if(AttractionMode[i] > 0)
			{
				// First check by name then by ID
				GameBase* GB = dynamic_cast<GameBase*>(Sim::findObject(attractedObjectID[i]));
				if(!GB)
					GB = dynamic_cast<GameBase*>(Sim::findObject(atoi(attractedObjectID[i])));
				if(GB){
					// GameBase object found!
					Point3F target = GB->getPosition();
					char attrBuf[255];
					strcpy(attrBuf,Attraction_offset[i]);

					MatrixF trans = GB->getTransform();

					// Parse the offset from a "x y z" string format.
					char* xBuf = strtok(attrBuf, " ");
					F32 x = atof(xBuf);
					char* yBuf = strtok(NULL, " ");
					F32 y = atof(yBuf);
					char* zBuf = strtok(NULL, " ");
					F32 z = atof(zBuf);
					// Rotate the offset
					Point3F* po = new Point3F(x,y,z);
					trans.mulV(*po);
					target += *po;

					// Read more about the attraction algorithm in the docs or on the T3D - CE wiki
					Point3F diff = (target - part->pos);
					Point3F* attR = new Point3F(attractionrange);
					if(diff.len() < 1)
						diff.normalize();
					Point3F ndiff = diff;
					ndiff.normalize();
					F32 fdiff = attractionrange/(diff.len())-1;
					if(fdiff < 0)
						fdiff = 0;
					// Attract
					if(AttractionMode[i] == 1)
						part->acc += (ndiff * fdiff)*Amount[i];
					// Repulse
					if(AttractionMode[i] == 2)
						part->acc -= (ndiff * fdiff)*Amount[i];

					delete(*attR);
					delete(*po);
				}
			}
		}

		Point3F a = part->acc;
		a -= part->vel * part->dataBlock->dragCoefficient;
		a -= mWindVelocity * part->dataBlock->windCoefficient;
		a += Point3F(0.0f, 0.0f, -9.81f) * part->dataBlock->gravityCoefficient;
		part->vel += a * t;

		// added part ----------------
		// Not sure if collision will ever have any use on a RadiusMesh particle emitter.
		/*RayInfo rInfo;
		if(gClientContainer.castRay(part->pos, part->pos + part->vel * t, TerrainObjectType | InteriorObjectType | VehicleObjectType | PlayerObjectType, &rInfo))
		{
		Point3F proj = mDot(part->vel,rInfo.normal)/(rInfo.normal.len()*rInfo.normal.len())*rInfo.normal;
		Point3F between = (part->vel - proj);
		part->vel = -(part->vel-(between*2)*0.8);
		}*/
		// end addition ---------------

		part->pos += part->vel * t;
		if(sticky)
			part->pos = parentNodePos + part->relPos;

		updateKeyData( part );
	}
}

//-----------------------------------------------------------------------------
// Copy particles to vertex buffer
// Not changed
//-----------------------------------------------------------------------------

// structure used for particle sorting.
struct SortParticle
{
	Particle* p;
	F32       k;
};

// qsort callback function for particle sorting
int QSORT_CALLBACK rm_cmpSortParticles(const void* p1, const void* p2)
{
	const SortParticle* sp1 = (const SortParticle*)p1;
	const SortParticle* sp2 = (const SortParticle*)p2;

	if (sp2->k > sp1->k)
		return 1;
	else if (sp2->k == sp1->k)
		return 0;
	else
		return -1;
}

void RadiusMeshEmitter::copyToVB( const Point3F &camPos, const ColorF &ambientColor )
{
	static Vector<SortParticle> orderedVector(__FILE__, __LINE__);

	PROFILE_START(RadiusMeshEmitter_copyToVB);

	PROFILE_START(RadiusMeshEmitter_copyToVB_Sort);
	// build sorted list of particles (far to near)
	if (sortParticles)
	{
		orderedVector.clear();

		MatrixF modelview = GFX->getWorldMatrix();
		Point3F viewvec; modelview.getRow(1, &viewvec);

		// add each particle and a distance based sort key to orderedVector
		for (Particle* pp = part_list_head.next; pp != NULL; pp = pp->next)
		{
			orderedVector.increment();
			orderedVector.last().p = pp;
			orderedVector.last().k = mDot(pp->pos, viewvec);
		}

		// qsort the list into far to near ordering
		dQsort(orderedVector.address(), orderedVector.size(), sizeof(SortParticle), rm_cmpSortParticles);
	}
	PROFILE_END();

#if defined(TORQUE_OS_XENON)
	// Allocate writecombined since we don't read back from this buffer (yay!)
	if(mVertBuff.isNull())
		mVertBuff = new GFX360MemVertexBuffer(GFX, 1, getGFXVertexFormat<ParticleVertexType>(), sizeof(ParticleVertexType), GFXBufferTypeDynamic, PAGE_WRITECOMBINE);
	if( n_parts > mCurBuffSize )
	{
		mCurBuffSize = n_parts;
		mVertBuff.resize(n_parts * 4);
	}

	ParticleVertexType *buffPtr = mVertBuff.lock();
#else
	static Vector<ParticleVertexType> tempBuff(2048);
	tempBuff.reserve( n_parts*4 + 64); // make sure tempBuff is big enough
	ParticleVertexType *buffPtr = tempBuff.address(); // use direct pointer (faster)
#endif

	if (orientParticles)
	{
		PROFILE_START(RadiusMeshEmitter_copyToVB_Orient);

		if (reverseOrder)
		{
			buffPtr += 4*(n_parts-1);
			// do sorted-oriented particles
			if (sortParticles)
			{
				SortParticle* partPtr = orderedVector.address();
				for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr-=4 )
					setupOriented(partPtr->p, camPos, ambientColor, buffPtr);
			}
			// do unsorted-oriented particles
			else
			{
				for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr-=4)
					setupOriented(partPtr, camPos, ambientColor, buffPtr);
			}
		}
		else
		{
			// do sorted-oriented particles
			if (sortParticles)
			{
				SortParticle* partPtr = orderedVector.address();
				for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr+=4 )
					setupOriented(partPtr->p, camPos, ambientColor, buffPtr);
			}
			// do unsorted-oriented particles
			else
			{
				for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr+=4)
					setupOriented(partPtr, camPos, ambientColor, buffPtr);
			}
		}
		PROFILE_END();
	}
	else if (alignParticles)
	{
		PROFILE_START(RadiusMeshEmitter_copyToVB_Aligned);

		if (reverseOrder)
		{
			buffPtr += 4*(n_parts-1);

			// do sorted-oriented particles
			if (sortParticles)
			{
				SortParticle* partPtr = orderedVector.address();
				for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr-=4 )
					setupAligned(partPtr->p, ambientColor, buffPtr);
			}
			// do unsorted-oriented particles
			else
			{
				Particle *partPtr = part_list_head.next;
				for (; partPtr != NULL; partPtr = partPtr->next, buffPtr-=4)
					setupAligned(partPtr, ambientColor, buffPtr);
			}
		}
		else
		{
			// do sorted-oriented particles
			if (sortParticles)
			{
				SortParticle* partPtr = orderedVector.address();
				for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr+=4 )
					setupAligned(partPtr->p, ambientColor, buffPtr);
			}
			// do unsorted-oriented particles
			else
			{
				Particle *partPtr = part_list_head.next;
				for (; partPtr != NULL; partPtr = partPtr->next, buffPtr+=4)
					setupAligned(partPtr, ambientColor, buffPtr);
			}
		}
		PROFILE_END();
	}
	else
	{
		PROFILE_START(RadiusMeshEmitter_copyToVB_NonOriented);
		// somewhat odd ordering so that texture coordinates match the oriented
		// particles
		Point3F basePoints[4];
		basePoints[0] = Point3F(-1.0, 0.0,  1.0);
		basePoints[1] = Point3F(-1.0, 0.0, -1.0);
		basePoints[2] = Point3F( 1.0, 0.0, -1.0);
		basePoints[3] = Point3F( 1.0, 0.0,  1.0);

		MatrixF camView = GFX->getWorldMatrix();
		camView.transpose();  // inverse - this gets the particles facing camera

		if (reverseOrder)
		{
			buffPtr += 4*(n_parts-1);
			// do sorted-billboard particles
			if (sortParticles)
			{
				SortParticle *partPtr = orderedVector.address();
				for( U32 i=0; i<n_parts; i++, partPtr++, buffPtr-=4 )
					setupBillboard( partPtr->p, basePoints, camView, ambientColor, buffPtr );
			}
			// do unsorted-billboard particles
			else
			{
				for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr-=4)
					setupBillboard( partPtr, basePoints, camView, ambientColor, buffPtr );
			}
		}
		else
		{
			// do sorted-billboard particles
			if (sortParticles)
			{
				SortParticle *partPtr = orderedVector.address();
				for( U32 i=0; i<n_parts; i++, partPtr++, buffPtr+=4 )
					setupBillboard( partPtr->p, basePoints, camView, ambientColor, buffPtr );
			}
			// do unsorted-billboard particles
			else
			{
				for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr+=4)
					setupBillboard( partPtr, basePoints, camView, ambientColor, buffPtr );
			}
		}

		PROFILE_END();
	}

#if defined(TORQUE_OS_XENON)
	mVertBuff.unlock();
#else
	PROFILE_START(RadiusMeshEmitter_copyToVB_LockCopy);
	// create new VB if emitter size grows
	if( !mVertBuff || n_parts > mCurBuffSize )
	{
		mCurBuffSize = n_parts;
		mVertBuff.set( GFX, n_parts * 4, GFXBufferTypeDynamic );
	}
	// lock and copy tempBuff to video RAM
	ParticleVertexType *verts = mVertBuff.lock();
	dMemcpy( verts, tempBuff.address(), n_parts * 4 * sizeof(ParticleVertexType) );
	mVertBuff.unlock();
	PROFILE_END();
#endif

	PROFILE_END();
}

//-----------------------------------------------------------------------------
// Set up particle for billboard style render
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::setupBillboard( Particle *part,
	Point3F *basePts,
	const MatrixF &camView,
	const ColorF &ambientColor,
	ParticleVertexType *lVerts )
{
	F32 width     = part->size * 0.5f;
	F32 spinAngle = part->spinSpeed * part->currentAge * AgedSpinToRadians;

	F32 sy, cy;
	mSinCos(spinAngle, sy, cy);

	const F32 ambientLerp = mClampF( ambientFactor, 0.0f, 1.0f );
	ColorF partCol = mLerp( part->color, ( part->color * ambientColor ), ambientLerp );

	// fill four verts, use macro and unroll loop
#define fillVert(){ \
	lVerts->point.x = cy * basePts->x - sy * basePts->z;  \
	lVerts->point.y = 0.0f;                                \
	lVerts->point.z = sy * basePts->x + cy * basePts->z;  \
	camView.mulV( lVerts->point );                        \
	lVerts->point *= width;                               \
	lVerts->point += part->pos;                           \
	lVerts->color = partCol; } \

	// Here we deal with UVs for animated particle (billboard)
	if (part->dataBlock->animateTexture)
	{ 
		S32 fm = (S32)(part->currentAge*(1.0/1000.0)*part->dataBlock->framesPerSec);
		U8 fm_tile = part->dataBlock->animTexFrames[fm % part->dataBlock->numFrames];
		S32 uv[4];
		uv[0] = fm_tile + fm_tile/part->dataBlock->animTexTiling.x;
		uv[1] = uv[0] + (part->dataBlock->animTexTiling.x + 1);
		uv[2] = uv[1] + 1;
		uv[3] = uv[0] + 1;

		fillVert();
		// Here and below, we copy UVs from particle datablock's current frame's UVs (billboard)
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[0]];
		++lVerts;
		++basePts;

		fillVert();
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[1]];
		++lVerts;
		++basePts;

		fillVert();
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[2]];
		++lVerts;
		++basePts;

		fillVert();
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[3]];
		++lVerts;
		++basePts;

		return;
	}

	fillVert();
	// Here and below, we copy UVs from particle datablock's texCoords (billboard)
	lVerts->texCoord = part->dataBlock->texCoords[0];
	++lVerts;
	++basePts;

	fillVert();
	lVerts->texCoord = part->dataBlock->texCoords[1];
	++lVerts;
	++basePts;

	fillVert();
	lVerts->texCoord = part->dataBlock->texCoords[2];
	++lVerts;
	++basePts;

	fillVert();
	lVerts->texCoord = part->dataBlock->texCoords[3];
	++lVerts;
	++basePts;
}

//-----------------------------------------------------------------------------
// Set up oriented particle
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::setupOriented( Particle *part,
	const Point3F &camPos,
	const ColorF &ambientColor,
	ParticleVertexType *lVerts )
{
	Point3F dir;

	if( orientOnVelocity )
	{
		// don't render oriented particle if it has no velocity
		if( part->vel.magnitudeSafe() == 0.0 ) return;
		dir = part->vel;
	}
	else
	{
		dir = part->orientDir;
	}

	Point3F dirFromCam = part->pos - camPos;
	Point3F crossDir;
	mCross( dirFromCam, dir, &crossDir );
	crossDir.normalize();
	dir.normalize();

	F32 width = part->size * 0.5f;
	dir *= width;
	crossDir *= width;
	Point3F start = part->pos - dir;
	Point3F end = part->pos + dir;

	const F32 ambientLerp = mClampF( ambientFactor, 0.0f, 1.0f );
	ColorF partCol = mLerp( part->color, ( part->color * ambientColor ), ambientLerp );

	// Here we deal with UVs for animated particle (oriented)
	if (part->dataBlock->animateTexture)
	{ 
		// Let particle compute the UV indices for current frame
		S32 fm = (S32)(part->currentAge*(1.0f/1000.0f)*part->dataBlock->framesPerSec);
		U8 fm_tile = part->dataBlock->animTexFrames[fm % part->dataBlock->numFrames];
		S32 uv[4];
		uv[0] = fm_tile + fm_tile/part->dataBlock->animTexTiling.x;
		uv[1] = uv[0] + (part->dataBlock->animTexTiling.x + 1);
		uv[2] = uv[1] + 1;
		uv[3] = uv[0] + 1;

		lVerts->point = start + crossDir;
		lVerts->color = partCol;
		// Here and below, we copy UVs from particle datablock's current frame's UVs (oriented)
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[0]];
		++lVerts;

		lVerts->point = start - crossDir;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[1]];
		++lVerts;

		lVerts->point = end - crossDir;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[2]];
		++lVerts;

		lVerts->point = end + crossDir;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[3]];
		++lVerts;

		return;
	}

	lVerts->point = start + crossDir;
	lVerts->color = partCol;
	// Here and below, we copy UVs from particle datablock's texCoords (oriented)
	lVerts->texCoord = part->dataBlock->texCoords[0];
	++lVerts;

	lVerts->point = start - crossDir;
	lVerts->color = partCol;
	lVerts->texCoord = part->dataBlock->texCoords[1];
	++lVerts;

	lVerts->point = end - crossDir;
	lVerts->color = partCol;
	lVerts->texCoord = part->dataBlock->texCoords[2];
	++lVerts;

	lVerts->point = end + crossDir;
	lVerts->color = partCol;
	lVerts->texCoord = part->dataBlock->texCoords[3];
	++lVerts;
}

void RadiusMeshEmitter::setupAligned( const Particle *part, 
	const ColorF &ambientColor,
	ParticleVertexType *lVerts )
{
	// The aligned direction will always be normalized.
	Point3F dir = alignDirection;

	// Find a right vector for this particle.
	Point3F right;
	if (mFabs(dir.y) > mFabs(dir.z))
		mCross(Point3F::UnitZ, dir, &right);
	else
		mCross(Point3F::UnitY, dir, &right);
	right.normalize();

	// If we have a spin velocity.
	if ( !mIsZero( part->spinSpeed ) )
	{
		F32 spinAngle = part->spinSpeed * part->currentAge * AgedSpinToRadians;

		// This is an inline quaternion vector rotation which
		// is faster that QuatF.mulP(), but generates different
		// results and hence cannot replace it right now.

		F32 sin, qw;
		mSinCos( spinAngle * 0.5f, sin, qw );
		F32 qx = dir.x * sin;
		F32 qy = dir.y * sin;
		F32 qz = dir.z * sin;

		F32 vx = ( right.x * qw ) + ( right.z * qy ) - ( right.y * qz );
		F32 vy = ( right.y * qw ) + ( right.x * qz ) - ( right.z * qx );
		F32 vz = ( right.z * qw ) + ( right.y * qx ) - ( right.x * qy );
		F32 vw = ( right.x * qx ) + ( right.y * qy ) + ( right.z * qz );

		right.x = ( qw * vx ) + ( qx * vw ) + ( qy * vz ) - ( qz * vy );
		right.y = ( qw * vy ) + ( qy * vw ) + ( qz * vx ) - ( qx * vz );
		right.z = ( qw * vz ) + ( qz * vw ) + ( qx * vy ) - ( qy * vx );
	}

	// Get the cross vector.
	Point3F cross;
	mCross(right, dir, &cross);

	F32 width = part->size * 0.5f;
	right *= width;
	cross *= width;
	Point3F start = part->pos - right;
	Point3F end = part->pos + right;

	const F32 ambientLerp = mClampF( ambientFactor, 0.0f, 1.0f );
	ColorF partCol = mLerp( part->color, ( part->color * ambientColor ), ambientLerp );

	// Here we deal with UVs for animated particle
	if (part->dataBlock->animateTexture)
	{ 
		// Let particle compute the UV indices for current frame
		S32 fm = (S32)(part->currentAge*(1.0f/1000.0f)*part->dataBlock->framesPerSec);
		U8 fm_tile = part->dataBlock->animTexFrames[fm % part->dataBlock->numFrames];
		S32 uv[4];
		uv[0] = fm_tile + fm_tile/part->dataBlock->animTexTiling.x;
		uv[1] = uv[0] + (part->dataBlock->animTexTiling.x + 1);
		uv[2] = uv[1] + 1;
		uv[3] = uv[0] + 1;

		lVerts->point = start + cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[0]];
		++lVerts;

		lVerts->point = start - cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[1]];
		++lVerts;

		lVerts->point = end - cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[2]];
		++lVerts;

		lVerts->point = end + cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[3]];
		++lVerts;
	}
	else
	{
		// Here and below, we copy UVs from particle datablock's texCoords
		lVerts->point = start + cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->texCoords[0];
		++lVerts;

		lVerts->point = start - cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->texCoords[1];
		++lVerts;

		lVerts->point = end - cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->texCoords[2];
		++lVerts;

		lVerts->point = end + cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->texCoords[3];
		++lVerts;
	}
}

//-----------------------------------------------------------------------------
// reload
// Not changed
//-----------------------------------------------------------------------------
bool RadiusMeshEmitterData::reload()
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
		Con::errorf( "RadiusMeshEmitterData(%s) has an empty particles string.", getName() );
		mReloadSignal.trigger();
		return false;
	}

	for( U32 i = 0; i < numUnits; ++ i )
	{
		const char* dbName = StringUnit::getUnit( particleString, i, " \t" );

		ParticleData* data = NULL;
		if( !Sim::findObject( dbName, data ) )
		{
			Con::errorf( ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) unable to find particle datablock: %s", getName(), dbName );
			continue;
		}

		particleDataBlocks.push_back( data );
		dataBlockIds.push_back( data->getId() );
	}

	// Check that we actually found some particle datablocks.

	if( particleDataBlocks.empty() )
	{
		Con::errorf( ConsoleLogEntry::General, "RadiusMeshEmitterData(%s) unable to find any particle datablocks", getName() );
		mReloadSignal.trigger();
		return false;
	}

	// Trigger reload.

	mReloadSignal.trigger();

	return true;
}

//-----------------------------------------------------------------------------
// packUpdate
// Changed
//-----------------------------------------------------------------------------
U32 RadiusMeshEmitter::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if ( stream->writeFlag( mask & InitialUpdateMask ) )
	{
		mathWrite(*stream, getTransform());
		mathWrite(*stream, getScale());
	}

	if( stream->writeFlag( mask & RadiusMeshEmitterMask ) )
	{
		stream->writeString(emitMesh);
		stream->writeFlag(evenEmission);
		stream->writeFlag(emitOnFaces);
		stream->writeVector(center,1000.0f,24,24);
		stream->writeFloat(radius/1000,18);
		stream->writeFlag(active);
		stream->writeFlag(renderIntersections);
		stream->writeFlag(renderEmittingFaces);
		stream->writeFlag(renderSphere);
		stream->writeFlag(shadeByCoverage);
		stream->writeFlag(enableDebugRender);
		stream->writeFlag(active);
		stream->writeFlag(mDeleteWhenEmpty);
	}

	if( stream->writeFlag( mask & particleMask ) )
	{
		stream->write(ejectionPeriodMS);
		stream->write(periodVarianceMS);
		stream->write(ejectionVelocity);
		stream->write(velocityVariance);
		stream->write(ejectionOffset);
		stream->write(lifetimeMS);
		stream->write(softnessDistance);
		stream->write(ambientFactor);
		stream->write(overrideAdvance);
		stream->write(orientParticles);
		stream->write(orientOnVelocity);
		stream->write(useEmitterSizes);
		stream->write(useEmitterColors);
		mathWrite(*stream, alignDirection);
		stream->write(blendStyle);
		stream->write(sortParticles);
		stream->write(reverseOrder);
		stream->write(highResOnly);
		stream->write(renderReflection);
	}

	if( stream->writeFlag( mask & physicsMask) )
	{
		stream->write(sticky);
		stream->write(attractionrange);
		for(int i = 0; i < attrobjectCount; i++)
		{
			stream->write(AttractionMode[i]);
			stream->write(Amount[i]);
			stream->writeString(attractedObjectID[i]);
			stream->writeString(Attraction_offset[i]);
		}
	}

	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
// Changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);

	if ( stream->readFlag() )
	{
		MatrixF temp;
		Point3F tempScale;
		mathRead(*stream, &temp);
		mathRead(*stream, &tempScale);

		setScale(tempScale);
		setTransform(temp);
	}

	// RadiusMeshemitter mask
	if ( stream->readFlag() )
	{
		char buf[256];
		stream->readString(buf);
		emitMesh = dStrdup(buf);
		evenEmission = stream->readFlag();
		emitOnFaces = stream->readFlag();
		stream->readVector(&center,1000.0f,24,24);
		radius = stream->readFloat(18) * 1000;
		loadFaces();
		active = stream->readFlag();
		renderIntersections = stream->readFlag();
		renderEmittingFaces = stream->readFlag();
		renderSphere = stream->readFlag();
		shadeByCoverage = stream->readFlag();
		enableDebugRender = stream->readFlag();
		active = stream->readFlag();
		mDeleteWhenEmpty = stream->readFlag();
	}

	// Particles mask
	if( stream->readFlag() )
	{
		stream->read(&ejectionPeriodMS);
		stream->read(&periodVarianceMS);
		stream->read(&ejectionVelocity);
		stream->read(&velocityVariance);
		stream->read(&ejectionOffset);
		stream->read(&lifetimeMS);
		stream->read(&softnessDistance);
		stream->read(&ambientFactor);
		stream->read(&overrideAdvance);
		stream->read(&orientParticles);
		stream->read(&orientOnVelocity);
		stream->read(&useEmitterSizes);
		stream->read(&useEmitterColors);
		mathRead(*stream, &alignDirection);
		stream->read(&blendStyle);
		stream->read(&sortParticles);
		stream->read(&reverseOrder);
		stream->read(&highResOnly);
		stream->read(&renderReflection);
	}

	// Physics mask
	if( stream->readFlag() )
	{
		stream->read(&sticky);
		stream->read((F32 *)&attractionrange);
		for(int i = 0; i < attrobjectCount; i++)
		{
			stream->read(&AttractionMode[i]);
			stream->read((F32 *)&Amount[i]);
			char buf[256];
			stream->readString(buf);
			attractedObjectID[i] = dStrdup(buf);
			char buf2[256];
			stream->readString(buf2);
			Attraction_offset[i] = dStrdup(buf2);
		}
	}
}

//-----------------------------------------------------------------------------
// onStaticModified
// Added
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::onStaticModified(const char* slotName, const char*newValue)
{
	// Was it an sa_value that was edited? Then set the emitterEdited bit.
	if(strcmp(slotName, "emitMesh") == 0||
		strcmp(slotName, "evenEmission") == 0||
		strcmp(slotName, "emitOnFaces") == 0||
		strcmp(slotName, "center") == 0||
		strcmp(slotName, "radius") == 0||
		strcmp(slotName, "renderIntersections") == 0||
		strcmp(slotName, "renderEmittingFaces") == 0||
		strcmp(slotName, "shadeByCoverage") == 0||
		strcmp(slotName, "enableDebugRender") == 0||
		strcmp(slotName, "renderSphere") == 0||
		strcmp(slotName, "active") == 0)
		setMaskBits(RadiusMeshEmitterMask);

	if(	strcmp(slotName, "ejectionPeriodMS") == 0 ||
		strcmp(slotName, "periodVarianceMS") == 0 ||
		strcmp(slotName, "ejectionVelocity") == 0 ||
		strcmp(slotName, "velocityVariance") == 0 ||
		strcmp(slotName, "ejectionOffset") == 0 ||
		strcmp(slotName, "lifetimeMS") == 0 ||
		strcmp(slotName, "softnessDistance") == 0 ||
		strcmp(slotName, "ambientFactor") == 0 ||
		strcmp(slotName, "overrideAdvance") == 0 ||
		strcmp(slotName, "orientParticles") == 0 ||
		strcmp(slotName, "orientOnVelocity") == 0 ||
		strcmp(slotName, "useEmitterSizes") == 0 ||
		strcmp(slotName, "useEmitterColors") == 0 ||
		strcmp(slotName, "alignParticles") == 0 ||
		strcmp(slotName, "alignDirection") == 0 ||
		strcmp(slotName, "blendStyle") == 0 ||
		strcmp(slotName, "sortParticles") == 0 ||
		strcmp(slotName, "reverseOrder") == 0 ||
		strcmp(slotName, "textureName") == 0 ||
		strcmp(slotName, "textureHandle") == 0 ||
		strcmp(slotName, "highResOnly") == 0 ||
		strcmp(slotName, "renderReflection") == 0)
		setMaskBits(particleMask);

	if( strcmp(slotName, "attracted") == 0 ||
		strcmp(slotName, "attractionrange") == 0 ||
		strcmp(slotName, "Attraction_offset") == 0 ||
		strcmp(slotName, "Amount") == 0 ||
		strcmp(slotName, "sticky") == 0 ||
		strcmp(slotName, "attractedObjectID") == 0 ||
		strcmp(slotName, "AttractionMode") == 0)
		setMaskBits(physicsMask);


	if(!isProperlyAdded())
	{
		if(strcmp(slotName, "Attraction_offset") == 0)
		{
			initialValues.push_back(std::string("Attraction_offset"));
			initialValues.push_back(std::string(Attraction_offset[0]));
			initialValues.push_back(std::string(Attraction_offset[1]));
		}
		else if(strcmp(slotName, "attractedObjectID") == 0)
		{
			initialValues.push_back(std::string("attractedObjectID"));
			initialValues.push_back(std::string(attractedObjectID[0]));
			initialValues.push_back(std::string(attractedObjectID[1]));
		}
		else if(strcmp(slotName, "AttractionMode") == 0)
		{
			initialValues.push_back(std::string("AttractionMode"));
			std::stringstream ss;
			ss << AttractionMode[0];
			initialValues.push_back(std::string(ss.str()));
			ss.str("");
			ss << AttractionMode[1];
			initialValues.push_back(std::string(ss.str()));
		}
		else if(strcmp(slotName, "Amount") == 0)
		{
			initialValues.push_back(std::string("Amount"));
			std::stringstream ss;
			ss << Amount[0];
			initialValues.push_back(std::string(ss.str()));
			ss.str("");
			ss << Amount[1];
			initialValues.push_back(std::string(ss.str()));
		}
		else{
			//Save the values and add them onNewDataBlock
			initialValues.push_back(std::string(slotName));
			initialValues.push_back(std::string(newValue));
		}
	}
}

//-----------------------------------------------------------------------------
// inspectPostApply
// Not changed
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::inspectPostApply()
{
	Parent::inspectPostApply();
	setMaskBits( StateMask );
}

//-----------------------------------------------------------------------------
// loadFaces
//  - This function calculates the area of all the triangles in the RadiusMesh
//  - finds the average area, and adds the triangles one or more times
//  - to the emitfaces vector based on their area relative to the other faces
//  - not a perfect way to do it, but it works fine.
// Custom
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::loadFaces()
{
	PROFILE_SCOPE(RadiusMeshEmitLoadFaces);
	emitfaces.clear();
	vertexCount = 0;
	SimObject* SB = dynamic_cast<SimObject*>(Sim::findObject(emitMesh));
	if(!SB)
		SB = dynamic_cast<SimObject*>(Sim::findObject(atoi(emitMesh)));
	ShapeBase* SS;
	TSStatic* TS;
	if(SB){
		SS = dynamic_cast<ShapeBase*>(SB);
		TS = dynamic_cast<TSStatic*>(SB);
	}
	// Make sure that we are dealing with some proper objects
	if(SB && (SS || TS)){
		TSShapeInstance* shape;
		if(SS)
			shape = SS->getShapeInstance();
		else{
			shape = TS->getShapeInstance();
		}
		std::vector<IPSCore::face> triangles;
		U32 trisIndex = 0;
		bool skinMesh = false;
		bool staticMesh = false;
		for(S32 meshIndex = 0; meshIndex < shape->getShape()->meshes.size(); meshIndex++)
		{
			//RadiusMesh = shape->getShape()->meshes[meshIndex];
			TSSkinMesh* sMesh = dynamic_cast<TSSkinMesh*>(shape->getShape()->meshes[meshIndex]);
			if(sMesh)
			{
				if(sMesh->mVertexData.size()){
					skinMesh = true;
				}
			}
			else
				if(shape->getShape()->meshes[meshIndex] && shape->getShape()->meshes[meshIndex]->mVertexData.size())
					staticMesh = true;
		}
		for (S32 meshIndex = 0; meshIndex < shape->getShape()->meshes.size(); meshIndex++)  
		{
			TSSkinMesh* sMesh;
			TSMesh* Mesh;

			Mesh = shape->getShape()->meshes[meshIndex];

			sMesh = dynamic_cast<TSSkinMesh*>(shape->getShape()->meshes[meshIndex]);
			skinMesh = false;
			staticMesh = false;
			if(sMesh)
			{
				if(sMesh->mVertexData.size()){
					skinMesh = true;
				}
			}
			else
				if(Mesh && Mesh->mVertexData.size())
					staticMesh = true;

#pragma region skinMesh
			if(skinMesh)
			{
				if(!sMesh)
					continue;

				U32 numVerts = sMesh->mVertexData.size();
				if(!numVerts)
					continue;
				vertexCount += numVerts;

				U32 numPrims = sMesh->primitives.size();
				if(!numPrims)
					continue;

				U32 numIndices = sMesh->indices.size();
				if(!numIndices)
					continue;

				for( U32 primIndex = 0; primIndex < numPrims; primIndex++ )
				{
					U32 start = sMesh->primitives[primIndex].start;

					U32 numElements = sMesh->primitives[primIndex].numElements;

					if ( (sMesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles) 
					{
						TSMesh::__TSMeshVertexBase v1, v2, v3;
						Point3F p1, p2, p3;

						for (U32 triIndex = 0; triIndex < numElements; triIndex+=3)
						{
							U32 triStart = start + triIndex;
							v1 = sMesh->mVertexData[sMesh->indices[triStart]];
							v2 = sMesh->mVertexData[sMesh->indices[triStart + 1]];
							v3 = sMesh->mVertexData[sMesh->indices[triStart + 2]];

							Point3F scale;
							if(SS)
								scale = SS->getScale();
							else
								scale = TS->getScale();
							Point3F worldCenter;
							MatrixF trans;
							MatrixF nodetrans;
							MatrixF mat;
							if(SS)
							{
								trans = SS->getTransform();
								trans.mulV(v1.vert(),&p1);
								trans.mulV(v2.vert(),&p2);
								trans.mulV(v3.vert(),&p3);
							}
							if(TS)
							{
								trans = TS->getTransform();
								nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
								trans.mulV(v1.vert(),&p1);
								trans.mulV(v2.vert(),&p2);
								trans.mulV(v3.vert(),&p3);

								mat.mul(trans,nodetrans);
								mat.mulV(v1.vert(),&p1);
								mat.mulV(v2.vert(),&p2);
								mat.mulV(v3.vert(),&p3);
								p1 *= TS->getScale();
								p2 *= TS->getScale();
								p3 *= TS->getScale();
							}
							IPSCore::face tris;
							tris.meshIndex = meshIndex;
							tris.skinMesh = true;
							tris.inboundSphere = false;
							tris.triStart = triStart;
							if(p1 == p2 || p2 == p3 || p3 == p1)
								continue;
							tris.sideAIntersect = IPSCore::raySphereIntersection(center,radius,p1,p2);
							tris.sideBIntersect = IPSCore::raySphereIntersection(center,radius,p2,p3);
							tris.sideCIntersect = IPSCore::raySphereIntersection(center,radius,p3,p1);
							tris.v1Inside = false;
							tris.v2Inside = false;
							tris.v3Inside = false;
							tris.arcArea1 = 0;
							tris.arcArea2 = 0;
							tris.arcArea1Coeff = 0;
							tris.arcArea2Coeff = 0;

							tris.vertsInside = 0;
							if((p1-center).len() <= radius){
								tris.v1Inside = true;
								tris.vertsInside++;
							}
							if((p2-center).len() <= radius){
								tris.v2Inside = true;
								tris.vertsInside++;
							}
							if((p3-center).len() <= radius){
								tris.v3Inside = true;
								tris.vertsInside++;
							}

							if(tris.vertsInside == 3){
								Point3F veca = p1 - p2;
								Point3F vecb = p2 - p3;
								Point3F vecc = p3 - p1;
								tris.area = IPSCore::HeronsF(veca, vecb, vecc);
								triangles.push_back(tris);
							}
							if(tris.vertsInside == 2)
							{
								Point3F veca;
								Point3F vecb;
								Point3F vecc;
								if(tris.v1Inside && tris.v2Inside && !tris.v3Inside)
								{
									veca = p2-p1;
									vecb = tris.sideCIntersect.intersection1 - p2;
									vecc = p1 - tris.sideCIntersect.intersection1;
									tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

									veca = tris.sideBIntersect.intersection1 - p2;
									vecb = tris.sideCIntersect.intersection1 - tris.sideBIntersect.intersection1;
									vecc = tris.sideCIntersect.intersection1 - p2;
									tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);
									tris.area = tris.trapezoidArea1 + tris.trapezoidArea2;
									tris.trapezoidCoef = tris.trapezoidArea1 / tris.area;
								}
								if(tris.v1Inside && !tris.v2Inside && tris.v3Inside)
								{
									veca = p1-p3;
									vecb = tris.sideAIntersect.intersection1 - p1;
									vecc = p3 - tris.sideAIntersect.intersection1;
									tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

									veca = tris.sideAIntersect.intersection1 - p1;
									vecb = tris.sideBIntersect.intersection1 - tris.sideAIntersect.intersection1;
									vecc = tris.sideBIntersect.intersection1 - p1;
									tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);
									tris.area = tris.trapezoidArea1 + tris.trapezoidArea2;
									tris.trapezoidCoef = tris.trapezoidArea1 / tris.area;
								}
								if(!tris.v1Inside && tris.v2Inside && tris.v3Inside)
								{
									veca = p3-p2;
									vecb = tris.sideAIntersect.intersection1 - p3;
									vecc = p2 - tris.sideAIntersect.intersection1;
									tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

									veca = tris.sideCIntersect.intersection1 - p3;
									vecb = tris.sideAIntersect.intersection1 - tris.sideCIntersect.intersection1;
									vecc = tris.sideAIntersect.intersection1 - p3;
									tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);
									tris.area = tris.trapezoidArea1 + tris.trapezoidArea2;
									tris.trapezoidCoef = tris.trapezoidArea1 / tris.area;
								}
								veca = p1 - p2;
								vecb = p2 - p3;
								vecc = p3 - p1;
								tris.fullArea = IPSCore::HeronsF(veca, vecb, vecc);
								triangles.push_back(tris);
							}
							if(tris.vertsInside == 1)
							{
								Point3F veca,vecb,vecc;
								Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
								Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
								Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
								Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
								Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
								Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));

								PlaneF plane = PlaneF(p1,p2,p3);
								Point3F PlaneP = plane.project(center);
								if(tris.v1Inside && !tris.v2Inside && !tris.v3Inside)
								{
									veca = sideA1 - p1;
									vecb = sideC1 - p1;
									vecc = sideA1 - sideC1;

									tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, radius, sideC1, sideA1);

								}
								if(!tris.v1Inside && tris.v2Inside && !tris.v3Inside)
								{
									veca = sideB1 - p2;
									vecb = sideA1 - p2;
									vecc = sideB1 - sideA1;

									tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, radius, sideA1, sideB1);
								}
								if(!tris.v1Inside && !tris.v2Inside && tris.v3Inside)
								{
									veca = sideC1 - p3;
									vecb = sideB1 - p3;
									vecc = sideC1 - sideB1;

									tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, (PlaneP-sideB1).len(), sideB1, sideC1);
								}
								tris.area = IPSCore::HeronsF(veca, vecb, vecc);
								tris.fullArea = IPSCore::HeronsF(veca, vecb, vecc) + tris.arcArea1;
								tris.arcArea1Coeff = tris.arcArea1 / tris.area;
								triangles.push_back(tris);
							}
							if(tris.vertsInside == 0)
							{
								PlaneF plane = PlaneF(p1,p2,p3);
								Point3F PlaneP = plane.project(center);
								IPSCore::triSphereIntersection(emitMesh, &tris, center, radius, p1, p2, p3);
								if(tris.inboundSphere)
								{
									tris.area = M_PI_F * IPSCore::square(radius);
									if(tris.sideAIntersect.count == 2)
									{
										Point3F c1 = p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff);
										Point3F c2 = p1+((p2-p1) * tris.sideAIntersect.intersection2Coeff);
										tris.area -= IPSCore::AreaOfCircularSegment(center,radius,c1,c2);
									}
									if(tris.sideBIntersect.count == 2)
									{
										Point3F c1 = p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff);
										Point3F c2 = p2+((p3-p2) * tris.sideBIntersect.intersection2Coeff);
										tris.area -= IPSCore::AreaOfCircularSegment(center,radius,c1,c2);
									}
									if(tris.sideCIntersect.count == 2)
									{
										Point3F c1 = p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff);
										Point3F c2 = p3+((p1-p3) * tris.sideCIntersect.intersection2Coeff);
										tris.area -= IPSCore::AreaOfCircularSegment(center,radius,c1,c2);
									}
									triangles.push_back(tris);
								}
								if(((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count != 2) || 
									(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
									(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)) &&
									!tris.inboundSphere)
								{
									PlaneF plane = PlaneF(p1,p2,p3);
									Point3F PlaneP = plane.project(center);
									if(tris.sideAIntersect.count == 2)
									{
										Point3F c1 = p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff);
										Point3F c2 = p1+((p2-p1) * tris.sideAIntersect.intersection2Coeff);
										tris.area = IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
									}
									if(tris.sideBIntersect.count == 2)
									{
										Point3F c1 = p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff);
										Point3F c2 = p2+((p3-p2) * tris.sideBIntersect.intersection2Coeff);
										tris.area = IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
									}
									if(tris.sideCIntersect.count == 2)
									{
										Point3F c1 = p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff);
										Point3F c2 = p3+((p1-p3) * tris.sideCIntersect.intersection2Coeff);
										tris.area = IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
									}
									triangles.push_back(tris);
								}

								// Trapezoid with arcs -- 4 intersections
								if((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
									(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2) || 
									(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2) &&
									!tris.inboundSphere)
								{
									Point3F veca = p1 - p2;
									Point3F vecb = p2 - p3;
									Point3F vecc = p3 - p1;
									Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
									Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
									Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
									Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
									Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
									Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));
									PlaneF plane = PlaneF(p1,p2,p3);
									Point3F PlaneP = plane.project(center);
									F32 a1radius = (sideA1 - PlaneP).len();
									F32 b1radius = (sideB1 - PlaneP).len();
									F32 c1radius = (sideC1 - PlaneP).len();

									tris.area = IPSCore::HeronsF(veca, vecb, vecc);
									if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2)
									{
										//trapezoid 1
										veca = sideB1 - sideA1;
										vecb = sideB2 - sideB1;
										vecc = sideA1 - sideB2;
										tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

										//trapezoid 2
										veca = sideA2 - sideA1;
										vecb = sideB1 - sideA2;
										vecc = sideA1 - sideB1;
										tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);

										tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, a1radius, sideA1, sideB2);
										tris.arcArea2 = IPSCore::AreaOfCircularSegment(PlaneP, a1radius, sideB1, sideA2);

										tris.area = tris.trapezoidArea1 + tris.trapezoidArea2 + tris.arcArea1 + tris.arcArea2;
										tris.trapezoidCoef = tris.trapezoidArea1 / (tris.trapezoidArea1 + tris.trapezoidArea2);
										tris.arcArea1Coeff = tris.arcArea1 / tris.area;
										tris.arcArea2Coeff = tris.arcArea2 / tris.area;
									}
									if(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2)
									{
										//trapezoid 1
										veca = sideC1 - sideB1;
										vecb = sideC2 - sideC1;
										vecc = sideB1 - sideC2;
										tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

										//trapezoid 2
										veca = sideB2 - sideB1;
										vecb = sideC1 - sideB2;
										vecc = sideB1 - sideC1;
										tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);

										tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, b1radius, sideB1, sideC2);
										tris.arcArea2 = IPSCore::AreaOfCircularSegment(PlaneP, b1radius, sideC1, sideB2);

										tris.area = tris.trapezoidArea1 + tris.trapezoidArea2 + tris.arcArea1 + tris.arcArea2;
										tris.trapezoidCoef = tris.trapezoidArea1 / (tris.trapezoidArea1 + tris.trapezoidArea2);
										tris.arcArea1Coeff = tris.arcArea1 / tris.area;
										tris.arcArea2Coeff = tris.arcArea2 / tris.area;
									}
									if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)
									{
										//trapezoid 1
										veca = sideA1 - sideC1;
										vecb = sideA2 - sideA1;
										vecc = sideC1 - sideA2;
										tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

										//trapezoid 2
										veca = sideC2 - sideC1;
										vecb = sideA1 - sideC2;
										vecc = sideC1 - sideA1;
										tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);

										tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, c1radius, sideC1, sideA2);
										tris.arcArea2 = IPSCore::AreaOfCircularSegment(PlaneP, c1radius, sideA1, sideC2);

										tris.area = tris.trapezoidArea1 + tris.trapezoidArea2 + tris.arcArea1 + tris.arcArea2;
										tris.trapezoidCoef = tris.trapezoidArea1 / (tris.trapezoidArea1 + tris.trapezoidArea2);
										tris.arcArea1Coeff = tris.arcArea1 / tris.area;
										tris.arcArea2Coeff = tris.arcArea2 / tris.area;
									}
									triangles.push_back(tris);
								}
							}
						}
					}
					else
					{
						Con::printf("Not tris");
					}
				}
			}
#pragma endregion
#pragma region !skinMesh
			if(staticMesh)
			{
				if(!Mesh)
					continue;
				S32 numVerts = Mesh->mVertexData.size();
				if(!numVerts)
					continue;
				vertexCount += numVerts;

				S32 numPrims = Mesh->primitives.size();
				if(!numPrims)
					continue;

				S32 numIndices = Mesh->indices.size();
				if(!numIndices)
					continue;
				for( S32 primIndex = 0; primIndex < numPrims; primIndex++ )
				{
					U32 start = Mesh->primitives[primIndex].start;
					U32 numElements = Mesh->primitives[primIndex].numElements;
					if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles) 
					{
						TSMesh::__TSMeshVertexBase v1, v2, v3;
						Point3F p1, p2, p3;
						for (U32 triIndex = 0; triIndex < numElements; triIndex+=3)
						{
							U32 triStart = start + triIndex;
							v1 = Mesh->mVertexData[Mesh->indices[triStart]];
							v2 = Mesh->mVertexData[Mesh->indices[triStart + 1]];
							v3 = Mesh->mVertexData[Mesh->indices[triStart + 2]];
							

							Point3F scale;
							if(SS)
								scale = SS->getScale();
							else
								scale = TS->getScale();
							Point3F worldCenter;
							MatrixF trans;
							MatrixF nodetrans;
							MatrixF mat;
							if(SS)
							{
								trans = SS->getTransform();
								trans.mulP(v1.vert(),&p1);
								trans.mulP(v2.vert(),&p2);
								trans.mulP(v3.vert(),&p3);
							}
							if(TS)
							{
								trans = TS->getTransform();
								nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
								//trans.mul(nodetrans);
								trans.mulV(v1.vert(),&p1);
								trans.mulV(v2.vert(),&p2);
								trans.mulV(v3.vert(),&p3);

								mat.mul(trans,nodetrans);
								mat.mulV(v1.vert(),&p1);
								mat.mulV(v2.vert(),&p2);
								mat.mulV(v3.vert(),&p3);
								p1 *= TS->getScale();
								p2 *= TS->getScale();
								p3 *= TS->getScale();
							}
							/*p1 = v1.vert();
							p2 = v2.vert();
							p3 = v3.vert();*/

							IPSCore::face tris;
							tris.meshIndex = meshIndex;
							tris.skinMesh = false;
							tris.inboundSphere = false;
							tris.triStart = triStart;
							tris.sideAIntersect = IPSCore::raySphereIntersection(center,radius,p1,p2);
							tris.sideBIntersect = IPSCore::raySphereIntersection(center,radius,p2,p3);
							tris.sideCIntersect = IPSCore::raySphereIntersection(center,radius,p3,p1);
							tris.v1Inside = false;
							tris.v2Inside = false;
							tris.v3Inside = false;
							F32 length = (p1-center).len();
							tris.vertsInside = 0;
							if((p1-center).len() <= radius){
								tris.v1Inside = true;
								tris.vertsInside++;
							}
							if((p2-center).len() <= radius){
								tris.v2Inside = true;
								tris.vertsInside++;
							}
							if((p3-center).len() <= radius){
								tris.v3Inside = true;
								tris.vertsInside++;
							}
							if(tris.vertsInside == 3){
								Point3F veca = p1 - p2;
								Point3F vecb = p2 - p3;
								Point3F vecc = p3 - p1;
								tris.area = IPSCore::HeronsF(veca, vecb, vecc);
								triangles.push_back(tris);
							}
							if(tris.vertsInside == 2)
							{
								Point3F veca;
								Point3F vecb;
								Point3F vecc;
								if(tris.v1Inside && tris.v2Inside && !tris.v3Inside)
								{
									veca = p2-p1;
									vecb = tris.sideCIntersect.intersection1 - p2;
									vecc = p1 - tris.sideCIntersect.intersection1;
									tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

									veca = tris.sideBIntersect.intersection1 - p2;
									vecb = tris.sideCIntersect.intersection1 - tris.sideBIntersect.intersection1;
									vecc = tris.sideCIntersect.intersection1 - p2;
									tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);
									tris.area = tris.trapezoidArea1 + tris.trapezoidArea2;
									tris.trapezoidCoef = tris.trapezoidArea1 / tris.area;
								}
								if(tris.v1Inside && !tris.v2Inside && tris.v3Inside)
								{
									veca = p1 - p3;
									vecb = tris.sideAIntersect.intersection1Coeff*(p2-p1);
									vecc = tris.sideAIntersect.intersection1Coeff*(p2-p1) - p3;
									tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

									veca = tris.sideBIntersect.intersection1Coeff*(p3-p2) - p3;
									vecb = tris.sideAIntersect.intersection1Coeff*(p2-p1) - tris.sideBIntersect.intersection1Coeff*(p3-p2);
									vecc = p3 - tris.sideAIntersect.intersection1Coeff*(p2-p1);
									tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);
									tris.area = tris.trapezoidArea1 + tris.trapezoidArea2;
									tris.trapezoidCoef = tris.trapezoidArea1 / tris.area;
								}
								if(!tris.v1Inside && tris.v2Inside && tris.v3Inside)
								{
									veca = p3-p2;
									vecb = tris.sideAIntersect.intersection1 - p3;
									vecc = p2 - tris.sideAIntersect.intersection1;
									tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

									veca = tris.sideCIntersect.intersection1 - p3;
									vecb = tris.sideAIntersect.intersection1 - tris.sideCIntersect.intersection1;
									vecc = tris.sideAIntersect.intersection1 - p3;
									tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);
									tris.area = tris.trapezoidArea1 + tris.trapezoidArea2;
									tris.trapezoidCoef = tris.trapezoidArea1 / tris.area;
								}
								veca = p1 - p2;
								vecb = p2 - p3;
								vecc = p3 - p1;
								tris.fullArea = IPSCore::HeronsF(veca, vecb, vecc);
								triangles.push_back(tris);
							}
							if(tris.vertsInside == 1)
							{
								Point3F veca,vecb,vecc;
								Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
								Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
								Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
								Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
								Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
								Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));

								PlaneF plane = PlaneF(p1,p2,p3);
								Point3F PlaneP = plane.project(center);
								if(tris.v1Inside && !tris.v2Inside && !tris.v3Inside)
								{
									veca = sideA1 - p1;
									vecb = sideC1 - p1;
									vecc = sideA1 - sideC1;

									tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, radius, sideC1, sideA1);

								}
								if(!tris.v1Inside && tris.v2Inside && !tris.v3Inside)
								{
									veca = sideB1 - p2;
									vecb = sideA1 - p2;
									vecc = sideB1 - sideA1;

									tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, radius, sideA1, sideB1);
								}
								if(!tris.v1Inside && !tris.v2Inside && tris.v3Inside)
								{
									veca = sideC1 - p3;
									vecb = sideB1 - p3;
									vecc = sideC1 - sideB1;

									tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, (PlaneP-sideB1).len(), sideB1, sideC1);
								}
								tris.area = IPSCore::HeronsF(veca, vecb, vecc) + tris.arcArea1;
								veca = p1 - p2;
								vecb = p2 - p3;
								vecc = p3 - p1;
								tris.fullArea = IPSCore::HeronsF(veca, vecb, vecc);
								tris.arcArea1Coeff = tris.arcArea1 / tris.area;
								triangles.push_back(tris);
							}
							if(tris.vertsInside == 0)
							{
								PlaneF plane = PlaneF(p1,p2,p3);
								Point3F PlaneP = plane.project(center);
								IPSCore::triSphereIntersection(emitMesh, &tris, center, radius, p1, p2, p3);
								if(tris.inboundSphere)
								{
									tris.area = M_PI_F * IPSCore::square(radius);
									if(tris.sideAIntersect.count == 2)
									{
										Point3F c1 = p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff);
										Point3F c2 = p1+((p2-p1) * tris.sideAIntersect.intersection2Coeff);
										tris.area -= IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
									}
									if(tris.sideBIntersect.count == 2)
									{
										Point3F c1 = p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff);
										Point3F c2 = p2+((p3-p2) * tris.sideBIntersect.intersection2Coeff);
										tris.area -= IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
									}
									if(tris.sideCIntersect.count == 2)
									{
										Point3F c1 = p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff);
										Point3F c2 = p3+((p1-p3) * tris.sideCIntersect.intersection2Coeff);
										tris.area -= IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
									}
									triangles.push_back(tris);
								}

								if(((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count != 2) || 
									(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
									(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)) &&
									!tris.inboundSphere)
								{
									PlaneF plane = PlaneF(p1,p2,p3);
									Point3F PlaneP = plane.project(center);
									if(tris.sideAIntersect.count == 2)
									{
										Point3F c1 = p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff);
										Point3F c2 = p1+((p2-p1) * tris.sideAIntersect.intersection2Coeff);
										tris.area = IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
									}
									if(tris.sideBIntersect.count == 2)
									{
										Point3F c1 = p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff);
										Point3F c2 = p2+((p3-p2) * tris.sideBIntersect.intersection2Coeff);
										tris.area = IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
									}
									if(tris.sideCIntersect.count == 2)
									{
										Point3F c1 = p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff);
										Point3F c2 = p3+((p1-p3) * tris.sideCIntersect.intersection2Coeff);
										tris.area = IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
									}
									triangles.push_back(tris);
								}

								// Trapezoid with arcs -- 4 intersections
								if((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
									(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2) || 
									(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2) &&
									!tris.inboundSphere)
								{
									Point3F veca = p1 - p2;
									Point3F vecb = p2 - p3;
									Point3F vecc = p3 - p1;
									Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
									Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
									Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
									Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
									Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
									Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));
									PlaneF plane = PlaneF(p1,p2,p3);
									Point3F PlaneP = plane.project(center);
									F32 a1radius = (sideA1 - PlaneP).len();
									F32 b1radius = (sideB1 - PlaneP).len();
									F32 c1radius = (sideC1 - PlaneP).len();

									tris.area = IPSCore::HeronsF(veca, vecb, vecc);
									if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2)
									{
										//trapezoid 1
										veca = sideB1 - sideA1;
										vecb = sideB2 - sideB1;
										vecc = sideA1 - sideB2;
										tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

										//trapezoid 2
										veca = sideA2 - sideA1;
										vecb = sideB1 - sideA2;
										vecc = sideA1 - sideB1;
										tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);

										tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, a1radius, sideA1, sideB2);
										tris.arcArea2 = IPSCore::AreaOfCircularSegment(PlaneP, a1radius, sideB1, sideA2);

										tris.area = tris.trapezoidArea1 + tris.trapezoidArea2 + tris.arcArea1 + tris.arcArea2;
										tris.trapezoidCoef = tris.trapezoidArea1 / (tris.trapezoidArea1 + tris.trapezoidArea2);
										tris.arcArea1Coeff = tris.arcArea1 / tris.area;
										tris.arcArea2Coeff = tris.arcArea2 / tris.area;
									}
									if(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2)
									{
										//trapezoid 1
										veca = sideC1 - sideB1;
										vecb = sideC2 - sideC1;
										vecc = sideB1 - sideC2;
										tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

										//trapezoid 2
										veca = sideB2 - sideB1;
										vecb = sideC1 - sideB2;
										vecc = sideB1 - sideC1;
										tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);

										tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, b1radius, sideB1, sideC2);
										tris.arcArea2 = IPSCore::AreaOfCircularSegment(PlaneP, b1radius, sideC1, sideB2);

										tris.area = tris.trapezoidArea1 + tris.trapezoidArea2 + tris.arcArea1 + tris.arcArea2;
										tris.trapezoidCoef = tris.trapezoidArea1 / (tris.trapezoidArea1 + tris.trapezoidArea2);
										tris.arcArea1Coeff = tris.arcArea1 / tris.area;
										tris.arcArea2Coeff = tris.arcArea2 / tris.area;
									}
									if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)
									{
										//trapezoid 1
										veca = sideA1 - sideC1;
										vecb = sideA2 - sideA1;
										vecc = sideC1 - sideA2;
										tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

										//trapezoid 2
										veca = sideC2 - sideC1;
										vecb = sideA1 - sideC2;
										vecc = sideC1 - sideA1;
										tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);

										tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, c1radius, sideC1, sideA2);
										tris.arcArea2 = IPSCore::AreaOfCircularSegment(PlaneP, c1radius, sideA1, sideC2);
										tris.area = tris.trapezoidArea1 + tris.trapezoidArea2 + tris.arcArea1 + tris.arcArea2;
										tris.trapezoidCoef = tris.trapezoidArea1 / (tris.trapezoidArea1 + tris.trapezoidArea2);
										tris.arcArea1Coeff = tris.arcArea1 / tris.area;
										tris.arcArea2Coeff = tris.arcArea2 / tris.area;
									}
									triangles.push_back(tris);
								}
							}
						}
					}
					else
					{
						Con::printf("Not tris");
					}
				}

			}
#pragma endregion
		}

		F32 averageArea = 0;
		for(S32 index = 0; index < triangles.size(); index++)
		{
			averageArea += triangles[index].area;
		}
		averageArea = averageArea / triangles.size();

		for(S32 index = 0; index < triangles.size(); index++)
		{
			float n = triangles[index].area / averageArea;
			float t;
			t = n - floor(n);
			if (t >= 0.5)    
				n = ceil(n);
			else 
				n = floor(n);
			for(int i = -1; i < n; i++)
			{
				emitfaces.push_back(triangles[index]);
			}
		}
	}
}

rm_DeleteEvent::rm_DeleteEvent(S32 id)
{
	mNode = id;
}

rm_DeleteEvent::~rm_DeleteEvent()
{
	//dFree(&Max);
	//dFree(&mNode);
}

void rm_DeleteEvent::pack(NetConnection* conn, BitStream *bstream)
{
	bstream->write(mNode);
}

void rm_DeleteEvent::unpack(NetConnection *conn, BitStream *bstream)
{
	bstream->read(&mNode);
}
NetConnection* conn = NetConnection::getConnectionToServer();

// This just prints the event in the console. You might
// want to do something more clever here -- BJG
void rm_DeleteEvent::process(NetConnection *conn)
{
	RadiusMeshEmitter* m_Node = dynamic_cast< RadiusMeshEmitter* >(conn->resolveObjectFromGhostIndex(mNode));
	if(m_Node)
		m_Node->safeDeleteObject();
}

void rm_DeleteEvent::write(NetConnection* conn, BitStream *bstream)
{
	pack(conn, bstream);
}

IMPLEMENT_CO_NETEVENT_V1(rm_DeleteEvent);



DefineEngineMethod(RadiusMeshEmitterData, reload, void,(),,
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

DefineEngineMethod(RadiusMeshEmitter, DeleteWhenEmpty, void, (void),,
	"Delete the emitter.\n")
{
	object->active = false;
	object->tickToDeath(true);
	//object->deleteWhenEmpty();
}