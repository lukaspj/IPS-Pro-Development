// Copyright (C) 2013 Winterleaf Entertainment L,L,C.
// 
// THE SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND,
// INCLUDING WITHOUT LIMITATION THE WARRANTIES OF MERCHANT ABILITY, FITNESS
// FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT. THE ENTIRE RISK AS TO THE
// QUALITY AND PERFORMANCE OF THE SOFTWARE IS THE RESPONSIBILITY OF LICENSEE.
// SHOULD THE SOFTWARE PROVE DEFECTIVE IN ANY RESPECT, LICENSEE AND NOT LICEN-
// SOR OR ITS SUPPLIERS OR RESELLERS ASSUMES THE ENTIRE COST OF ANY SERVICE AND
// REPAIR. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS
// AGREEMENT. NO USE OF THE SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
// 
// The use of the WinterLeaf Entertainment LLC Improved Particle System Bundle (IPS Bundle)
// is governed by this license agreement.
// 
// RESTRICTIONS
// 
// (a) Licensee may not: (i) create any derivative works of IPS Bundle, including but not
// limited to translations, localizations, technology add-ons, or game making software
// other than Games without express permission from Winterleaf Entertainment; (ii) redistribute, 
// encumber , sell, rent, lease, sublicense, or otherwise
// transfer rights to IPS Bundle; or (iii) remove or alter any trademark, logo, copyright
// or other proprietary notices, legends, symbols or labels in IPS Bundle; or (iv) use
// the Software to develop or distribute any software that competes with the Software
// without WinterLeaf Entertainment's prior written consent; or (v) use the Software for
// any illegal purpose.
// (b) Licensee may not distribute the IPS Bundle in any manner.
// 
// LICENSEGRANT.
// This license allows companies of any size, government entities or individuals to cre-
// ate, sell, rent, lease, or otherwise profit commercially from, games using executables
// created from the source code of IPS Bundle
// 
// Please visit http://www.winterleafentertainment.com for more information about the project and latest updates.

#include "emitterParticle.h"
#include "console/consoleTypes.h"
#include "console/typeValidators.h"
#include "core/stream/bitStream.h"
#include "math/mathIO.h"
#include "console/engineAPI.h"
#include "core/resourceManager.h"
#include "sim/netConnection.h"

IMPLEMENT_CO_DATABLOCK_V1(EmitterParticleData);

ConsoleDocClass( EmitterParticleData,
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

static const float sgDefaultSpinSpeed = 1.f;
static const float sgDefaultSpinRandomMin = 0.f;
static const float sgDefaultSpinRandomMax = 0.f;

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
EmitterParticleData::EmitterParticleData()
{
   mEmitterData = NULL;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
EmitterParticleData::~EmitterParticleData()
{
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void EmitterParticleData::initPersistFields()
{
   addProtectedField( "times", TYPEID< F32 >(), Offset(times, EmitterParticleData), &protectedSetTimes, 
      &defaultProtectedGetFn, PDC_NUM_KEYS,
      "@brief Time keys used with the colors and sizes keyframes.\n\n"
      "Values are from 0.0 (particle creation) to 1.0 (end of lifespace)." );

   addField( "EmitterDatablock", TYPEID< ParticleEmitterData >(), Offset(mEmitterData, EmitterParticleData), "");

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// Pack data
//-----------------------------------------------------------------------------
void EmitterParticleData::packData(BitStream* stream)
{
   Parent::packData(stream);

   S32 i, count;

   // see how many frames there are:
   for(count = 0; count < PDC_NUM_KEYS - 1; count++)
      if(times[count] >= 1)
         break;

   count++;
   
   stream->writeInt(count, PDC_NUM_BITS);

   for( i=0; i<count; i++ )
   {
   }

   // Write out any of the updated editable properties
   if(stream->writeFlag(mEmitterData))
      stream->writeRangedU32(mEmitterData->getId(), DataBlockObjectIdFirst,
                     DataBlockObjectIdLast);
}

//-----------------------------------------------------------------------------
// Unpack data
//-----------------------------------------------------------------------------
void EmitterParticleData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   S32 i;
   S32 count = stream->readInt(PDC_NUM_BITS);
   for(i = 0;i < count; i++)
   {
   }

   if(stream->readFlag())
   {
      ParticleEmitterData *dptr = 0;
      SimObjectId id = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
      if(	Sim::findObject( id, dptr )	)
         mEmitterData = dptr;
   }
}

bool EmitterParticleData::protectedSetTimes( void *object, const char *index, const char *data) 
{
   EmitterParticleData *pData = static_cast<EmitterParticleData*>( object );
   F32 val = dAtof(data);
   U32 i;

   if (!index)
      i = 0;
   else
      i = dAtoui(index);

   pData->times[i] = mClampF( val, 0.f, 1.f );

   return false;
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool EmitterParticleData::onAdd()
{
   if (!Parent::onAdd())
      return false;

   return true;
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool EmitterParticleData::preload(bool server, String &errorStr)
{
   if (Parent::preload(server, errorStr) == false)
      return false;

   bool error = false;

   return !error;
}

bool EmitterParticleData::reload(char errorBuffer[256])
{
   bool error = Parent::reload(errorBuffer);
   return !error;
}

void EmitterParticleData::preloadVector(Vector<ParticleData*> _particleDataBlocks, U32 mask, ParticleEmitterData* emitterData)
{
}

DefineEngineMethod(EmitterParticleData, reload, void, (),,
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