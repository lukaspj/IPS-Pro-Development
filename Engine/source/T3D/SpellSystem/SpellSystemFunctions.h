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
#include "sim\netConnection.h"
#include "T3D\gameBase\gameConnection.h"
#include "core/stream/bitStream.h"
#include "SpellSystemInterfaces.h"
#include "SpellManager.h"
#include "console\simDatablock.h"

namespace SpellSystem
{
   const StringTableEntry GameTSCtrlName = "playGUI";
   const StringTableEntry CanvasName = "Canvas";

#pragma region RayCasts

   bool ScreenCenterRayCast(ShapeBase* scObj, F32 range, RayInfo &rInfo);

   bool ScreenCenterRayCast(ShapeBase* scObj, F32 range, Point3F &pos);

   bool CursorRayCast(SceneObject *& obj);

   bool CursorRayCast(Point3F &pos);

   U8 CursorRayCast(RayInfo &rInfo, U32 TypeMask = TerrainObjectType | StaticObjectType, bool Debug = false);

#pragma endregion

   void setMovementSpeed(NetConnection *conn, F32 speed);
   void setTarget(NetConnection *conn, NetObject* Target);
   const char* getTarget(NetConnection* conn = NULL);

   enum SpellSystemRemoteAction{
      Freeze = BIT(0),
      SetTarget = BIT(1),
      CastSpell = BIT(2),
      CreateCDMGR = BIT(3)
   };

   class SpellSystemNetEvent : public NetEvent
   {
      typedef NetEvent Parent;
   public:
      U32 actionMask;
      F32 FreezeAmount;
      NetObject* Target;
      bool TargetIsServerObject;
      SimDataBlock* theSpell;
      CooldownManager* CDManager;

      SpellSystemNetEvent()
      {
         actionMask = 0;
         FreezeAmount = 0;
         Target = NULL;
         theSpell = NULL;
         TargetIsServerObject = false;
         CDManager = NULL;
      }

      void pack(NetConnection* conn, BitStream *stream)  
      {
         bool server = stream->writeFlag(!conn->isConnectionToServer());
         if(stream->writeFlag(actionMask & Freeze))
            stream->writeInt(FreezeAmount * 1000, 16);
         if(stream->writeFlag(actionMask & SetTarget))
         {
            if(stream->writeFlag(Target))
            {
               if(server)
               {
                  S32 ghostID = conn->getGhostIndex(Target);
                  if(stream->writeFlag(ghostID != -1))
                  {
                     stream->writeRangedU32(U32(ghostID), 0, NetConnection::MaxGhostCount);
                  }
                  else
                     Target = NULL;
               }
               else
               {
                  if(stream->writeFlag(TargetIsServerObject))
                  {
                     stream->writeInt(Target->getId(),32);
                  }
                  else{
                     S32 ghostID = Target->getNetIndex();
                     ghostID = conn->getGhostIndex(Target);
                     if(stream->writeFlag(ghostID != -1))
                     {
                        stream->writeRangedU32(U32(ghostID), 0, NetConnection::MaxGhostCount);
                     }
                     else
                        Target = NULL;
                  }
               }
            }
            if(Target)
               conn->setDataField(StringTable->insert("SpellSystemTarget"), NULL, Target->getIdString());
            else
               conn->setDataField(StringTable->insert("SpellSystemTarget"), NULL, "");
         }
         if(stream->writeFlag(actionMask & CastSpell))
         {
            if(stream->writeFlag(theSpell))
            {
               stream->writeRangedU32(theSpell->getId(), DataBlockObjectIdFirst,  
                  DataBlockObjectIdLast);
            }
         }
         if(stream->writeFlag(actionMask & CreateCDMGR))
         {
            if(stream->writeFlag(CDManager))
            {
               if(server)
               {
                  S32 ghostID = conn->getGhostIndex(CDManager);
                  if(stream->writeFlag(ghostID != -1))
                  {
                     stream->writeRangedU32(U32(ghostID), 0, NetConnection::MaxGhostCount);
                  }
                  else
                     CDManager = NULL;
               }
               else
               {
                  if(stream->writeFlag(TargetIsServerObject))
                  {
                     stream->writeInt(CDManager->getId(),32);
                  }
                  else{
                     S32 ghostID = CDManager->getNetIndex();
                     ghostID = conn->getGhostIndex(CDManager);
                     if(stream->writeFlag(ghostID != -1))
                     {
                        stream->writeRangedU32(U32(ghostID), 0, NetConnection::MaxGhostCount);
                     }
                     else
                        CDManager = NULL;
                  }
               }
               if(CDManager)
                  conn->setDataField(StringTable->insert("SpellSystemCDManager"), NULL, CDManager->getIdString());
               else
                  conn->setDataField(StringTable->insert("SpellSystemCDManager"), NULL, "");
            }
         }
      }

      void write(NetConnection* conn, BitStream *stream)  
      {  
         pack(conn, stream);
      }

      void unpack(NetConnection* conn, BitStream *stream)  
      {
         bool server = !stream->readFlag();
         if(stream->readFlag())
         {
            FreezeAmount = stream->readInt(16) / 1000.0f;
            actionMask |= Freeze;
         }
         if(stream->readFlag())
         {
            if(stream->readFlag())
            {
               if(!server)
               {
                  if(stream->readFlag())
                  {
                     S32 TargetID = stream->readRangedU32(0, NetConnection::MaxGhostCount);
                     Target = conn->resolveObjectFromGhostIndex(TargetID);
                  }
                  else
                     Target = NULL;
               }
               else
               {
                  if(stream->readFlag())
                  {
                     S32 ID = stream->readInt(32);
                     if(!Sim::findObject(ID, Target))
                        Target = NULL;
                  }
                  else
                  {
                     if(stream->readFlag())
                     {
                        S32 TargetID = stream->readRangedU32(0, NetConnection::MaxGhostCount);
                        Target = conn->resolveObjectFromGhostIndex(TargetID);
                     }
                     else
                        Target = NULL;
                  }
               }
            }
            else
               Target = NULL;
            actionMask |= SetTarget;
         }
         if(stream->readFlag())
         {
            if(stream->readFlag())
            {
               SimDataBlock *dptr = 0;  
               SimObjectId id = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );  
               Sim::findObject( id, dptr );  
               if(dptr)  
                  theSpell = (SimDataBlock*)dptr;  
               actionMask |= CastSpell;
            }
         }
         if(stream->readFlag())
         {
            if(stream->readFlag())
            {
               if(!server)
               {
                  if(stream->readFlag())
                  {
                     S32 CDMGRID = stream->readRangedU32(0, NetConnection::MaxGhostCount);
                     CDManager = (CooldownManager*)conn->resolveGhost(CDMGRID);
                  }
                  else
                     CDManager = NULL;
               }
               else
               {
                  if(stream->readFlag())
                  {
                     S32 ID = stream->readInt(32);
                     if(!Sim::findObject(ID, CDManager))
                        CDManager = NULL;
                  }
                  else
                  {
                     if(stream->readFlag())
                     {
                        S32 CDMGRID = stream->readRangedU32(0, NetConnection::MaxGhostCount);
                        CDManager = (CooldownManager*)conn->resolveObjectFromGhostIndex(CDMGRID);
                     }
                     else
                        CDManager = NULL;
                  }
               }
            }
            else
               CDManager = NULL;
            actionMask |= CreateCDMGR;
         }
      }

      void process(NetConnection *conn)
      {
         if(actionMask & Freeze)
         {
            setMovementSpeed(conn, FreezeAmount);
         }
         if(actionMask & SetTarget)
         {
            setTarget(conn, Target);
         }
         if(actionMask & CastSpell)
         {
            SpellManager* mgr;
            GameBase* ctrlObj = ((GameConnection*)conn)->getControlObject();
            const char* managerName = ctrlObj->getDataField(StringTable->insert("SpellManager"), NULL);
            if(Sim::findObject(managerName, mgr))
               mgr->BeginCast((SpellData*)theSpell);
         }
         if(actionMask & CreateCDMGR)
         {
            if(CDManager)
               conn->setDataField(StringTable->insert("SpellSystemCDManager"), NULL, CDManager->getIdString());
            else
               conn->setDataField(StringTable->insert("SpellSystemCDManager"), NULL, "");
         }
         actionMask = 0;
      }

      DECLARE_CONOBJECT(SpellSystemNetEvent);  
   };
}