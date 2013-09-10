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

#include "effectParticleManager.h"
#include "effectParticle.h"
#include "T3D\fx\particle.h"
#include "sim\netObject.h"
#include "T3D\pointLight.h"
#include "gfx/gfxTransformSaver.h"
#include "lighting/lightQuery.h"

EffectParticleManager::EffectParticleManager()
{
   part_list_head.next = NULL;
}

EffectParticleManager::~EffectParticleManager()
{
   for (Particle* part = part_list_head.next; part != NULL; part = part->next)
   {
      EffectParticle* _part = static_cast<EffectParticle*>(part);
      if(_part->effect)
         _part->effect->deleteObject();
   }
   for( S32 i = 0; i < part_store.size(); i++ )
   {
      delete [] ((EffectParticle*)part_store[i]);
   }
}

Particle* EffectParticleManager::AddParticle()
{
	n_parts++;
   if (n_parts > n_part_capacity || n_parts > mEmitterDatablock->getPartListInitSize())
	{
		// In an emergency we allocate additional particles in blocks of 16.
		// This should happen rarely.
      EffectParticle* store_block = new EffectParticle[16];
		part_store.push_back(store_block);
		n_part_capacity += 16;
		for (S32 i = 0; i < 16; i++)
		{
			store_block[i].next = part_freelist;
			part_freelist = &store_block[i];
		}
      mEmitterDatablock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
	}
   EffectParticle* pNew = static_cast<EffectParticle*>(part_freelist);
	part_freelist = pNew->next;
	pNew->next = part_list_head.next;
	part_list_head.next = pNew;
   pNew->effect = NULL;
   return pNew;
}

void EffectParticleManager::DeleteDeadParticles(U32 numMSToUpdate)
{
   Particle* last_part = &part_list_head;
   for (Particle* part = part_list_head.next; part != NULL; part = part->next)
   {
      part->currentAge += numMSToUpdate;
      if (part->currentAge > part->totalLifetime)
      {
         EffectParticle* _part = static_cast<EffectParticle*>(part);
         if(_part->effect){
            _part->effect->deleteObject();
            _part->effect = NULL;
         }
         n_parts--;
         last_part->next = part->next;
         part->next = part_freelist;
         part_freelist = part;
         part = last_part;
      }
      else
      {
         last_part = part;
      }
   }
}

//-----------------------------------------------------------------------------
// Initialize particle
//-----------------------------------------------------------------------------
void EffectParticleManager::initializeParticle(Particle* init, const Point3F& inheritVelocity, ParticleData* pDat)
{
   ParticleManager::initializeParticle(init, inheritVelocity, pDat);

   EffectParticle* _init = static_cast<EffectParticle*>(init);
   EffectParticleData* _pDat = static_cast<EffectParticleData*>(pDat);
   if(_pDat->mEffectData)
   {
      _init->effect = new ParticleEffect;
      _init->effect->setGhost();
      _init->effect->setDataBlock(_pDat->mEffectData);
      _init->effect->mLifeTimeMS = init->totalLifetime;
      _init->effect->registerObject();
   }
}

void EffectParticleManager::UpdateKeyData( Particle* part, U32 numMilliseconds )
{
   ParticleManager::UpdateKeyData(part);
   EffectParticle* _part = static_cast<EffectParticle*>(part);
   if(_part->effect)
      _part->effect->setPosition(_part->pos);
   return;


  /* F32 t = F32(_part->currentAge) / F32(_part->totalLifetime);
   AssertFatal(t <= 1.0f, "Out out bounds filter function for particle.");
   
   for( U32 i = 1; i < ParticleData::PDC_NUM_KEYS; i++ )
   {
      if( _part->dataBlock->times[i] >= t )
      {
         F32 firstPart = t - _part->dataBlock->times[i-1];
         F32 total     = _part->dataBlock->times[i] - _part->dataBlock->times[i-1];

         firstPart /= total;

         break;
      }
   }*/
}

void EffectParticleManager::RenderParticles( SceneRenderState* state )
{
}

void EffectParticleManager::AllocateParticles()
{
   //   Allocate particle structures and init the freelist. Member part_store
   //   is a Vector so that we can allocate more particles if partListInitSize
   //   turns out to be too small. 
   //
   
   if (mEmitterDatablock->getPartListInitSize() > 0)
   {
      for( S32 i = 0; i < part_store.size(); i++ )
      {
         delete [] part_store[i];
      }
      part_store.clear();
      n_part_capacity = mEmitterDatablock->getPartListInitSize();
      EffectParticle* store_block = new EffectParticle[n_part_capacity];
      part_store.push_back(store_block);
      part_freelist = store_block;
      EffectParticle* last_part = static_cast<EffectParticle*>(part_freelist);
      EffectParticle* part = last_part+1;
      for( S32 i = 1; i < n_part_capacity; i++, part++, last_part++ )
      {
         last_part->next = part;
      }
      store_block[n_part_capacity-1].next = NULL;
      part_list_head.next = NULL;
      n_parts = 0;
   }
}