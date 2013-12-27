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

#include "BezierProjectile.h"
#include "T3D/shapeBase.h"
#include "core/stream/bitStream.h"
#include "sim/netConnection.h"

IMPLEMENT_CO_DATABLOCK_V1(BezierProjectileData);
IMPLEMENT_CO_NETOBJECT_V1(BezierProjectile);


//--------------------------------------------
// BezierProjectileData
//--------------------------------------------

//--------------------------------------------
// Constructor
//--------------------------------------------
BezierProjectileData::BezierProjectileData()
{
   bezier = Point3F(0);
}

//--------------------------------------------
// Destructor
//--------------------------------------------
BezierProjectileData::~BezierProjectileData()
{
}

//--------------------------------------------
// onAdd
//--------------------------------------------
bool BezierProjectileData::onAdd()
{
   if(!Parent::onAdd())
      return false;
   return true;
}

//--------------------------------------------
// preload
//--------------------------------------------
bool BezierProjectileData::preload(bool server, String &errorStr)
{
   if(!Parent::preload(server, errorStr))
      return false;
   return true;
}

//--------------------------------------------
// initPersistFields
//--------------------------------------------
void BezierProjectileData::initPersistFields()
{
   addGroup("BezierProjectileData");

   addField("bezierweights", TypePoint3F, Offset(bezier, BezierProjectileData), "");

   endGroup("BezierProjectileData");
   Parent::initPersistFields();
}

//--------------------------------------------
// packData
//--------------------------------------------
void BezierProjectileData::packData(BitStream* stream)
{
   Parent::packData(stream);
   stream->writeCompressedPoint(bezier);
}

//--------------------------------------------
// unpackData
//--------------------------------------------
void BezierProjectileData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
   stream->readCompressedPoint(&bezier);
}

//--------------------------------------------
// BezierProjectile
//--------------------------------------------
//--------------------------------------------
// Constructor
//--------------------------------------------
BezierProjectile::BezierProjectile()
{
   TargetPosition = Point3F(0);
   BezierWeights = Point3F::Max;
   Parent::SpellProjectile();
}

//--------------------------------------------
// Destructor
//--------------------------------------------
BezierProjectile::~BezierProjectile()
{
   Parent::~SpellProjectile();
}

//--------------------------------------------
// initPersistFields
//--------------------------------------------
void BezierProjectile::initPersistFields()
{
   addField("TargetPosition", TypePoint3F, Offset(TargetPosition, BezierProjectile), "");

   addField("BezierWeights", TypePoint3F, Offset(BezierWeights, BezierProjectile), "");

   Parent::initPersistFields();
}

bool BezierProjectile::onNewDataBlock( GameBaseData *dptr, bool reload )
{
   mDataBlock = dynamic_cast< BezierProjectileData* >( dptr );
   if( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
      return false;

   if(BezierWeights == Point3F::Max)
      BezierWeights = ((BezierProjectileData*)mDataBlock)->bezier;

   scriptOnNewDataBlock();
   return true;

}

//--------------------------------------------
// packUpdate
//--------------------------------------------
U32 BezierProjectile::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate(conn, mask, stream);
   if(stream->writeFlag(mask & InitialUpdateMask))
      stream->writeCompressedPoint(TargetPosition);
   stream->writeCompressedPoint(BezierWeights);
   return retMask;
}

//--------------------------------------------
// unpackUpdate
//--------------------------------------------
void BezierProjectile::unpackUpdate(NetConnection* conn, BitStream* stream)
{
   Parent::unpackUpdate(conn, stream);
   if(stream->readFlag())
      stream->readCompressedPoint(&TargetPosition);
   stream->readCompressedPoint(&BezierWeights);
}

//--------------------------------------------
// updatePosition
//--------------------------------------------
void BezierProjectile::updatePosition(F32 dt, Point3F &oldPosition, Point3F &newPosition)
{
   oldPosition = mCurrPosition;
   if ( mDataBlock->isBallistic )
      mCurrVelocity.z -= 9.81 * mDataBlock->gravityMod * dt;
   Point3F end;
   if(mHoming && mTargetObject)
      end = mTargetObject->getWorldBox().getCenter();
   else
      end = TargetPosition;
   F32 t = (F32)mCurrTick / (F32)mDataBlock->lifetime;
   MatrixF mat = mInitialTransform.getMatrix();
   Point3F halfWay = mInitialPosition+((end - mInitialPosition) / 2);
   Point3F bezier;
   mat.mulV(BezierWeights, &bezier);
   halfWay += bezier;
   Point3F p0, p1, p2;
   p0 = ((1-t)*(1-t))*mInitialPosition;
   p1 = 2*(1-t)*t*halfWay;
   p2 = (t*t*end);

   newPosition = p0+p1+p2;

   mCurrVelocity = (newPosition - oldPosition)/dt;
   newPosition = oldPosition + mCurrVelocity * dt;
}