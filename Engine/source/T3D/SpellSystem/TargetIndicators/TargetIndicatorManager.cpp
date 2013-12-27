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

#include "TargetIndicatorManager.h"
#include "console\engineAPI.h"
#include "../SpellSystemFunctions.h"

#include "T3D/gameBase/gameProcess.h"
#include "scene\sceneObject.h"

IMPLEMENT_CONOBJECT(TargetIndicatorManager);


//--------------------------------------------
// SimObject
//--------------------------------------------
bool TargetIndicatorManager::onAdd()
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

   F32 radius = 5.0;
   mObjBox.minExtents = Point3F(-radius, -radius, -radius);
   mObjBox.maxExtents = Point3F(radius, radius, radius);
   resetWorldBox();

   if( getSceneManager() == NULL )
   {
      removeFromProcessList();
      gClientSceneGraph->addObjectToScene(this);
      ClientProcessList::get()->addObject(this);
   }

   return true;
}

//--------------------------------------------
// onRemove
//--------------------------------------------
void TargetIndicatorManager::onRemove()
{
   if(mIndicator){
      mIndicator->Dispose();
      delete mIndicator;
   }
   mTarget.mTargetObj = NULL;
   removeFromScene();
   Parent::onRemove();
}

void TargetIndicatorManager::initPersistFields()
{
   Parent::initPersistFields();
}

//----------------------- ITickable ------------------------
void TargetIndicatorManager::interpolateTick( F32 delta ) 
{
   SceneObject* SSTarget;
   if(Sim::findObject(SpellSystem::getTarget(), SSTarget))
   {
      if(UseTargetingSystem && SSTarget && SSTarget != mTarget.mTargetObj)
      {
         mTarget.mTargetObj = SSTarget;
         if(mTarget.mTargetObj->mProcessTick)
            processAfter(mTarget.mTargetObj);
      }
      if(!mIndicator->isInitialized() && mTarget.mTargetObj != NULL)
         mIndicator->Initialize(mTarget);
   }
   else if(mIndicator->isInitialized()){
      mIndicator->Dispose();
      mTarget.mTargetObj = NULL;
      clearProcessAfter();
   }
   mIndicator->Update(delta, mTarget);
}
void TargetIndicatorManager::processTick(const Move* move) 
{
   SceneObject* SSTarget;
   if(Sim::findObject(SpellSystem::getTarget(), SSTarget))
   {
      if(UseTargetingSystem && SSTarget && SSTarget != mTarget.mTargetObj)
      {
         mTarget.mTargetObj = SSTarget;
         if(mTarget.mTargetObj->mProcessTick)
            processAfter(mTarget.mTargetObj);
      }
      if(!mIndicator->isInitialized() && mTarget.mTargetObj != NULL)
         mIndicator->Initialize(mTarget);
   }
   else if(mIndicator->isInitialized()){
      mIndicator->Dispose();
      mTarget.mTargetObj = NULL;
      clearProcessAfter();
   }
   mIndicator->Update(mTarget);
}
void TargetIndicatorManager::advanceTime( F32 timeDelta ) 
{
}

//---------------- TargetIndicatorManager ------------------
TargetIndicatorManager::TargetIndicatorManager()
{
   mNetFlags.set(IsGhost);
   mTarget.mTargetType = SpellTarget::TargetType::Object;
   mTargetIdString = "";
   mIndicator = NULL;
   mLastTickTime = Platform::getVirtualMilliseconds();
}

void TargetIndicatorManager::SetIndicatorData(SimDataBlock* data)
{
   if(!data)
      return;
   TargetIndicatorData* TID = dynamic_cast<TargetIndicatorData*>(data);
   mIndicator = TID->getIndicator();
   mIndicator->SetDataBlock(TID);
   mIndicator->Initialize(mTarget);
}

DefineEngineMethod(TargetIndicatorManager, SetIndicatorDataBlock, void, (SimDataBlock* dat), (NULL), "")
{
   object->SetIndicatorData(dat);
}

DefineEngineMethod(TargetIndicatorManager, SetTarget, void, (SceneObject* obj), (NULL), "")
{
   if(obj)
      object->mTarget.mTargetObj = obj;
}