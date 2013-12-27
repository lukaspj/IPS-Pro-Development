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

#include "SpellCooldowns.h"

IMPLEMENT_CO_NETOBJECT_V1(CooldownManager);

CooldownManager::CooldownManager()
{
   mNetFlags.set(Ghostable);
}

void CooldownManager::AdvanceTime(U32 ms)
{
   Vector<Cooldown> tbr;
   for(int i = 0; i < mCooldownVector.size(); i++)
   {
      if(mCooldownVector[i].TimeLeft <= ms)
         tbr.push_back(mCooldownVector[i]);
      mCooldownVector[i].TimeLeft -= ms;
   }
   while(tbr.size() > 0)
   {
      RemoveCooldown(tbr[0].Owner);
      tbr.pop_front();
   }
}

void CooldownManager::AddCooldown(SpellData* dat)
{
   Cooldown cd;
   cd.TimeLeft = dat->mCooldown;
   cd.Owner = dat;
   mCooldownVector.push_back(cd);
   mNewCooldowns.push_back(cd);
   setMaskBits(CooldownActionFlags::cdAdd);
}

void CooldownManager::RemoveCooldown(SpellData* dat)
{
   for(int i = 0; i < mCooldownVector.size(); i++)
   {
      if(mCooldownVector[i].Owner == dat){
         mRemovedCooldowns.push_back(mCooldownVector[i]);
         setMaskBits(CooldownActionFlags::cdRemove);
         mCooldownVector.erase_fast(i);
      }
   }
}

U16 CooldownManager::GetCooldown(SpellData* dat)
{
   for(int i = 0; i < mCooldownVector.size(); i++)
   {
      if(mCooldownVector[i].Owner == dat)
         return mCooldownVector[i].TimeLeft;
   }
   return 0;
}

bool CooldownManager::IsOnCooldown(SpellData* dat)
{
   for(int i = 0; i < mCooldownVector.size(); i++)
   {
      if(mCooldownVector[i].Owner == dat)
         return true;
   }
   return false;
}

U16 CooldownManager::Count()
{
   return mCooldownVector.size();
}

U32 CooldownManager::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate(conn, mask, stream);
   if(isServerObject())
      setProcessTicks(false);
   if(stream->writeFlag(mask & CooldownActionFlags::cdAdd))
   {
      stream->writeInt(mNewCooldowns.size(), 5);
      for(int i = 0; i < mNewCooldowns.size(); i++)
      {
         Cooldown cd = mNewCooldowns[i];
         stream->writeRangedU32( cd.Owner->getId(), 
                                 DataBlockObjectIdFirst,
                                 DataBlockObjectIdLast );
         //stream->writeInt(cd.TimeLeft, 22);
      }
      mNewCooldowns.clear();
   }

   if(stream->writeFlag(mask & CooldownActionFlags::cdRemove))
   {
      stream->writeInt(mRemovedCooldowns.size(), 5);
      for(int i = 0; i < mRemovedCooldowns.size(); i++)
      {
         Cooldown cd = mRemovedCooldowns[i];
         stream->writeRangedU32( cd.Owner->getId(), 
                                 DataBlockObjectIdFirst,
                                 DataBlockObjectIdLast );
      }
      mRemovedCooldowns.clear();
   }

   return retMask;
}

void CooldownManager::unpackUpdate(NetConnection* conn, BitStream* stream)
{
   Parent::unpackUpdate(conn, stream);

   if(stream->readFlag())
   {
      U8 size = stream->readInt(5);
      for(U8 i = 0; i < size; i++)
      {
         U32 id = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );
         SpellData *dat = NULL;
      
         if ( Sim::findObject( id, dat ) )
         {
            Cooldown cd;
            cd.TimeLeft = dat->mCooldown;
            cd.Owner = dat;
            mCooldownVector.push_back(cd);
         }
         else
            conn->setLastError( "CooldownManager::unpackUpdate() - invalid SpellData!" );
      }
   }

   if(stream->readFlag())
   {
      U8 size = stream->readInt(5);
      for(U8 i = 0; i < size; i++)
      {
         U32 id = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );
         SpellData *dat = NULL;
      
         if ( Sim::findObject( id, dat ) )
         {
            RemoveCooldown(dat);
         }
         else
            conn->setLastError( "CooldownManager::unpackUpdate() - invalid SpellData!" );
      }
   }
}

//----------------------- ITickable -------------------------
void CooldownManager::interpolateTick( F32 delta ) 
{
}
void CooldownManager::processTick() 
{
}
void CooldownManager::advanceTime( F32 timeDelta ) 
{
   if(isProperlyAdded() && isClientObject())
   {
      AdvanceTime(timeDelta);
   }
}