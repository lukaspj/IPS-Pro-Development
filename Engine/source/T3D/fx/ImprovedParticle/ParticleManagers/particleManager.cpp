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

#include "particleManager.h"

ParticleManager::ParticleManager()
{
   part_store = 0;
   part_freelist = NULL;
   n_part_capacity = 0;
   n_parts = 0;

   mCurBuffSize = 0;
}

ParticleManager::~ParticleManager()
{
}

void ParticleManager::DeleteDeadParticles(U32 numMSToUpdate)
{
   Particle* last_part = &part_list_head;
   for (Particle* part = part_list_head.next; part != NULL; part = part->next)
   {
      part->currentAge += numMSToUpdate;
      if (part->currentAge > part->totalLifetime)
      {
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
void ParticleManager::initializeParticle(Particle* init, const Point3F& inheritVelocity, ParticleData* pData)
{
   init->dataBlock = pData;

   // Calculate the constant accleration...
   init->vel += inheritVelocity * pData->inheritedVelFactor;
   init->acc  = init->vel * pData->constantAcceleration;

   // Calculate this instance's lifetime...
   init->totalLifetime = pData->lifetimeMS;
   if (pData->lifetimeVarianceMS != 0)
      init->totalLifetime += S32(gRandGen.randI() % (2 * pData->lifetimeVarianceMS + 1)) - S32(pData->lifetimeVarianceMS);
}

void ParticleManager::UpdateKeyData( Particle* part, U32 numMilliseconds )
{
   //Ensure that our lifetime is never below 0
   if( part->totalLifetime < 1 )
      part->totalLifetime = 1;

   F32 t = F32(part->currentAge) / F32(part->totalLifetime);
   AssertFatal(t <= 1.0f, "Out out bounds filter function for particle.");
   
   for( U32 i = 1; i < ParticleData::PDC_NUM_KEYS; i++ )
   {
      if( part->dataBlock->times[i] >= t )
      {
         F32 firstPart = t - part->dataBlock->times[i-1];
         F32 total     = part->dataBlock->times[i] -
            part->dataBlock->times[i-1];

         firstPart /= total;

         if( mEmitterDatablock->useEmitterSizes() )
         {
            part->size = (mEmitter->getSizes()[i-1] * (1.0 - firstPart)) +
               (mEmitter->getSizes()[i]   * firstPart);
         }
         else
         {
            part->size = (part->dataBlock->sizes[i-1] * (1.0 - firstPart)) +
               (part->dataBlock->sizes[i]   * firstPart);
         }
         break;

      }
   }
}

ColorF ParticleManager::getCollectiveColor()
{
   U32 count = 0;
   ColorF color = ColorF(0.0f, 0.0f, 0.0f);

   count = n_parts;
   for( Particle* part = part_list_head.next; part != NULL; part = part->next )
   {
      //color += part->getColor();
   }

   if(count > 0)
   {
      //color /= F32(count);
   }

   //if(color.red == 0.0f && color.green == 0.0f && color.blue == 0.0f)
   //	color = color;

   return color;
}