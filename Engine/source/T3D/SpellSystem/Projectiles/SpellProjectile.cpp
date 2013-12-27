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

#include "SpellProjectile.h"
#include "T3D/shapeBase.h"
#include "T3D/physics/physicsWorld.h"
#include "core/stream/bitStream.h"
#include "sim\netConnection.h"

#include "math\mathIO.h"

IMPLEMENT_CO_DATABLOCK_V1(SpellProjectileData);
IMPLEMENT_CO_NETOBJECT_V1(SpellProjectile);


//--------------------------------------------
// SpellProjectileData
//--------------------------------------------

//--------------------------------------------
// Constructor
//--------------------------------------------
SpellProjectileData::SpellProjectileData()
{
}

//--------------------------------------------
// Destructor
//--------------------------------------------
SpellProjectileData::~SpellProjectileData()
{
}

//--------------------------------------------
// onAdd
//--------------------------------------------
bool SpellProjectileData::onAdd()
{
   if(!Parent::onAdd())
      return false;
   return true;
}

//--------------------------------------------
// preload
//--------------------------------------------
bool SpellProjectileData::preload(bool server, String &errorStr)
{
   if(!Parent::preload(server, errorStr))
      return false;
   return true;
}

//--------------------------------------------
// initPersistFields
//--------------------------------------------
void SpellProjectileData::initPersistFields()
{
   addGroup("SpellProjectileData");



   endGroup("SpellProjectileData");
   Parent::initPersistFields();
}

//--------------------------------------------
// packData
//--------------------------------------------
void SpellProjectileData::packData(BitStream* stream)
{
   Parent::packData(stream);
}

//--------------------------------------------
// unpackData
//--------------------------------------------
void SpellProjectileData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
}

//--------------------------------------------
// SpellProjectile
//--------------------------------------------
//--------------------------------------------
// SpellProjectile
//--------------------------------------------
SpellProjectile::SpellProjectile()
{
   mTargetObject = NULL;
   mHoming = false;
   mOnlyCollideWithTarget = false;
   Parent::Projectile();
}

//--------------------------------------------
// SpellProjectile
//--------------------------------------------
SpellProjectile::~SpellProjectile()
{
   Parent::~Projectile();
}

//--------------------------------------------
// initPersistFields
//--------------------------------------------
void SpellProjectile::initPersistFields()
{
   addGroup("SpellProjectile");

   addField("TargetObject", TYPEID<SceneObject>(), Offset(mTargetObject, SpellProjectile), "");

   addField("OnlyCollideWithTarget", TypeBool, Offset(mOnlyCollideWithTarget, SpellProjectile), "");

   addField("Homing", TypeBool, Offset(mHoming, SpellProjectile), "");

   addField("InitialTransform", TypeTransformF, Offset(mInitialTransform, SpellProjectile), "");

   endGroup("SpellProjectile");

   Parent::initPersistFields();
}

//--------------------------------------------
// packUpdate
//--------------------------------------------
U32 SpellProjectile::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate(conn, mask, stream);
   if(stream->writeFlag(mask & SpellProjectileMask || mask & InitialUpdateMask))
   {
      S32 ghostID = -1;
      if(mTargetObject)
         ghostID = conn->getGhostIndex(mTargetObject);
      if(stream->writeFlag(ghostID != -1))
         stream->writeRangedU32(U32(ghostID), 0, NetConnection::MaxGhostCount);
      else
         mTargetObject = NULL;
      stream->writeFlag(mOnlyCollideWithTarget);
      stream->writeFlag(mHoming);
      stream->writeCompressedPoint(mInitialPosition);
      mathWrite(*stream, mInitialTransform.getMatrix());
   }
   return retMask;
}

//--------------------------------------------
// unpackUpdate
//--------------------------------------------
void SpellProjectile::unpackUpdate(NetConnection* conn, BitStream* stream)
{
   Parent::unpackUpdate(conn, stream);
   if(stream->readFlag())
   {
      if(stream->readFlag())
      {
         S32 TargetID = stream->readRangedU32(0, NetConnection::MaxGhostCount);
         mTargetObject = (SceneObject*)conn->resolveGhost(TargetID);
      }
      else
         mTargetObject = NULL;
      mOnlyCollideWithTarget = stream->readFlag();
      mHoming = stream->readFlag();
      stream->readCompressedPoint(&mInitialPosition);
      MatrixF mat;
      mathRead(*stream, &mat);
      mInitialTransform.set(mat);
   }
}

//--------------------------------------------
// processTick
//--------------------------------------------
void SpellProjectile::processTick( const Move *move )
{
   GameBase::processTick( move );
   mCurrTick++;

   simulate( TickSec );
}

//--------------------------------------------
// simulate
//--------------------------------------------
void SpellProjectile::simulate( F32 dt )
{
   if ( isServerObject() && mCurrTick >= mDataBlock->lifetime )
   {
      if(mTargetObject && mOnlyCollideWithTarget && mHoming)
      {
         VectorF normal = -getVelocity();
         normal.normalize();
         onCollision( getPosition(), normal, mTargetObject );
         explode( getPosition(), normal, mTargetObject->getTypeMask() );
      }
      deleteObject();
      return;
   }

   if ( mHasExploded )
      return;

   // ... otherwise, we have to do some simulation work.
   RayInfo rInfo;
   Point3F oldPosition;
   Point3F newPosition;

   updatePosition(dt, oldPosition, newPosition);

   if(!(mTargetObject && mOnlyCollideWithTarget && mHoming))
   {
      // disable the source objects collision reponse for a short time while we
      // determine if the projectile is capable of moving from the old position
      // to the new position, otherwise we'll hit ourself
      bool disableSourceObjCollision = (mSourceObject.isValid() && mCurrTick <= SourceIdTimeoutTicks);
      if ( disableSourceObjCollision )
         mSourceObject->disableCollision();
      disableCollision();

      // Determine if the projectile is going to hit any object between the previous
      // position and the new position. This code is executed both on the server
      // and on the client (for prediction purposes). It is possible that the server
      // will have registered a collision while the client prediction has not. If this
      // happens the client will be corrected in the next packet update.
      // Raycast the abstract PhysicsWorld if a PhysicsPlugin exists.
      bool hit = false;

      if ( mPhysicsWorld )
         hit = mPhysicsWorld->castRay( oldPosition, newPosition, &rInfo, Point3F( newPosition - oldPosition) * mDataBlock->impactForce );            
      else 
         hit = getContainer()->castRay(oldPosition, newPosition, csmDynamicCollisionMask | csmStaticCollisionMask, &rInfo);

      if ( hit )
      {
         // make sure the client knows to bounce
         if ( isServerObject() && ( rInfo.object->getTypeMask() & csmStaticCollisionMask ) == 0 )
            setMaskBits( BounceMask );

         // Next order of business: do we explode on this hit?
         if ( mCurrTick > mDataBlock->armingDelay || mDataBlock->armingDelay == 0 )
         {
            if(!(mTargetObject && mOnlyCollideWithTarget && rInfo.object != mTargetObject) || !mOnlyCollideWithTarget)
            {
               MatrixF xform( true );
               xform.setColumn( 3, rInfo.point );
               setTransform( xform );
               mCurrPosition    = rInfo.point;
               mCurrVelocity    = Point3F::Zero;

               // Get the object type before the onCollision call, in case
               // the object is destroyed.
               U32 objectType = rInfo.object->getTypeMask();

               // re-enable the collision response on the source object since
               // we need to process the onCollision and explode calls
               if ( disableSourceObjCollision )
                  mSourceObject->enableCollision();

               // Ok, here is how this works:
               // onCollision is called to notify the server scripts that a collision has occurred, then
               // a call to explode is made to start the explosion process. The call to explode is made
               // twice, once on the server and once on the client.
               // The server process is responsible for two things:
               //    1) setting the ExplosionMask network bit to guarantee that the client calls explode
               //    2) initiate the explosion process on the server scripts
               // The client process is responsible for only one thing:
               //    1) drawing the appropriate explosion

               // It is possible that during the processTick the server may have decided that a hit
               // has occurred while the client prediction has decided that a hit has not occurred.
               // In this particular scenario the client will have failed to call onCollision and
               // explode during the processTick. However, the explode function will be called
               // during the next packet update, due to the ExplosionMask network bit being set.
               // onCollision will remain uncalled on the client however, therefore no client
               // specific code should be placed inside the function!
               onCollision( rInfo.point, rInfo.normal, rInfo.object );
               explode( rInfo.point, rInfo.normal, objectType );

               // break out of the collision check, since we've exploded
               // we don't want to mess with the position and velocity
            }
         }
         else
         {
            if ( mDataBlock->isBallistic )
            {
               // Otherwise, this represents a bounce.  First, reflect our velocity
               //  around the normal...
               Point3F bounceVel = mCurrVelocity - rInfo.normal * (mDot( mCurrVelocity, rInfo.normal ) * 2.0);
               mCurrVelocity = bounceVel;

               // Add in surface friction...
               Point3F tangent = bounceVel - rInfo.normal * mDot(bounceVel, rInfo.normal);
               mCurrVelocity  -= tangent * mDataBlock->bounceFriction;

               // Now, take elasticity into account for modulating the speed of the grenade
               mCurrVelocity *= mDataBlock->bounceElasticity;

               // Set the new position to the impact and the bounce
               // will apply on the next frame.
               //F32 timeLeft = 1.0f - rInfo.t;
               newPosition = oldPosition = rInfo.point + rInfo.normal * 0.05f;
            }
         }
      }

      // re-enable the collision response on the source object now
      // that we are done processing the ballistic movement
      if ( disableSourceObjCollision )
         mSourceObject->enableCollision();
      enableCollision();
   }

   if ( isClientObject() )
   {
      emitParticles( mCurrPosition, newPosition, mCurrVelocity, U32( dt * 1000.0f ) );
      updateSound();
   }

   mCurrDeltaBase = newPosition;
   mCurrBackDelta = mCurrPosition - newPosition;
   mCurrPosition = newPosition;

   MatrixF xform( true );
   xform.setColumn( 3, mCurrPosition );
   setTransform( xform );
}

void SpellProjectile::updatePosition(F32 dt, Point3F &oldPosition, Point3F &newPosition)
{
   oldPosition = mCurrPosition;
   if ( mDataBlock->isBallistic )
      mCurrVelocity.z -= 9.81 * mDataBlock->gravityMod * dt;
   if(mHoming && mTargetObject)
   {
      Point3F dir = mTargetObject->getPosition() - oldPosition;
      dir.normalize();
      mCurrVelocity = dir * mCurrVelocity.len();
   }
   newPosition = oldPosition + mCurrVelocity * dt;
}