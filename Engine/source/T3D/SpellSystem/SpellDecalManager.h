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

#ifndef SPELL_DECAL_MANAGER_H
#define SPELL_DECAL_MANAGER_H

#include "T3D\decal\decalManager.h"

#include "sim\netConnection.h"
#include "console\consoleTypes.h"
#include "console\simDatablock.h"
#include "core\iTickable.h"
#include "T3D\shapeBase.h"
#include "core/stream/bitStream.h"
#include "PickMethods\PickMethod.h"

#include <string>

class SpellDecalManagerData : public SimDataBlock
{
   typedef SimDataBlock Parent;

public:
   //------- Enums -------
   enum PositionType{
      ScreenCenter,
      Cursor,
      None
   };
   //--------------------------------------------
   // SimDataBlock
   //--------------------------------------------
   bool onAdd();
   void packData(BitStream*);
   void unpackData(BitStream*);
   bool preload(bool server, String &errorStr);
   static void initPersistFields();

   //--------------------------------------------
   // SpellDecalManagerData
   //--------------------------------------------
   SpellDecalManagerData();
   ~SpellDecalManagerData();

   //------- Fields -------
   DecalData* mDecalData;
   PositionType mPositionType;

   DECLARE_CONOBJECT(SpellDecalManagerData);
};

class SpellDecalManager : public NetObject, public ITickable
{
   typedef NetObject Parent;
   enum MaskBits{
      Init = BIT(4),
   };
public:

   //--------------------------------------------
   // ITickable
   //--------------------------------------------
   virtual void interpolateTick( F32 delta );
   virtual void processTick();
   virtual void advanceTime( F32 timeDelta );

   //--------------------------------------------
   // SimObject
   //--------------------------------------------
   virtual bool onAdd();
   virtual void onRemove();
   static void initPersistFields();

   //--------------------------------------------
   // NetObject
   //--------------------------------------------
   virtual U32  packUpdate(NetConnection * conn, U32 mask, BitStream *stream);
   virtual void unpackUpdate(NetConnection * conn, BitStream *stream);

   //--------------------------------------------
   // SpellDecalManager
   //--------------------------------------------
   SpellDecalManager();
   void UpdateDecalPosition();
   void SpawnDecal();
   void BindMouse();
   void Finish(Point3F pos = Point3F(0));

   //--------------------------------------------
   // Fields
   //--------------------------------------------
   DecalInstance* mDecalInstance;
   ShapeBase* mDecalController;
   SpellDecalManagerData* mDataBlock;
   PickPositionCallback mCallback;
   void* caller;
   std::string mPreviousMakeCommand;
   std::string mPreviousBreakCommand;
   StringTableEntry ServerID;

   S8 SpawnWhenPossible;
   DECLARE_CONOBJECT(SpellDecalManager);
};

class SpellDecalManagerFinishEvent : public NetEvent
{
   typedef NetEvent Parent;
   SimObjectId DecalManagerID;
   Point3F position;
public:
   SpellDecalManagerFinishEvent()
   {
      DecalManagerID = 0;
      position = Point3F(0);
   }
   SpellDecalManagerFinishEvent(SimObjectId mgrID, Point3F _pos)
   {
      DecalManagerID = mgrID;
      position = _pos;
   }

   void pack(NetConnection* conn, BitStream *stream)  
   { 
      stream->write(DecalManagerID);
      stream->writeInt(position.x * 10000, 32); 
      stream->writeInt(position.y * 10000, 32);
      stream->writeInt(position.z * 10000, 32);
   }

   void write(NetConnection* conn, BitStream *stream)  
   {  
      pack(conn, stream);  
   }

   void unpack(NetConnection* conn, BitStream *stream)  
   {
      stream->read(&DecalManagerID);
      F32 x,y,z;
      x = stream->readInt(32) / 10000.0f;
      y = stream->readInt(32) / 10000.0f;
      z = stream->readInt(32) / 10000.0f;
      position = Point3F(x,y,z);
   }

   void process(NetConnection *conn)  
   {
      SpellDecalManager* SDM = (SpellDecalManager*)conn->resolveObjectFromGhostIndex(DecalManagerID);
      SDM->Finish(position);
   }

   DECLARE_CONOBJECT(SpellDecalManagerFinishEvent);  
};

#endif // SPELL_DECAL_MANAGER_H
