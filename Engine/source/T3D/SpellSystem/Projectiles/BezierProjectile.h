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

#ifndef BEZIER_PROJECTILE_H
#define BEZIER_PROJECTILE_H

#include "SpellProjectile.h"

class BezierProjectileData : public SpellProjectileData
{
   typedef SpellProjectileData Parent;

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
   BezierProjectileData();
   ~BezierProjectileData();

   //------- Fields -------
   Point3F bezier;


   DECLARE_CONOBJECT(BezierProjectileData);
};

class BezierProjectile : public SpellProjectile
{
   typedef SpellProjectile Parent;

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
   virtual bool onNewDataBlock(GameBaseData* dptr, bool reload);

   //--------------------------------------------
   // SpellProjectile
   //--------------------------------------------
   BezierProjectile();
   ~BezierProjectile();

   virtual void updatePosition(F32 dt, Point3F &oldPosition, Point3F &newPosition);

   //------- Fields -------
   Point3F TargetPosition;
   Point3F BezierWeights;

   void setWeights(Point3F weights) { BezierWeights = weights; };
   void setTargetPosition(Point3F pos) { TargetPosition = pos; };

   DECLARE_CONOBJECT(BezierProjectile);
};

#endif // BEZIER_PROJECTILE_H
