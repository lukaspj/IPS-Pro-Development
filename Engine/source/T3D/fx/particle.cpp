//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
#include "particle.h"
#include "console/consoleTypes.h"
#include "console/typeValidators.h"
#include "core/stream/bitStream.h"
#include "math/mRandom.h"
#include "console/engineAPI.h"

ConsoleDocClass( ParticleData,
                "@brief Contains information for how specific particles should look and react "
                "including particle colors, particle imagemap, acceleration value for individual "
                "particles and spin information.\n"

                "@tsexample\n"
                "datablock ParticleData( GLWaterExpSmoke )\n"
                "{\n"
                "   textureName = \"art/shapes/particles/smoke\";\n"
                "   dragCoefficient = 0.4;\n"
                "   gravityCoefficient = -0.25;\n"
                "   inheritedVelFactor = 0.025;\n"
                "   constantAcceleration = -1.1;\n"
                "   lifetimeMS = 1250;\n"
                "   lifetimeVarianceMS = 0;\n"
                "   useInvAlpha = false;\n"
                "   spinSpeed = 1;\n"
                "   spinRandomMin = -200.0;\n"
                "   spinRandomMax = 200.0;\n\n"
                "   colors[0] = \"0.1 0.1 1.0 1.0\";\n"
                "   colors[1] = \"0.4 0.4 1.0 1.0\";\n"
                "   colors[2] = \"0.4 0.4 1.0 0.0\";\n\n"
                "   sizes[0] = 2.0;\n"
                "   sizes[1] = 6.0;\n"
                "   sizes[2] = 2.0;\n\n"
                "   times[0] = 0.0;\n"
                "   times[1] = 0.5;\n"
                "   times[2] = 1.0;\n"
                "};\n"
                "@endtsexample\n"

                "@ingroup FX\n"
                "@see ParticleEmitter\n"
                "@see ParticleEmitterData\n"
                "@see ParticleEmitterNode\n"
                );

static const float sgDefaultWindCoefficient = 0.0f;
static const float sgDefaultConstantAcceleration = 0.f;


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
ParticleData::ParticleData()
{
   dragCoefficient      = 0.0f;
   windCoefficient      = sgDefaultWindCoefficient;
   gravityCoefficient   = 0.0f;
   inheritedVelFactor   = 0.0f;
   constantAcceleration = sgDefaultConstantAcceleration;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 0;

   S32 i;
   for( i=0; i<PDC_NUM_KEYS; i++ )
   {
      sizes[i] = 1.0;
   }

   times[0] = 0.0f;
   times[1] = 0.33f;
   times[2] = 0.66f;
   times[3] = 1.0f;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
ParticleData::~ParticleData()
{
}

FRangeValidator dragCoefFValidator(0.f, 5.f);
FRangeValidator gravCoefFValidator(-10.f, 10.f);

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void ParticleData::initPersistFields()
{
   addFieldV( "dragCoefficient", TYPEID< F32 >(), Offset(dragCoefficient, ParticleData), &dragCoefFValidator,
      "Particle physics drag amount." );
   addField( "windCoefficient", TYPEID< F32 >(), Offset(windCoefficient, ParticleData),
      "Strength of wind on the particles." );
   addFieldV( "gravityCoefficient", TYPEID< F32 >(), Offset(gravityCoefficient, ParticleData), &gravCoefFValidator,
      "Strength of gravity on the particles." );
   addFieldV( "inheritedVelFactor", TYPEID< F32 >(), Offset(inheritedVelFactor, ParticleData), &CommonValidators::NormalizedFloat,
      "Amount of emitter velocity to add to particle initial velocity." );
   addField( "constantAcceleration", TYPEID< F32 >(), Offset(constantAcceleration, ParticleData),
      "Constant acceleration to apply to this particle." );
   addField( "lifetimeMS", TYPEID< S32 >(), Offset(lifetimeMS, ParticleData),
      "Time in milliseconds before this particle is destroyed." );
   addField( "lifetimeVarianceMS", TYPEID< S32 >(), Offset(lifetimeVarianceMS, ParticleData),
      "Variance in lifetime of particle, from 0 - lifetimeMS." );
   
   addProtectedField( "sizes", TYPEID< F32 >(), Offset(sizes, ParticleData), &protectedSetSizes, 
      &defaultProtectedGetFn, PDC_NUM_KEYS,
      "@brief Particle size keyframe values.\n\n"
      "The particle size will linearly interpolate between the size/time keys "
      "over the lifetime of the particle." );

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// Pack data
//-----------------------------------------------------------------------------
void ParticleData::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeFloat(dragCoefficient / 5, 10);
   if( stream->writeFlag(windCoefficient != sgDefaultWindCoefficient ) )
      stream->write(windCoefficient);
   if (stream->writeFlag(gravityCoefficient != 0.0f))
      stream->writeSignedFloat(gravityCoefficient / 10, 12); 
   stream->writeFloat(inheritedVelFactor, 9);
   if( stream->writeFlag( constantAcceleration != sgDefaultConstantAcceleration ) )
      stream->write(constantAcceleration);

   stream->write( lifetimeMS );
   stream->write( lifetimeVarianceMS );

   S32 i, count;

   // see how many frames there are:
   for(count = 0; count < PDC_NUM_KEYS - 1; count++)
      if(times[count] >= 1)
         break;

   count++;

   stream->writeInt(count, PDC_NUM_BITS);

   for( i=0; i<count; i++ )
   {
      stream->writeFloat( times[i], 8);
      stream->writeFloat( sizes[i]/MaxParticleSize, 14);
   }
}

//-----------------------------------------------------------------------------
// Unpack data
//-----------------------------------------------------------------------------
void ParticleData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   dragCoefficient = stream->readFloat(10) * 5;
   if(stream->readFlag())
      stream->read(&windCoefficient);
   else
      windCoefficient = sgDefaultWindCoefficient;
   if (stream->readFlag()) 
      gravityCoefficient = stream->readSignedFloat(12)*10; 
   else 
      gravityCoefficient = 0.0f; 
   inheritedVelFactor = stream->readFloat(9);
   if(stream->readFlag())
      stream->read(&constantAcceleration);
   else
      constantAcceleration = sgDefaultConstantAcceleration;

   stream->read( &lifetimeMS );
   stream->read( &lifetimeVarianceMS );

   S32 i;
   S32 count = stream->readInt(PDC_NUM_BITS);
   for(i = 0;i < count; i++)
   {
      times[i] = stream->readFloat(8);
      sizes[i] = stream->readFloat(14) * MaxParticleSize;
   }
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool ParticleData::onAdd()
{
   if (Parent::onAdd() == false)
      return false;

   if (dragCoefficient < 0.0) {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) drag coeff less than 0", getName());
      dragCoefficient = 0.0f;
   }
   if (lifetimeMS < 1) {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) lifetime < 1 ms", getName());
      lifetimeMS = 1;
   }
   if (lifetimeVarianceMS >= lifetimeMS) {
      Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) lifetimeVariance >= lifetime", getName());
      lifetimeVarianceMS = lifetimeMS - 1;
   }
   times[0] = 0.0f;
   for (U32 i = 1; i < 4; i++) {
      if (times[i] < times[i-1]) {
         Con::warnf(ConsoleLogEntry::General, "ParticleData(%s) times[%d] < times[%d]", getName(), i, i-1);
         times[i] = times[i-1];
      }
   }

   return true;
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool ParticleData::preload(bool server, String &errorStr)
{
   if (Parent::preload(server, errorStr) == false)
      return false;

   return true;
}

bool ParticleData::reload(char errorBuffer[256])
{
   bool error = false;
   /*
   numFrames = 0;
   for( int i=0; i<PDC_MAX_TEX; i++ )
   {
   if( textureNameList[i] && textureNameList[i][0] )
   {
   textureList[i] = TextureHandle( textureNameList[i], MeshTexture );
   if (!textureList[i].getName())
   {
   dSprintf(errorBuffer, 256, "Missing particle texture: %s", textureNameList[i]);
   error = true;
   }
   numFrames++;
   }
   }
   */
   return !error;
}

bool ParticleData::protectedSetSizes( void *object, const char *index, const char *data) 
{
   ParticleData *pData = static_cast<ParticleData*>( object );
   F32 val = dAtof(data);
   U32 i;

   if (!index)
      i = 0;
   else
      i = dAtoui(index);

   pData->sizes[i] = mClampF( val, 0.f, MaxParticleSize );

   return false;
}

DefineEngineMethod(ParticleData, reload, void, (),,
                   "Reloads this particle.\n"
                   "@tsexample\n"
                   "// Get the editor's current particle\n"
                   "%particle = PE_ParticleEditor.currParticle\n\n"
                   "// Change a particle value\n"
                   "%particle.setFieldValue( %propertyField, %value );\n\n"
                   "// Reload it\n"
                   "%particle.reload();\n"
                   "@endtsexample\n" )
{
   char errorBuffer[256];
   object->reload(errorBuffer);
}
