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

#include "SpellManager.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "console\engineAPI.h"
#include "PickMethods\ScreenCenterPick.h"
#include "PickMethods\AOEPick.h"
#include "PickMethods\SelfPick.h"
#include "PickMethods\TargetPick.h"
#include "SpellSystemFunctions.h"

IMPLEMENT_CONOBJECT(SpellManager);

IMPLEMENT_CALLBACK(SpellManager, onOOM, void, (SimDataBlock* spellID),(spellID), "");
IMPLEMENT_CALLBACK(SpellManager, onOutOfRange, void, (SimDataBlock* spellID),(spellID), "");
IMPLEMENT_CALLBACK(SpellManager, onNoTarget, void, (SimDataBlock* spellID),(spellID), "");
IMPLEMENT_CALLBACK(SpellManager, onCooldown, void, (SimDataBlock* spellID, U32 TimeLeft),(spellID, TimeLeft), "");
IMPLEMENT_CALLBACK(SpellManager, onTargetNotFound, void, (SimDataBlock* spellID),(spellID), "");
IMPLEMENT_CALLBACK(SpellManager, onAlreadyCastingSpell, void, (SimDataBlock* spellID),(spellID), "");
IMPLEMENT_CALLBACK(SpellManager, onTechnicalError, void, (SimDataBlock* spellID),(spellID), "");

//----------------------- SpellManager -------------------------
//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
SpellManager::SpellManager(){
   mTimeSpent = 0;
   mSpellState = SpellState::Idle;
   mCurSpellData = NULL;
   mCurSpell = NULL;
   mControllingObject = NULL;
   mClient = NULL;
   mCDManager = NULL;
   mCDManagerSynced = false;
}

//--------------------------------------------------------------
// BeginCast
//--------------------------------------------------------------
bool SpellManager::BeginCast(SpellData *SpellDat){
   if(mSpellState != Idle)
      return false;
   if(mCDManager->IsOnCooldown(SpellDat))
   {
      onCooldown_callback(SpellDat, mCDManager->GetCooldown(SpellDat));
      return false;
   }
   IPlayer* player = dynamic_cast<IPlayer*>(mControllingObject);
   if(player && player->getEnergyLeft() < SpellDat->mCost)
   {
      Con::errorf("SpellData::BeginCast Player was out of mana!");
      onOOM_callback(SpellDat);
      return false;
   }
   mSpellState = SpellState::Initializing;
   mTimeSpent = 0;
   mCurSpellData = SpellDat;

   mCurSpell = new Spell();
   if(!mCurSpell->registerObject())
   {
      delete mCurSpell;
      mCurSpell = 0;
      Con::errorf("SpellData::BeginCast Failed to register Spell object");
      onTechnicalError_callback(SpellDat);
      mSpellState = SpellState::Idle;
      return false;
   }
   mCurSpell->mSource = mControllingObject;
   mCurSpell->mClient = mClient;
   return true;
}

//--------------------------------------------------------------
// setTarget
//--------------------------------------------------------------
void SpellManager::setTarget(SpellTarget target)
{
   if(target.mTargetType == SpellTarget::None)
      breakCasting();
   else{
      mCurSpell->mTarget = target;
      mSpellState = SpellState::PreChannel;
      mCurSpellData->onChannelBegin_callback(mCurSpell->getId());
      mTimeSpent = 0;
   }
}

//--------------------------------------------------------------
// findTarget
//--------------------------------------------------------------
void SpellManager::findTarget()
{
   IPickMethod* pickType = NULL;
   switch (mCurSpellData->mPickType)
   {
   case SpellData::ScreenCenter:
      pickType = new ScreenCenterPick();
      break;
   case SpellData::AOE:
      pickType = new AOEPick();
      break;
   case SpellData::Self:
      pickType = new SelfPick();
      break;
   case SpellData::Target:
      pickType = new TargetPick();
      break;
   default:
      break;
   }

   if(!mControllingObject)
   {
      breakCasting();
      onTechnicalError_callback(mCurSpellData);
      Con::errorf("SpellManager::findTarget - mControllingObject is no longer valid");
      return;
   }

   if(mCurSpellData->mTargetType == SpellData::Object)
   {
      pickType->PickObject(this, mCurSpellData, mControllingObject, mCurSpellData->mTypeMask, (&SpellManager::PickCallback));
   }
   else if(mCurSpellData->mTargetType == SpellData::Point)
   {
      pickType->PickPosition(this, mCurSpellData, mControllingObject, mCurSpellData->mTypeMask, (&SpellManager::PickCallback));
   }
}

//--------------------------------------------------------------
// PickCallback (Point3F)
//--------------------------------------------------------------
void SpellManager::PickCallback(void* _this, Point3F pos, SpellPickError ReturnCode)
{
   SpellManager* __this = (SpellManager*)_this;

   if(__this->mSpellState != SpellState::AwaitingTarget)
   {
      Con::errorf("SpellManager::findTarget - Wasn't awaiting a target");
      __this->onTechnicalError_callback(__this->mCurSpellData);
      __this->breakCasting();
      return;
   }
   
   if(ReturnCode == SpellPickError::Success && __this->mCurSpellData->mRange < mAbs((pos - __this->mCurSpell->mSource->getPosition()).len()))
      ReturnCode = SpellPickError::OutOfRange;

   switch(ReturnCode)
   {
   case SpellPickError::Success:
      __this->mCurSpell->mTarget.mTargetType = SpellTarget::Point;
      __this->mCurSpell->mTarget.mTargetPos = pos;

      __this->mSpellState = SpellState::PreChannel;
      __this->mCurSpellData->onChannelBegin_callback(__this->mCurSpell->getId());
      __this->mTimeSpent = 0;
      break;
   case SpellPickError::NoTarget:
      Con::errorf("SpellManager::findTarget - Failed to acquire a target position");
      __this->onNoTarget_callback(__this->mCurSpellData);
      __this->breakCasting();
      break;
   case SpellPickError::OutOfRange:
      Con::errorf("SpellManager::findTarget - Out of range");
      __this->onOutOfRange_callback(__this->mCurSpellData);
      __this->breakCasting();
      break;
   case SpellPickError::NotFound:
      Con::errorf("SpellManager::findTarget - Didn't find any target position");
      __this->onTargetNotFound_callback(__this->mCurSpellData);
      __this->breakCasting();
      break;
   case SpellPickError::TechnicalError:
      Con::errorf("SpellManager::findTarget - A technical error occured");
      __this->onTechnicalError_callback(__this->mCurSpellData);
      __this->breakCasting();
      break;
   }
}

//--------------------------------------------------------------
// PickCallback (SceneObject)
//--------------------------------------------------------------
void SpellManager::PickCallback(void* _this, SceneObject* obj, SpellPickError ReturnCode)
{
   SpellManager* __this = (SpellManager*)_this;

   if(__this->mSpellState != SpellState::AwaitingTarget)
   {
      Con::errorf("SpellManager::findTarget - Already casting a spell");
      __this->onAlreadyCastingSpell_callback(__this->mCurSpellData);
      __this->breakCasting();
      return;
   }
   
   if(ReturnCode == SpellPickError::Success && __this->mCurSpellData->mRange < mAbs((obj->getPosition() - __this->mCurSpell->mSource->getPosition()).len()))
      ReturnCode = SpellPickError::OutOfRange;

   switch(ReturnCode)
   {
   case SpellPickError::Success:
      __this->mCurSpell->mTarget.mTargetType = SpellTarget::Object;
      __this->mCurSpell->mTarget.mTargetObj = (ShapeBase*)obj;

      __this->mSpellState = SpellState::PreChannel;
      __this->mCurSpellData->onChannelBegin_callback(__this->mCurSpell->getId());
      __this->mTimeSpent = 0;
      break;
   case SpellPickError::NoTarget:
      Con::errorf("SpellManager::findTarget - Failed to acquire a target");
      __this->onNoTarget_callback(__this->mCurSpellData);
      __this->breakCasting();
      break;
   case SpellPickError::OutOfRange:
      Con::errorf("SpellManager::findTarget - Out of range");
      __this->onOutOfRange_callback(__this->mCurSpellData);
      __this->breakCasting();
      break;
   case SpellPickError::NotFound:
      Con::errorf("SpellManager::findTarget - Didn't find any target");
      __this->onTargetNotFound_callback(__this->mCurSpellData);
      __this->breakCasting();
      break;
   case SpellPickError::TechnicalError:
      Con::errorf("SpellManager::findTarget - A technical error occured");
      __this->onTechnicalError_callback(__this->mCurSpellData);
      __this->breakCasting();
      break;
   }
}

//--------------------------------------------------------------
// breakCasting
//--------------------------------------------------------------
void SpellManager::breakCasting()
{
   mSpellState = SpellState::Idle;
   mCurSpellData = NULL;
   if(mCurSpell)
      mCurSpell->deleteObject();
   mCurSpell = NULL;
}

//----------------------- SimObject -------------------------
//--------------------------------------------------------------
// onAdd
//--------------------------------------------------------------
bool SpellManager::onAdd()
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

//--------------------------------------------------------------
// onRemove
//--------------------------------------------------------------
void SpellManager::onRemove()
{

   Parent::onRemove();
}

//--------------------------------------------------------------
// initPersistFields
//--------------------------------------------------------------
void SpellManager::initPersistFields()
{
   addProtectedField("Client", TYPEID<NetConnection>(), Offset(mClient, SpellManager),
      &_ReadOnly);
   Parent::initPersistFields();
}

bool SpellManager::_ReadOnly( void *object, const char *index, const char *data )  
{
   return false;  
}  

//----------------------- ITickable -------------------------
//--------------------------------------------------------------
// processTick
//--------------------------------------------------------------
void SpellManager::processTick()
{
   if(!mCDManagerSynced && mCDManager && mClient->getGhostIndex(mCDManager) != -1)
   {
      SpellSystem::SpellSystemNetEvent* evt = new SpellSystem::SpellSystemNetEvent();
      evt->actionMask |= SpellSystem::SpellSystemRemoteAction::CreateCDMGR;
      evt->CDManager = mCDManager;
      mClient->postNetEvent(evt);
      mCDManagerSynced = true;
   }
}

//--------------------------------------------------------------
// advanceTime
//--------------------------------------------------------------
void SpellManager::advanceTime( F32 TimeDelta )
{
   mCDManager->AdvanceTime(TimeDelta);
   if(mSpellState != SpellState::Idle)
      mTimeSpent += TimeDelta * 1000;
   if(mTimeSpent == 0)
      mTimeSpent = 0.00001f;
   U32 duration;
   if(mCurSpellData)
      duration = mCurSpellData->mChannelTimes[0] + mCurSpellData->mChannelTimes[1] + mCurSpellData->mChannelTimes[2];
   switch (mSpellState)
   {
   case SpellManager::Initializing:
      mCurSpellData->onInitializeCast_callback(mCurSpell->getId());
      mSpellState = SpellState::AwaitingTarget;
      findTarget();
      mTimeSpent = 0;
      mProgress = 0;
      break;
   case SpellManager::PreChannel:
      mProgress = duration / mTimeSpent;
      if(mTimeSpent >= mCurSpellData->mChannelTimes[0])
      {
         mCurSpellData->onChannel_callback(mCurSpell->getId());
         mSpellState = SpellState::Channel;
         mTimeSpent = 0;
      }
      break;
   case SpellManager::Channel:
      mProgress = duration / (mCurSpellData->mChannelTimes[0] + mTimeSpent);
      if(mTimeSpent >= mCurSpellData->mChannelTimes[1])
      {
         mCurSpellData->onChannelEnd_callback(mCurSpell->getId());
         mSpellState = SpellState::PostChannel;
         mTimeSpent = 0;
      }
      break;
   case SpellManager::PostChannel:
      mProgress = duration / (mCurSpellData->mChannelTimes[0] + mCurSpellData->mChannelTimes[1] + mTimeSpent);
      if(mTimeSpent >= mCurSpellData->mChannelTimes[2])
      {
         mCurSpellData->onPreCast_callback(mCurSpell->getId());
         mSpellState = SpellState::PreCast;
         mTimeSpent = 0;
      }
      break;
   case SpellManager::PreCast:
      if(mTimeSpent >= mCurSpellData->mCastTimes[0])
      {
         mCurSpellData->onCast_callback(mCurSpell->getId());
         mSpellState = SpellState::Cast;
         mTimeSpent = 0;
      }
      break;
   case SpellManager::Cast:
      if(mTimeSpent >= mCurSpellData->mCastTimes[1])
      {
         mCurSpellData->onPostCast_callback(mCurSpell->getId());
         mSpellState = SpellState::PostCast;
         mTimeSpent = 0;
         mCDManager->AddCooldown(mCurSpellData);
         IPlayer* player = dynamic_cast<IPlayer*>(mControllingObject);
         if(player)
            player->applyEnergyDrain(mCurSpellData->mCost);
      }
      break;
   case SpellManager::PostCast:
      if(mTimeSpent >= mCurSpellData->mCastTimes[2])
      {
         if(mCurSpell)
            mCurSpell->deleteObject();
         mCurSpell = NULL;
         mCurSpellData = NULL;
         mSpellState = SpellState::Idle;
      }
      break;
   default:
      break;
   }
}

//--------------------------------------------------------------
// interpolateTick
//--------------------------------------------------------------
void SpellManager::interpolateTick(F32 delta)
{
}

//--------------------------------------------------------------
// Console functions
//--------------------------------------------------------------
DefineEngineMethod(SpellManager, beginCast, bool, (SimDataBlock* data, NetConnection* client), (NULL, NULL), "Usage")
{
   if(!data)
      return false;
   SpellData* spellDat = NULL;
   if(data)
      spellDat = (SpellData*)data;
   if(spellDat)
      return object->BeginCast(spellDat);
   return true;
}

DefineEngineMethod(SpellManager, Attach, void, (SimObjectId ID), (NULL), "usage")
{
   SceneObject* obj = dynamic_cast<SceneObject*>(Sim::findObject(ID));
   if(obj)
   {
      obj->setDataField(StringTable->insert("SpellManager", false), NULL, object->getIdString());
      object->mControllingObject = obj;
   }
}

DefineEngineMethod(SpellManager, Interrupt, void, (), , "")
{
   object->breakCasting();
}