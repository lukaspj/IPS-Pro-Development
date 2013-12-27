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

#include "DecalIndicator.h"

#include "core/stream/bitStream.h"

#include "math\mathIO.h"
#include "console\engineAPI.h"

IMPLEMENT_CO_DATABLOCK_V1(DecalIndicatorData);

//--------------------------------------------
// DecalIndicatorData
//--------------------------------------------

//--------------------------------------------
// Constructor
//--------------------------------------------
DecalIndicatorData::DecalIndicatorData()
{
   mDecalData = NULL;
}


//--------------------------------------------
// Destructor
//--------------------------------------------
DecalIndicatorData::~DecalIndicatorData()
{
}

//--------------------------------------------
// SimDataBlock
//--------------------------------------------
//--------------------------------------------
// initPersistFields
//--------------------------------------------
void DecalIndicatorData::initPersistFields()
{
   addField( "DecalData", TYPEID< DecalData >(), Offset( mDecalData, DecalIndicatorData ) );

   Parent::initPersistFields();
}

//--------------------------------------------
// onAdd
//--------------------------------------------
bool DecalIndicatorData::onAdd()
{
   if(!Parent::onAdd())
      return false;
   //Verify stuff
   return true;
}

//--------------------------------------------
// onRemove
//--------------------------------------------
void DecalIndicatorData::onRemove()
{
   Parent::onRemove();
}

//--------------------------------------------
// packData
//--------------------------------------------
void DecalIndicatorData::packData(BitStream* stream)
{
   if( stream->writeFlag( mDecalData ) )
   {
      stream->writeRangedU32( mDecalData->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
   }
   Parent::packData(stream);
}

//--------------------------------------------
// unpackData
//--------------------------------------------
void DecalIndicatorData::unpackData(BitStream* stream)
{
   if( stream->readFlag() )
   {
      S32 decalID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
      if (!Sim::findObject(decalID, mDecalData))
         Con::errorf(ConsoleLogEntry::General, "SpellData::unpackData - Invalid packet, bad datablockId(decalData): 0x%x", decalID);
   }
   Parent::unpackData(stream);
}

//--------------------------------------------
// preload
//--------------------------------------------
bool DecalIndicatorData::preload(bool server, String &errorStr)
{
   if(!Parent::preload(server, errorStr))
      return false;
   //Verify stuff
   return true;
}

TargetIndicator* DecalIndicatorData::getIndicator() { return new DecalIndicator(); }

//--------------------------------------------
// DecalIndicator
//--------------------------------------------

DecalIndicator::DecalIndicator()
{
   mInitialized = false;
   mDecalInstance = NULL;
}

DecalIndicator::~DecalIndicator()
{
   Dispose();
}

void DecalIndicator::Initialize(SpellTarget target)
{
   Dispose();
   U8 flags = DecalFlags::SpellSystemDecal;
   mDecalInstance = gDecalManager->addDecal(target.getTargetPosition(), Point3F(0,0,1), 0, mDataBlock->mDecalData, 1, 0, flags);
   // Add the decal to the instance vector.
   if(mDecalInstance)
   {
      mDecalInstance->mId = gDecalManager->mDecalInstanceVec.size();
      gDecalManager->mDecalInstanceVec.push_back( mDecalInstance );
   }
   mInitialized = true;
}

void DecalIndicator::Update(F32 Delta, SpellTarget target)
{
   if(mDecalInstance)
   {
      if(!mDecalInstance->mDataBlock)
      {
         gDecalManager->removeDecal(mDecalInstance);
         Initialize(target);
      }
      mDecalInstance->mPosition = target.mTargetObj->getRenderPosition();
      //gDecalManager->clipDecal( mDecalInstance, NULL, NULL );
   }
}

void DecalIndicator::Update(SpellTarget target)
{
   if(mDecalInstance)
   {
      if(!mDecalInstance->mDataBlock)
      {
         gDecalManager->removeDecal(mDecalInstance);
         Initialize(target);
      }
      mDecalInstance->mPosition = target.mTargetObj->getPosition();
      //gDecalManager->clipDecal( mDecalInstance, NULL, NULL );
   }
}

void DecalIndicator::Dispose()
{
   if(mDecalInstance && gDecalManager->getDecal(mDecalInstance->mId))
      gDecalManager->removeDecal(mDecalInstance);
   mDecalInstance = NULL;
   mInitialized = false;
}