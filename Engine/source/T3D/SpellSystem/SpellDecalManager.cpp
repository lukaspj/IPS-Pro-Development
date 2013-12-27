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

#include "SpellDecalManager.h"

#include "sim\netObject.h"
#include "PickMethods\ScreenCenterPick.h"
#include "T3D/gameBase/processList.h"
#include "sim\actionMap.h"
#include "console/consoleTypes.h"
#include "console\engineAPI.h"
#include "core/strings/stringUnit.h"
#include "SpellSystemFunctions.h"

IMPLEMENT_CO_NETOBJECT_V1(SpellDecalManager);
IMPLEMENT_CO_DATABLOCK_V1(SpellDecalManagerData);
IMPLEMENT_CO_NETEVENT_V1(SpellDecalManagerFinishEvent);

//--------------------------------------------
// SpellDecalManagerData
//--------------------------------------------

typedef SpellDecalManagerData::PositionType PositionTypes;
DefineEnumType( PositionTypes );

ImplementEnumType( PositionTypes,
                  "How the spells should be cast.\n"
                  "@ingroup SpellSystem\n\n")
{ PositionTypes::ScreenCenter,		"ScreenCenter",			"\n" },
{ PositionTypes::Cursor,			"Cursor",				"\n" },
EndImplementEnumType;

//--------------------------------------------
// Constructor
//--------------------------------------------
SpellDecalManagerData::SpellDecalManagerData()
{
   mDecalData = NULL;
   mPositionType = PositionType::None;
}


//--------------------------------------------
// Destructor
//--------------------------------------------
SpellDecalManagerData::~SpellDecalManagerData()
{
}

//--------------------------------------------
// SimDataBlock
//--------------------------------------------
//--------------------------------------------
// initPersistFields
//--------------------------------------------
void SpellDecalManagerData::initPersistFields()
{
   addField( "DecalData", TYPEID< DecalData >(), Offset( mDecalData, SpellDecalManagerData ) );

   addField( "PositionType", TYPEID< PositionTypes >(), Offset(mPositionType, SpellDecalManagerData), "");

   Parent::initPersistFields();
}

//--------------------------------------------
// onAdd
//--------------------------------------------
bool SpellDecalManagerData::onAdd()
{
   if(!Parent::onAdd())
      return false;
   //Verify stuff
   return true;
}

//--------------------------------------------
// packData
//--------------------------------------------
void SpellDecalManagerData::packData(BitStream* stream)
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
void SpellDecalManagerData::unpackData(BitStream* stream)
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
bool SpellDecalManagerData::preload(bool server, String &errorStr)
{
   if(!Parent::preload(server, errorStr))
      return false;
   //Verify stuff
   return true;
}

//--------------------------------------------
// SpellDecalManager
//--------------------------------------------
//--------------------------------------------
// Constructor
//--------------------------------------------
SpellDecalManager::SpellDecalManager()
{
   mNetFlags.set(Ghostable);
   mDecalInstance = NULL;
   mDecalController = NULL;
   caller = NULL;
   ServerID = 0;
   SpawnWhenPossible = 0;
}

//--------------------------------------------
// UpdateDecalPosition
//--------------------------------------------
void SpellDecalManager::UpdateDecalPosition()
{
   if(mDataBlock && mDecalInstance && mDataBlock->mPositionType != SpellDecalManagerData::PositionType::None)
   {
      if(mDataBlock->mPositionType == SpellDecalManagerData::PositionType::ScreenCenter && mDecalController)
      {
         Point3F pos;
         if(SpellSystem::ScreenCenterRayCast(mDecalController, 100, pos))
         {
            mDecalInstance->mPosition = pos;
         }
      }
      if(mDataBlock->mPositionType == SpellDecalManagerData::PositionType::Cursor)
      {
         Point3F pos;
         if(SpellSystem::CursorRayCast(pos))
         {
            mDecalInstance->mPosition = pos;
         }
      }
   }
}

//--------------------------------------------
// UpdateDecalPosition
//--------------------------------------------
void SpellDecalManager::SpawnDecal()
{
   if(isServerObject())
      setMaskBits(MaskBits::Init);
   else if(isClientObject() && mDataBlock)
   {
      U8 flags = DecalFlags::SpellSystemDecal;
      mDecalInstance = gDecalManager->addDecal(Point3F(0), Point3F(0,0,1), 0, mDataBlock->mDecalData, 1, -1, flags);
      // Add the decal to the instance vector.
      if(mDecalInstance)
      {
         mDecalInstance->mId = gDecalManager->mDecalInstanceVec.size();
         gDecalManager->mDecalInstanceVec.push_back( mDecalInstance );
      }
      UpdateDecalPosition();
      BindMouse();
   }
}

//--------------------------------------------
// BindMouse
//--------------------------------------------
void SpellDecalManager::BindMouse()
{
   if(isClientObject())
   {
      SimSet* pActionMapSet = Sim::getActiveActionMapSet();
      ActionMap* currentMap = dynamic_cast< ActionMap* >( pActionMapSet->last() );
      const char* prevCommand = currentMap->getCommand("mouse","button0");
      mPreviousMakeCommand = std::string(StringUnit::getUnit( prevCommand, 0, "\t\n" ));
      mPreviousBreakCommand = std::string(StringUnit::getUnit( prevCommand, 1, "\t\n" ));

      currentMap->processBindCmd("mouse", "button0", "IPSSDMUnbindMouse();", "");

      Con::setVariable("$IPS::SpellDecalManager", getIdString());
   }
}

//--------------------------------------------
// Finish
//--------------------------------------------
void SpellDecalManager::Finish(Point3F pos)
{
   if(isClientObject())
   {
      NetConnection *conn = NetConnection::getConnectionToServer();
      if(conn)
         conn->postNetEvent(new SpellDecalManagerFinishEvent(conn->getGhostIndex(this), mDecalInstance->mPosition));
      SimSet* pActionMapSet = Sim::getActiveActionMapSet();
      ActionMap* currentMap = dynamic_cast< ActionMap* >( pActionMapSet->last() );
      currentMap->processBindCmd("mouse", "button0", mPreviousMakeCommand.c_str(), mPreviousBreakCommand.c_str());
      if(mDecalInstance)
         gDecalManager->removeDecal(mDecalInstance);
      mDecalInstance = NULL;
   }
   if(isServerObject())
   {
      mCallback(caller, pos, SpellPickError::Success);
      deleteObject();
   }
}

//--------------------------------------------
// SimObject
//--------------------------------------------
//--------------------------------------------
// initPersistFields
//--------------------------------------------
void SpellDecalManager::initPersistFields()
{
   //addField("DataBlock", TYPEID<SpellDecalManagerData>(), Offset(mDataBlock, SpellDecalManager));

   Parent::initPersistFields();
}

//--------------------------------------------
// onAdd
//--------------------------------------------
bool SpellDecalManager::onAdd()
{
   if(!Parent::onAdd())
      return false;

   // add to client side mission cleanup
   SimGroup *cleanup = dynamic_cast<SimGroup *>( Sim::findObject( "ClientMissionCleanup") );
   if( cleanup != NULL )
   {
      cleanup->addObject( this );
   }
   else
   {
      AssertFatal( false, "Error, could not find ClientMissionCleanup group" );
      return false;
   }

   return true;
}

//--------------------------------------------
// onRemove
//--------------------------------------------
void SpellDecalManager::onRemove()
{
   if(mDecalInstance && gDecalManager->getDecal(mDecalInstance->mId))
      gDecalManager->removeDecal(mDecalInstance);
   mDecalInstance = NULL;
   Parent::onRemove();
}

//--------------------------------------------
// ITickable
//--------------------------------------------
//--------------------------------------------
// interpolateTick
//--------------------------------------------
void SpellDecalManager::interpolateTick( F32 delta )
{
   if(isProperlyAdded() && isClientObject())
   {
      if(mDecalInstance && mDecalInstance->mDataBlock)
      {
         UpdateDecalPosition();
         gDecalManager->clipDecal( mDecalInstance, NULL, NULL );
      }
   }
}

//--------------------------------------------
// processTick
//--------------------------------------------
void SpellDecalManager::processTick()
{
   if(isProperlyAdded() && isClientObject())
   {
      if(mDecalInstance && mDecalInstance->mDataBlock)
      {
         UpdateDecalPosition();
         gDecalManager->clipDecal( mDecalInstance, NULL, NULL );
      }
   }
}

//--------------------------------------------
// advanceTime
//--------------------------------------------
void SpellDecalManager::advanceTime( F32 timeDelta )
{
   if(isProperlyAdded() && isClientObject())
   {
      if(SpawnWhenPossible == 1)
      {
         SpawnDecal();
         SpawnWhenPossible = -1;
      }
   }
}

//--------------------------------------------
// packUpdate
//--------------------------------------------
U32 SpellDecalManager::packUpdate(NetConnection * conn, U32 mask, BitStream *stream)
{
   U32 retMask = Parent::packUpdate(conn, mask, stream);
   if( stream->writeFlag( mDataBlock && mDataBlock->isProperlyAdded() ) )
   {
      stream->writeRangedU32( mDataBlock->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
   }
   stream->write(mDecalController->getId());
   stream->writeFlag(mask & Init);
   return retMask;
}

//--------------------------------------------
// unpackUpdate
//--------------------------------------------
void SpellDecalManager::unpackUpdate(NetConnection * conn, BitStream *stream)
{
   Parent::unpackUpdate(conn, stream);
   if( stream->readFlag() )
   {
      S32 managerID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
      if (!Sim::findObject(managerID, mDataBlock))
         Con::errorf(ConsoleLogEntry::General, "SpellDecalManager::unpackData - Invalid packet, bad datablockId(SpellDecalManagerData): 0x%x", managerID);
   }

   S32 controllerID;
   stream->read(&controllerID);
   if(!Sim::findObject(controllerID, mDecalController))
      Con::errorf(ConsoleLogEntry::General, "SpellDecalManager::unpackData - Invalid packet, bad shapeId(ShapeBase): 0x%x", controllerID);

   if(stream->readFlag() && SpawnWhenPossible == 0 && isClientObject())
      SpawnWhenPossible = 1;
   clearMaskBits(U32_MAX);
}

DefineEngineFunction(IPSSDMUnbindMouse, void, (),,"")
{
   SpellDecalManager* SDM;
   if(!Sim::findObject(Con::getIntVariable("$IPS::SpellDecalManager"),SDM))
      return; // Do something fail safe here
   SDM->Finish();
}