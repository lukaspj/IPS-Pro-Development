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

#include "gui\core\guiCanvas.h"
#include "console/consoleTypes.h"
#include "T3D\shapeBase.h"
#include "T3D\gameFunctions.h"
#include "gui\3d\guiTSControl.h"
#include "console\engineAPI.h"
#include "SpellSystemFunctions.h"
#include "SpellManager.h"
#include "TargetIndicators\TargetIndicatorManager.h"
#include "core\dnet.h"
#include "T3D\player.h"
#include "Projectiles\SpellProjectile.h"
#include "Projectiles\BezierProjectile.h"

#include "gfx/sim/debugDraw.h"

#include <typeinfo>

#pragma region RayCasts

bool SpellSystem::ScreenCenterRayCast(ShapeBase* scObj, F32 range, Point3F &pos)
{
   RayInfo rInfo;
   bool result = ScreenCenterRayCast(scObj, range, rInfo);
   pos = rInfo.point;
   return result;
}

bool SpellSystem::ScreenCenterRayCast(ShapeBase* scObj, F32 range, RayInfo &rInfo)
{
   MatrixF mat;
   //Eyepoint = position, eyeVector = forwardvector
   scObj->getRenderEyeTransform(&mat);

   VectorF eyeVector = mat.getForwardVector();
   Point3F start = mat.getPosition();
   VectorF rayDirection = eyeVector * range;
   VectorF end = start + rayDirection;
   return gServerContainer.castRay(start, end, TerrainObjectType | StaticObjectType, &rInfo);
}

bool SpellSystem::CursorRayCast(SceneObject *& obj)
{
   RayInfo rInfo;
   U8 result = SpellSystem::CursorRayCast(rInfo);
   obj = rInfo.object;
   return result;
}

bool SpellSystem::CursorRayCast(Point3F &pos)
{
   RayInfo rInfo;
   U8 result = SpellSystem::CursorRayCast(rInfo);
   pos = rInfo.point;
   return result;
}

U8 SpellSystem::CursorRayCast(RayInfo &rInfo, U32 TypeMask, bool Debug)
{
   Point2I windowPoint;
   GuiTSCtrl* playGUI;
   if(!Sim::findObject(GameTSCtrlName, playGUI))
      Con::errorf("SpellSystem::CursorRayCast - %s was not defined", GameTSCtrlName);
   GuiCanvas* Canvas;
   if(!Sim::findObject(CanvasName, Canvas))
      Con::errorf("SpellSystem::CursorRayCast - %s was not defined", CanvasName);
   windowPoint = Canvas->getCursorPos();
   if( Canvas->getPlatformWindow() )
      windowPoint = Canvas->getPlatformWindow()->screenToClient( windowPoint );
   MatrixF mat;
   Point3F vel;
   if ( GameGetCameraTransform(&mat, &vel) )
   {
      Point3F camPos;
      mat.getColumn(3,&camPos);
      Point3F screenPoint((F32)windowPoint.x, (F32)windowPoint.y, 1.0f);
      Point3F worldPoint;
      if (playGUI->unproject(screenPoint, &worldPoint)) {
         Point3F vec = worldPoint - camPos;
         Point3F start = camPos;
         vec.normalizeSafe();
         Point3F end = camPos + vec * 1000;
         bool hit = gClientContainer.castRay(start, end, TypeMask, &rInfo);
         if(Debug)
         {
            DebugDrawer::get()->drawLine(start, rInfo.point, ColorI(gRandGen.randI() % 255, gRandGen.randI() % 255, gRandGen.randI() % 255));
            DebugDrawer::get()->setLastTTL(5000);
         }
         if(hit)
            return 1;
         else
            return 0;
      }
      else{
         Con::errorf("SpellSystem::CursorRayCast - %s->unproject failed", GameTSCtrlName);
         return -1;
      }
   }
   else{
      Con::errorf("SpellSystem::CursorRayCast - GameGetCameraTransform failed");
      return -1;
   }
}

#pragma endregion

#pragma region Misc
void SpellSystem::setMovementSpeed(NetConnection* conn,F32 speed)
{
   Con::setFloatVariable("$movementSpeed", speed);
}

void SpellSystem::setTarget(NetConnection* conn, NetObject* Target)
{
   if(Target)
      conn->setDataField(StringTable->insert("SpellSystemTarget"), NULL, Target->getIdString());
   else
      conn->setDataField(StringTable->insert("SpellSystemTarget"), NULL, "");
}

const char* SpellSystem::getTarget(NetConnection* conn)
{
   if(!conn)
      conn = NetConnection::getConnectionToServer();
   if(conn)
      return conn->getDataField(StringTable->insert("SpellSystemTarget"), NULL);
   else
      return "";
}

#pragma endregion

#pragma region EngineFunctions

DefineEngineFunction(getObjectAtCursor,SceneObject*,(S32 TypeMask, bool Debug),(U32_MAX, false),"")
{
   RayInfo rInfo;
   U8 result = SpellSystem::CursorRayCast(rInfo, (U32)TypeMask, Debug);
   if(result > 0)
      return rInfo.object;
   else
      return NULL;
}

DefineEngineFunction(initializePointAndClickSystem,void,(void),,"")
{
   GuiTSCtrl* playGUI;
   if(!Sim::findObject(SpellSystem::GameTSCtrlName, playGUI))
      Con::errorf("SpellSystem::CursorRayCast - %s was not defined", SpellSystem::GameTSCtrlName);
   GuiCanvas* Canvas;
   if(!Sim::findObject(SpellSystem::CanvasName, Canvas))
      Con::errorf("SpellSystem::CursorRayCast - %s was not defined", SpellSystem::CanvasName);
   if(playGUI)
      playGUI->setField("noCursor", "0");
   if(Canvas)
      Canvas->setField("alwaysHandleMouseButtons", "1");
   Canvas->setCursorON(true);
}

DefineEngineMethod(GameConnection,SetSpeedModifier,void,(F32 amount),(0),"")
{
   IPlayer* player = dynamic_cast<IPlayer*>(object->getControlObject());
   if(player)
      player->setSpeedModifier(amount);
}

DefineEngineMethod(GameBase,ForceAnimation,bool,(bool make, const char* name, bool hold, bool fsp),(false, "", false, true),"")
{
   if(name == "" && make)
      return false;
   IPlayer* player = dynamic_cast<IPlayer*>(object);
   if(player){
      player->setImmobilized(make);
      if(!make)
         return true;;
      return player->setActionThread(name, hold, true, fsp);
   }
   else
      return false;
}

DefineEngineFunction(SetTarget, bool, (NetObject* obj, bool isServerObject),(NULL, false), "")
{
   NetConnection* conn = NetConnection::getConnectionToServer();
   if(!conn)
      return false;
   SpellSystem::setTarget(conn, obj);
   SpellSystem::SpellSystemNetEvent* evt = new SpellSystem::SpellSystemNetEvent();
   evt->actionMask |= SpellSystem::SpellSystemRemoteAction::SetTarget;
   evt->Target = obj;
   evt->TargetIsServerObject = isServerObject;
   return conn->postNetEvent(evt);

}

DefineEngineFunction(GetTarget, StringTableEntry, (NetConnection* conn),(NULL),"")
{
   if(conn)
      return SpellSystem::getTarget(conn);
   NetConnection* servConn = NetConnection::getConnectionToServer();
   if(servConn)
      return SpellSystem::getTarget(servConn);
   return "";
}

DefineEngineFunction(AttachSpellManager,bool,(SceneObject* Obj, NetConnection* Client),(NULL), "")
{
   SpellManager* manager = new SpellManager();
   if(!manager->registerObject())
   {
      delete manager;
      return false;
   }
   if(Obj)
   {
      Obj->setDataField(StringTable->insert("SpellManager", false), NULL, manager->getIdString());
      manager->mControllingObject = Obj;
      manager->mClient = Client;
      CooldownManager* CDM = new CooldownManager();
      if(!CDM->registerObject())
      {
         delete CDM;
         return false;
      }
      Client->objectLocalScopeAlways(CDM);
      manager->mCDManager = CDM;
      return true;
   }
   else{
      manager->deleteObject();
      return false;
   }
}

DefineEngineFunction(CreateTargetIndicator,bool,(SimDataBlock* data),(NULL), "")
{
   TargetIndicatorManager* manager = new TargetIndicatorManager();
   if(!manager->registerObject())
   {
      delete manager;
      return false;
   }
   if(data)
   {
      Con::setVariable("$SpellSystem::TargetIndicator", manager->getIdString());
      manager->SetIndicatorData(data);
      return true;
   }
   else{
      manager->deleteObject();
      return false;
   }
}

DefineEngineFunction(ThrowHomingProjectile, void, (ShapeBase* src, SceneObject* tgt, SpellProjectileData* dat, bool IgnoreCollisions), (NULL,NULL,NULL,true), "")
{
   Point3F start = src->getWorldBox().getCenter();
   Point3F end = tgt->getWorldBox().getCenter();
   Point3F velocity = end - start;
   velocity.normalize();
   velocity *= dat->muzzleVelocity;
   SpellProjectile* bullet = new SpellProjectile();
   bullet->setInitialPosition(start);
   bullet->setInitialVelocity(velocity);
   bullet->setSourceObject(src);
   bullet->setSourceSlot(0);
   bullet->setHoming(true);
   bullet->setOnlyCollideWithTarget(IgnoreCollisions);
   bullet->setInitialTransform(src->getTransform());
   bullet->setDataBlock(dat);
   bullet->setTarget(tgt);
   if(!bullet->registerObject())
   {
      delete bullet;
      return;
   }
   bullet->addToScene();
   // add to client side mission cleanup
   SimGroup *cleanup = dynamic_cast<SimGroup *>( Sim::findObject( "ClientMissionCleanup") );
   if( cleanup != NULL )
   {
      cleanup->addObject( bullet );
   }
   else
   {
      AssertFatal( false, "Error, could not find ClientMissionCleanup group" );
   }
}

DefineEngineFunction(ThrowHomingBezierProjectile, StringTableEntry, (ShapeBase* src, SceneObject* tgt, BezierProjectileData* dat, bool IgnoreCollisions, Point3F weights), (NULL,NULL,NULL,true,Point3F(0)), "")
{
   if(src == NULL || tgt == NULL || dat == NULL)
      return "";
   Point3F start = src->getWorldBox().getCenter();
   Point3F end = tgt->getWorldBox().getCenter();
   Point3F velocity = end - start;
   velocity.normalize();
   velocity *= dat->muzzleVelocity;
   BezierProjectile* bullet = new BezierProjectile();
   bullet->setInitialPosition(start);
   bullet->setInitialVelocity(velocity);
   bullet->setSourceObject(src);
   bullet->setSourceSlot(0);
   bullet->setHoming(true);
   bullet->setOnlyCollideWithTarget(IgnoreCollisions);
   bullet->setInitialTransform(src->getTransform());
   bullet->setWeights(weights);
   bullet->setTarget(tgt);
   bullet->setDataBlock(dat);
   if(!bullet->registerObject())
   {
      delete bullet;
      return "";
   }
   bullet->addToScene();
   // add to client side mission cleanup
   SimGroup *cleanup = dynamic_cast<SimGroup *>( Sim::findObject( "ClientMissionCleanup") );
   if( cleanup != NULL )
   {
      cleanup->addObject( bullet );
   }
   else
   {
      AssertFatal( false, "Error, could not find ClientMissionCleanup group" );
   }
   return bullet->getIdString();
}

DefineEngineFunction(ThrowBezierProjectile, StringTableEntry, (ShapeBase* src, Point3F end, BezierProjectileData* dat, bool IgnoreCollisions, Point3F weights), (NULL,Point3F(0),NULL,true,Point3F(0)), "")
{
   Point3F start = src->getWorldBox().getCenter();
   Point3F velocity = end - start;
   velocity.normalize();
   velocity *= dat->muzzleVelocity;
   BezierProjectile* bullet = new BezierProjectile();
   bullet->setInitialPosition(start);
   bullet->setInitialVelocity(velocity);
   bullet->setSourceObject(src);
   bullet->setSourceSlot(0);
   bullet->setHoming(false);
   bullet->setOnlyCollideWithTarget(false);
   bullet->setInitialTransform(src->getTransform());
   bullet->setWeights(weights);
   bullet->setTargetPosition(end);
   bullet->setDataBlock(dat);
   if(!bullet->registerObject())
   {
      delete bullet;
      return "";
   }
   bullet->addToScene();
   // add to client side mission cleanup
   SimGroup *cleanup = dynamic_cast<SimGroup *>( Sim::findObject( "ClientMissionCleanup") );
   if( cleanup != NULL )
   {
      cleanup->addObject( bullet );
   }
   else
   {
      AssertFatal( false, "Error, could not find ClientMissionCleanup group" );
   }
   return bullet->getIdString();
}

DefineEngineFunction(CastSpellOnServer, bool, (SpellData* dat), (NULL), "")
{
   NetConnection* conn = NetConnection::getConnectionToServer();
   if(!conn)
      return false;
   SpellSystem::SpellSystemNetEvent* evt = new SpellSystem::SpellSystemNetEvent();
   evt->actionMask |= SpellSystem::SpellSystemRemoteAction::CastSpell;
   evt->theSpell = dat;
   return conn->postNetEvent(evt);
}

DefineEngineFunction(DrawTimedDebugVector, void, (Point3F Start, Point3F End, U32 numFrames, ColorI Col), (ColorI(gRandGen.randI() % 255, gRandGen.randI() % 255, gRandGen.randI() % 255)), "")
{
   DebugDrawer::get()->drawLine(Start, End, Col);
   DebugDrawer::get()->setLastTTL(numFrames);
}

DefineEngineMethod(ShapeBase, applyHeal, void, (F32 amount),, "")
{
   F32 curAmount = object->getDamageLevel();
   if(amount > 0)
      object->setDamageLevel(amount < curAmount ? curAmount - amount : 0);
}

DefineEngineMethod(Player, applySlow, void, (F32 amount),, "")
{
   F32 curMultiplier = object->getSpeedModifier();
   object->setSpeedModifier(curMultiplier * (1-(amount / 100)));
}

DefineEngineMethod(Player, removeSlow, void, (F32 amount),, "")
{
   F32 curMultiplier = object->getSpeedModifier();
   object->setSpeedModifier(curMultiplier / (1-(amount / 100)));
}

DefineEngineMethod(Player, applyHaste, void, (F32 amount),, "")
{
   F32 curMultiplier = object->getSpeedModifier();
   object->setSpeedModifier(curMultiplier / (1-(amount / 100)));
}

DefineEngineMethod(Player, removeHaste, void, (F32 amount),, "")
{
   F32 curMultiplier = object->getSpeedModifier();
   object->setSpeedModifier(curMultiplier * (1-(amount / 100)));
}

typedef SpellSystem::SpellSystemNetEvent SSNetEvent;
IMPLEMENT_CO_NETEVENT_V1(SSNetEvent);

#pragma endregion