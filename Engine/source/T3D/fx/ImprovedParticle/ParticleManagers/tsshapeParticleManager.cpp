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

#include "tsshapeParticleManager.h"
#include "tsshapeParticle.h"
#include "T3D\fx\particle.h"
#include "sim\netObject.h"
#include "T3D\pointLight.h"
#include "gfx/gfxTransformSaver.h"
#include "lighting/lightQuery.h"

TSShapeParticleManager::TSShapeParticleManager()
{
   part_list_head.next = NULL;
}

TSShapeParticleManager::~TSShapeParticleManager()
{
   for (Particle* part = part_list_head.next; part != NULL; part = part->next)
   {
      TSShapeParticle* _part = static_cast<TSShapeParticle*>(part);
      if(_part->shape)
         delete _part->shape;
   }
   for( S32 i = 0; i < part_store.size(); i++ )
   {
      delete [] ((TSShapeParticle*)part_store[i]);
   }
}

Particle* TSShapeParticleManager::AddParticle()
{
	n_parts++;
   if (n_parts > n_part_capacity || n_parts > mEmitterDatablock->getPartListInitSize())
	{
		// In an emergency we allocate additional particles in blocks of 16.
		// This should happen rarely.
      TSShapeParticle* store_block = new TSShapeParticle[16];
		part_store.push_back(store_block);
		n_part_capacity += 16;
		for (S32 i = 0; i < 16; i++)
		{
			store_block[i].next = part_freelist;
			part_freelist = &store_block[i];
		}
      mEmitterDatablock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
	}
   TSShapeParticle* pNew = static_cast<TSShapeParticle*>(part_freelist);
	part_freelist = pNew->next;
	pNew->next = part_list_head.next;
	part_list_head.next = pNew;
   return pNew;
}

void TSShapeParticleManager::DeleteDeadParticles(U32 numMSToUpdate)
{
   Particle* last_part = &part_list_head;
   for (Particle* part = part_list_head.next; part != NULL; part = part->next)
   {
      part->currentAge += numMSToUpdate;
      if (part->currentAge > part->totalLifetime)
      {
         TSShapeParticle* _part = static_cast<TSShapeParticle*>(part);
         if(_part->shape)
            SAFE_DELETE( _part->shape );
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
void TSShapeParticleManager::initializeParticle(Particle* init, const Point3F& inheritVelocity, ParticleData* pDat)
{
   ParticleManager::initializeParticle(init, inheritVelocity, pDat);

   TSShapeParticle* _init = static_cast<TSShapeParticle*>(init);
   TSShapeParticleData* _pDat = static_cast<TSShapeParticleData*>(pDat);
   if(_pDat->mShape)
      _init->shape = new TSShapeInstance(_pDat->mShape, true);
   _init->pos = _init->pos;
}

void TSShapeParticleManager::UpdateKeyData( Particle* part, U32 numMilliseconds )
{
   ParticleManager::UpdateKeyData(part);
   //TSShapeParticle* _part = static_cast<TSShapeParticle*>(part);
   return;


   /*F32 t = F32(_part->currentAge) / F32(_part->totalLifetime);
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

void TSShapeParticleManager::RenderParticles( SceneRenderState* state )
{
   for (Particle* part = part_list_head.next; part != NULL; part = part->next)
   {
      RenderParticle(state, part);
   }
}

void TSShapeParticleManager::RenderParticle( SceneRenderState* state, Particle* part)
{
   TSShapeParticle* _part = static_cast<TSShapeParticle*>(part);
   // Make sure we have a TSShapeInstance
   if ( !_part->shape || _part->size == 0.0f )
      return;

   // Calculate the distance of this object from the camera
   Point3F cameraOffset = _part->pos;
   cameraOffset -= state->getDiffuseCameraPosition();
   F32 dist = cameraOffset.len();
   if ( dist < 0.01f )
      dist = 0.01f;

   // Set up the LOD for the shape
   F32 invScale = ( 1.0f / _part->size );

   _part->shape->setDetailFromDistance( state, dist * invScale );

   // Make sure we have a valid level of detail
   if ( _part->shape->getCurrentDetail() < 0 )
      return;

   // GFXTransformSaver is a handy helper class that restores
   // the current GFX matrices to their original values when
   // it goes out of scope at the end of the function
   GFXTransformSaver saver;

   // Set up our TS render state      
   TSRenderState rdata;
   rdata.setSceneState( state );
   rdata.setFadeOverride( 1.0f );

   // We might have some forward lit materials
   // so pass down a query to gather lights.
   LightQuery query;
   query.init( SphereF(_part->pos, _part->size / 2) );
   rdata.setLightQuery( &query );

   // Set the world matrix to the objects render transform
   MatrixF mat = MatrixF(_part->orientDir, _part->pos);
   mat.scale( _part->size );
  // F32 det = m_matF_determinant( mat );
   GFX->setWorldMatrix( mat );

   // Animate the the shape
   _part->shape->animate();

   // Allow the shape to submit the RenderInst(s) for itself
   _part->shape->render( rdata );
}

void TSShapeParticleManager::AllocateParticles()
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
      TSShapeParticle* store_block = new TSShapeParticle[n_part_capacity];
      part_store.push_back(store_block);
      part_freelist = store_block;
      TSShapeParticle* last_part = static_cast<TSShapeParticle*>(part_freelist);
      TSShapeParticle* part = last_part+1;
      for( S32 i = 1; i < n_part_capacity; i++, part++, last_part++ )
      {
         last_part->next = part;
      }
      store_block[n_part_capacity-1].next = NULL;
      part_list_head.next = NULL;
      n_parts = 0;
   }
}