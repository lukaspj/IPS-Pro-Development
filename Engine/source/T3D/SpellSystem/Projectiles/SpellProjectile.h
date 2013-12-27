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

#ifndef SPELL_PROJECTILE_H
#define SPELL_PROJECTILE_H

#include "../../projectile.h"
#include "math\mTransform.h"
#include "T3D\shapeBase.h"

class SpellProjectileData : public ProjectileData
{
   typedef ProjectileData Parent;

public:
   //------- Enums -------
public:
   //--------------------------------------------
   // SimDataBlock
   //--------------------------------------------
   virtual bool onAdd();
   virtual void packData(BitStream*);
   virtual void unpackData(BitStream*);
   virtual bool preload(bool server, String &errorStr);
   static void initPersistFields();

   //--------------------------------------------
   // SpellProjectileData
   //--------------------------------------------
   SpellProjectileData();
   ~SpellProjectileData();

   //------- Fields -------


   DECLARE_CONOBJECT(SpellProjectileData);
};

class SpellProjectile : public Projectile
{
   typedef Projectile Parent;

   //------- Enums -------
   enum UpdateMasks{
      SpellProjectileMask = Parent::NextFreeMask << 0
   };
public:
   //--------------------------------------------
   // SimObject
   //--------------------------------------------
   static void initPersistFields();

   //--------------------------------------------
   // GameBase
   //--------------------------------------------
   virtual U32 packUpdate(NetConnection* conn, U32 mask, BitStream* stream);
   virtual void unpackUpdate(NetConnection* conn, BitStream* stream);


   //--------------------------------------------
   // ITickable
   //--------------------------------------------
   virtual void processTick(const Move *move);

   //--------------------------------------------
   // Projectile
   //--------------------------------------------
   virtual void simulate( F32 dt );

   //--------------------------------------------
   // SpellProjectile
   //--------------------------------------------
   SpellProjectile();
   ~SpellProjectile();

   //------- Fields -------
   bool mHoming;
   bool mOnlyCollideWithTarget;
   SceneObject* mTargetObject;
   TransformF mInitialTransform;

   void setSourceObject(ShapeBase* obj) { mSourceObject = obj; mSourceObjectId = obj->getId(); };
   void setSourceSlot(S32 slot) { mSourceObjectSlot = slot; };
   void setHoming(bool homing) { mHoming = homing; };
   void setOnlyCollideWithTarget(bool boolean) { mOnlyCollideWithTarget = boolean; };
   void setTarget(SceneObject* obj) { mTargetObject = obj; };
   void setInitialTransform(MatrixF mat) { mInitialTransform = mat; };

   virtual void updatePosition(F32 dt, Point3F &oldPosition, Point3F &newPosition);


   DECLARE_CONOBJECT(SpellProjectile);
};

#endif // SPELL_PROJECTILE_H
