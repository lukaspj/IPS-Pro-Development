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

#include "Spell.h"
#include "console\engineAPI.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(SpellData);
IMPLEMENT_CONOBJECT(Spell);

// See full description in the new CHM manual
ConsoleDocClass( SpellData,
                "@brief Base class for almost all objects involved in the simulation.\n\n"

                "@ingroup Console\n"
                );

// See full description in the new CHM manual
ConsoleDocClass( Spell,
                "@brief Base class for almost all objects involved in the simulation.\n\n"

                "@ingroup Console\n"
                );



IMPLEMENT_CALLBACK(SpellData, onInitializeCast, void, (SimObjectId spellID), (spellID), "");

IMPLEMENT_CALLBACK(SpellData, onChannelBegin, void, (SimObjectId spellID), (spellID), "");
IMPLEMENT_CALLBACK(SpellData, onChannel, void, (SimObjectId spellID), (spellID), "");
IMPLEMENT_CALLBACK(SpellData, onChannelEnd, void, (SimObjectId spellID), (spellID), "");

IMPLEMENT_CALLBACK(SpellData, onPreCast, void, (SimObjectId spellID), (spellID), "");
IMPLEMENT_CALLBACK(SpellData, onCast, void, (SimObjectId spellID), (spellID), "");
IMPLEMENT_CALLBACK(SpellData, onPostCast, void, (SimObjectId spellID), (spellID), "");

typedef SpellData::PickType PickTypes;
DefineEnumType( PickTypes );

ImplementEnumType( PickTypes,
                  "How the spells should be cast.\n"
                  "@ingroup SpellSystem\n\n")
{ PickTypes::ScreenCenter,		"ScreenCenter",			"Casts the spell in the middle of the screen.\n" },
{ PickTypes::AOE,				"AOE",					"Spawns a spellindicator and casts the spell on next input (ala AOE's in WoW).\n" },
{ PickTypes::Self,				"Self",					"Casts the spell at the casters itself.\n" },
{ PickTypes::Target,			"Target",				"Casts the spell on the SpellSystem Target.\n" },
EndImplementEnumType;

typedef SpellData::TargetType TargetTypes;
DefineEnumType( TargetTypes );

ImplementEnumType( TargetTypes,
                  "How the spells should be cast.\n"
                  "@ingroup SpellSystem\n\n")
{ TargetTypes::Object,		"Object",			"\n" },
{ TargetTypes::Point,		"Point",			"\n" },
EndImplementEnumType;

//---------------------------------------------------------------
// Constructor
//---------------------------------------------------------------
SpellData::SpellData()
{
   mTypeMask = 0;
   mPickType = PickTypes::ERROR;
   mTargetType = TargetTypes::None;
   mDecalManagerData = NULL;
   mRange = 0.0;
   for(int i = 0; i < 3; i++)
   {
      mChannelTimes[i] = 0;
      mCastTimes[i] = 0;
   }
   mCooldown = 0;
   mCost = 0;
}

SpellData::~SpellData()
{
}

//----------------------- SpellData -------------------------


//----------------------- SimDataBlock -------------------------
//--------------------------------------------------------------
// InitPersistFields
//--------------------------------------------------------------
void SpellData::initPersistFields()
{
   addField("CastType", TYPEID<PickTypes>(), Offset(mPickType, SpellData), "");

   addField("TargetType", TYPEID<TargetTypes>(), Offset(mTargetType, SpellData), "");

   addField("TypeMask", TYPEID<S32>(), Offset(mTypeMask, SpellData), "");

   addField("ChannelTimesMS", TypeS32, Offset(mChannelTimes, SpellData), 3, "");

   addField("CastTimesMS", TypeS32, Offset(mCastTimes, SpellData), 3, "");

   addField("SpellDecalManager", TYPEID<SpellDecalManagerData>(), Offset(mDecalManagerData, SpellData), "");

   addField("Range", TypeF32, Offset(mRange, SpellData), 3, "");

   addField("CooldownMS", TypeS32, Offset(mCooldown, SpellData), "");

   addField("Cost", TypeF32, Offset(mCost, SpellData), "");

   addField("Logo", TypeStringFilename, Offset(mLogo, SpellData), "");

   Parent::initPersistFields();
}

//---------------------------------------------------------------
// OnAdd
//---------------------------------------------------------------
bool SpellData::onAdd()
{
   if( !Parent::onAdd() )
      return false;
   return true;
}

//---------------------------------------------------------------
// PackData
//--------------------------------------------------------------
void SpellData::packData(BitStream* stream)
{
   for(int i = 0; i < 3; i++)
   {
      stream->writeInt(mChannelTimes[i], 15);
      stream->writeInt(mCastTimes[i], 15);
   }
   stream->writeInt(mPickType, 3);
   stream->writeInt(mTargetType, 3);
   stream->writeInt(mTypeMask, 32);
   stream->writeInt(mRange * 1000, 16);
   if( stream->writeFlag( mDecalManagerData && mDecalManagerData->isProperlyAdded() ) )
   {
      stream->writeRangedU32( mDecalManagerData->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
   }
   Parent::packData(stream);
}

//---------------------------------------------------------------
// UnpackData
//--------------------------------------------------------------
void SpellData::unpackData(BitStream* stream)
{
   for(int i = 0; i < 3; i++)
   {
      mChannelTimes[i] = stream->readInt(15);
      mCastTimes[i] = stream->readInt(15);
   }
   mPickType = (PickTypes)stream->readInt(3);
   mTargetType = (TargetTypes)stream->readInt(3);
   mTypeMask = stream->readInt(32);
   mRange = stream->readInt(16) / 1000.0f;
   if( stream->readFlag() )
   {
      S32 managerID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
      if (!Sim::findObject(managerID, mDecalManagerData))
         Con::errorf(ConsoleLogEntry::General, "SpellData::unpackData - Invalid packet, bad datablockId(decalData): 0x%x", managerID);
   }
   Parent::unpackData(stream);
}

//--------------------------------------------------------------
// Preload
//--------------------------------------------------------------
bool SpellData::preload(bool server, String &errorStr)
{
   if( Parent::preload(server, errorStr) == false )
      return false;
   // Verify variables

   return true;
}

//--------------------------------------------------------------
// Console functions
//--------------------------------------------------------------
DefineEngineMethod(SpellData, beginCast, void, (SimObjectId ID), (U32_MAX), "Usage")
{
   if(ID == U32_MAX)
      return;
}


//----------------------- Spell -------------------------
//--------------------------------------------------------------
// onAdd
//--------------------------------------------------------------
bool Spell::onAdd()
{
   if( !Parent::onAdd() )
      return false;
   return true;
}

Spell::Spell()
{
   mTarget = SpellTarget();
   mDataBlock = NULL;
   mSource = NULL;
   mClient = NULL;
}

Spell::~Spell()
{
}

void Spell::initPersistFields()
{

}

DefineEngineMethod(Spell, getTargetPosition, Point3F, () , , "")
{
   return object->mTarget.getTargetPosition();
}

DefineEngineMethod(Spell, getTarget, S32, (),,"")
{
   if(object->mTarget.mTargetObj)
      return object->mTarget.mTargetObj->getId();
   else return 0;
}

DefineEngineMethod(Spell, getSource, S32, (),,"")
{
   if(object->mSource)
      return object->mSource->getId();
   else return 0;
}

DefineEngineMethod(Spell, getClient, S32, (),,"")
{
   if(object->mClient)
      return object->mClient->getId();
   else return 0;
}

DefineEngineMethod(Spell, getDataBlock, S32, (),,"")
{
   if(object->mDataBlock)
      return object->mDataBlock->getId();
   else return 0;
}