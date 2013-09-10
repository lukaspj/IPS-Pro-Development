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

#include "billboardRibbonParticleManager.h"
#include "billboardRibbonParticle.h"
#include "T3D\fx\particle.h"

BillboardRibbonParticleManager::BillboardRibbonParticleManager()
{
   part_list_head.next = NULL;
}

BillboardRibbonParticleManager::~BillboardRibbonParticleManager()
{
   for( S32 i = 0; i < part_store.size(); i++ )
   {
      delete [] ((BillboardRibbonParticle*)part_store[i]);
   }
}

Particle* BillboardRibbonParticleManager::AddParticle()
{
	n_parts++;
	if (n_parts > n_part_capacity || n_parts > mEmitterDatablock->getPartListInitSize())
	{
		// In an emergency we allocate additional particles in blocks of 16.
		// This should happen rarely.
      BillboardRibbonParticle* store_block = new BillboardRibbonParticle[16];
		part_store.push_back(store_block);
		n_part_capacity += 16;
		for (S32 i = 0; i < 16; i++)
		{
			store_block[i].next = part_freelist;
			part_freelist = &store_block[i];
		}
      mEmitterDatablock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
	}
   BillboardRibbonParticle* pNew = static_cast<BillboardRibbonParticle*>(part_freelist);
	part_freelist = pNew->next;
	pNew->next = part_list_head.next;
	part_list_head.next = pNew;
   return pNew;
}

//-----------------------------------------------------------------------------
// Initialize particle
//-----------------------------------------------------------------------------
void BillboardRibbonParticleManager::initializeParticle(Particle* init, const Point3F& inheritVelocity, ParticleData* pDat)
{
   ParticleManager::initializeParticle(init, inheritVelocity, pDat);

   BillboardRibbonParticle* _init = static_cast<BillboardRibbonParticle*>(init);
   BillboardRibbonParticleData* _pDat = static_cast<BillboardRibbonParticleData*>(pDat);

   // assign spin amount
   _init->spinSpeed = _pDat->spinSpeed * gRandGen.randF( _pDat->spinRandomMin, _pDat->spinRandomMax );
}

void BillboardRibbonParticleManager::UpdateKeyData( Particle* part, U32 numMilliseconds )
{
   ParticleManager::UpdateKeyData(part);

   BillboardRibbonParticle* _part = static_cast<BillboardRibbonParticle*>(part);
   BillboardRibbonParticleData* _data = static_cast<BillboardRibbonParticleData*>(_part->dataBlock);

   F32 t = F32(_part->currentAge) / F32(_part->totalLifetime);
   AssertFatal(t <= 1.0f, "Out out bounds filter function for particle.");
   
   for( U32 i = 1; i < ParticleData::PDC_NUM_KEYS; i++ )
   {
      if( _data->times[i] >= t )
      {
         F32 firstPart = t - _data->times[i-1];
         F32 total     = _data->times[i] - _part->dataBlock->times[i-1];

         firstPart /= total;

         if( mEmitterDatablock->useEmitterColors() )
         {
            _part->color.interpolate(mEmitter->getColors()[i-1], mEmitter->getColors()[i], firstPart);
         }
         else
         {
            _part->color.interpolate(_data->colors[i-1],
                                       _data->colors[i],
                                       firstPart);
         }
         break;
      }
   }
}

void BillboardRibbonParticleManager::RenderParticles( SceneRenderState* state )
{
   if( state->isReflectPass() && !mEmitterDatablock->isRenderingReflections() )
      return;

   // Never render into shadows.
   if (state->isShadowPass())
      return;

   PROFILE_SCOPE(BillboardRibbonParticleManager_prepRenderImage);

   if (  mEmitter->isDead() ||
         n_parts == 0 || 
         part_list_head.next == NULL )
      return;

   RenderPassManager *renderManager = state->getRenderPass();
   const Point3F &camPos = state->getCameraPosition();
   copyToVB( camPos, state->getAmbientLightColor() );

   if (!mVertBuff.isValid())
      return;

   ParticleRenderInst *ri = renderManager->allocInst<ParticleRenderInst>();

   ri->vertBuff = &mVertBuff;
   ri->primBuff = mEmitterDatablock->getPrimBuffRef();
   ri->translucentSort = true;
   ri->type = RenderPassManager::RIT_Particle;
   ri->sortDistSq = mEmitter->getRenderWorldBox().getSqDistanceToPoint( camPos );

   // Draw the system offscreen unless the highResOnly flag is set on the datablock
   ri->systemState = ( mEmitterDatablock->isHighResOnly() ? PSS_AwaitingHighResDraw : PSS_AwaitingOffscreenDraw );

   ri->modelViewProj = renderManager->allocUniqueXform(  GFX->getProjectionMatrix() * 
      GFX->getViewMatrix() * 
      GFX->getWorldMatrix() );

   // Update position on the matrix before multiplying it
   mEmitter->getBBObjToWorld().setPosition(mEmitter->getLastPosition());

   ri->bbModelViewProj = renderManager->allocUniqueXform( *ri->modelViewProj * mEmitter->getBBObjToWorld() );

   ri->count = n_parts;

   ri->blendStyle = mEmitterDatablock->getBlendStyle();
   
   // use first particle's texture unless there is an emitter texture to override it
   if (mEmitterDatablock->getTextureHandle())
      ri->diffuseTex = &*(mEmitterDatablock->getTextureHandle());
   else{
      BillboardRibbonParticleData* headnextdata = static_cast<BillboardRibbonParticleData*>(part_list_head.next->dataBlock);
      if(Con::getBoolVariable("$IPS::ParticleLOD"))
      {
         F32 dist = (mEmitter->getParentNodePos() - camPos).len();
         if(mEmitter->getParentNodeBox().isValidBox())
            dist = mEmitter->getParentNodeBox().getDistanceToPoint(camPos);
         bool hasHighRes = headnextdata->hResTextureName && headnextdata->hResTextureName[0];
         bool hasMidRes = headnextdata->mResTextureName && headnextdata->mResTextureName[0];
         bool hasLowRes = headnextdata->lResTextureName && headnextdata->lResTextureName[0];
         if((dist < mEmitterDatablock->getMediumResDistance() && hasHighRes) || (hasHighRes && !hasMidRes && !hasLowRes) )
            ri->diffuseTex = &*(headnextdata->hResTextureHandle);
         else if((dist < mEmitterDatablock->getLowResDistance() && hasMidRes ) || (hasMidRes && !hasLowRes) )
            ri->diffuseTex = &*(headnextdata->mResTextureHandle);
         else if(hasLowRes)
            ri->diffuseTex = &*(headnextdata->lResTextureHandle);
      }
      else{
         bool hasHighRes = headnextdata->hResTextureName && headnextdata->hResTextureName[0];
         bool hasMidRes = headnextdata->mResTextureName && headnextdata->mResTextureName[0];
         bool hasLowRes = headnextdata->lResTextureName && headnextdata->lResTextureName[0];
         if(hasHighRes)
            ri->diffuseTex = &*(headnextdata->hResTextureHandle);
         else if(hasMidRes)
            ri->diffuseTex = &*(headnextdata->mResTextureHandle);
         else if(hasLowRes)
            ri->diffuseTex = &*(headnextdata->lResTextureHandle);
      }
   }

   ri->softnessDistance = mEmitterDatablock->getSoftnessDistance(); 

   // Sort by texture too.
   ri->defaultKey = ri->diffuseTex ? (U32)ri->diffuseTex : (U32)ri->vertBuff;
   ri->defaultKey = (U32)ri->vertBuff;

   renderManager->addInst( ri );
}

//-----------------------------------------------------------------------------
// Copy particles to vertex buffer
//-----------------------------------------------------------------------------

// structure used for particle sorting.
struct SortParticle
{
   Particle* p;
   F32       k;
};

// qsort callback function for particle sorting
int QSORT_CALLBACK BRPM_cmpSortParticles(const void* p1, const void* p2)
{
   const SortParticle* sp1 = (const SortParticle*)p1;
   const SortParticle* sp2 = (const SortParticle*)p2;

   if (sp2->k > sp1->k)
      return 1;
   else if (sp2->k == sp1->k)
      return 0;
   else
      return -1;
}

void BillboardRibbonParticleManager::copyToVB( const Point3F &camPos, const ColorF &ambientColor )
{
   static Vector<SortParticle> orderedVector(__FILE__, __LINE__);

   PROFILE_START(BillboardRibbonParticleManager_copyToVB);

   PROFILE_START(BillboardRibbonParticleManager_copyToVB_Sort);
   // build sorted list of particles (far to near)
   if (mEmitterDatablock->useSortedParticles())
   {
      orderedVector.clear();

      MatrixF modelview = GFX->getWorldMatrix();
      Point3F viewvec; modelview.getRow(1, &viewvec);

      // add each particle and a distance based sort key to orderedVector
      for (Particle* pp = part_list_head.next; pp != NULL; pp = pp->next)
      {
         orderedVector.increment();
         orderedVector.last().p = pp;
         orderedVector.last().k = mDot(pp->pos, viewvec);
      }

      // qsort the list into far to near ordering
      dQsort(orderedVector.address(), orderedVector.size(), sizeof(SortParticle), BRPM_cmpSortParticles);
   }
   PROFILE_END();

#if defined(TORQUE_OS_XENON)
   // Allocate writecombined since we don't read back from this buffer (yay!)
   if(mVertBuff.isNull())
      mVertBuff = new GFX360MemVertexBuffer(GFX, 1, getGFXVertexFormat<ParticleVertexType>(), sizeof(ParticleVertexType), GFXBufferTypeDynamic, PAGE_WRITECOMBINE);
   if( n_parts > mCurBuffSize )
   {
      mCurBuffSize = n_parts;
      mVertBuff.resize(n_parts * 4);
   }

   ParticleVertexType *buffPtr = mVertBuff.lock();
#else
   static Vector<ParticleVertexType> tempBuff(2048);
   tempBuff.reserve( n_parts*4 + 64); // make sure tempBuff is big enough
   ParticleVertexType *buffPtr = tempBuff.address(); // use direct pointer (faster)
#endif

   PROFILE_START(BillboardRibbonParticleManager_copyToVB_Ribbon);

   if (mEmitterDatablock->useReversedOrder())
   {
      buffPtr += 4*(n_parts-1);
      // do sorted-oriented particles
      if (mEmitterDatablock->useSortedParticles())
      {
         SortParticle* partPtr = orderedVector.address();
         for (U32 i = 0; i < n_parts - 1; i++, partPtr++, buffPtr-=4 )
            setupRibbon(partPtr->p, partPtr++->p, partPtr--->p,camPos, ambientColor, buffPtr);
      }
      // do unsorted-oriented particles
      else
      {
		Particle* oldPtr = NULL;
		for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr-=4) {
            setupRibbon(partPtr, partPtr->next, oldPtr, camPos, ambientColor, buffPtr);
			oldPtr = partPtr;
		}
      }
   }
   else
   {
      // do sorted-oriented particles
      if (mEmitterDatablock->useSortedParticles())
      {
         SortParticle* partPtr = orderedVector.address();
      for (U32 i = 0; i < n_parts - 1; i++, partPtr++, buffPtr+=4 )
         setupRibbon(partPtr->p,  partPtr++->p, partPtr--->p, camPos, ambientColor, buffPtr);
      }
      // do unsorted-oriented particles
      else
      {
		Particle* oldPtr = NULL;
		for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr+=4) {
            setupRibbon(partPtr, partPtr->next, oldPtr, camPos, ambientColor, buffPtr);
			oldPtr = partPtr;
		}
      }
   }
	PROFILE_END();

#if defined(TORQUE_OS_XENON)
   mVertBuff.unlock();
#else
   PROFILE_START(BillboardRibbonParticleManager_copyToVB_LockCopy);
   // create new VB if emitter size grows
   if( !mVertBuff || n_parts > mCurBuffSize )
   {
      mCurBuffSize = n_parts;
      mVertBuff.set( GFX, n_parts * 4, GFXBufferTypeDynamic );
   }
   // lock and copy tempBuff to video RAM
   ParticleVertexType *verts = mVertBuff.lock();
   dMemcpy( verts, tempBuff.address(), n_parts * 4 * sizeof(ParticleVertexType) );
   mVertBuff.unlock();
   PROFILE_END();
#endif

   PROFILE_END();
}

//-----------------------------------------------------------------------------
// Set up Ribbon particle
//-----------------------------------------------------------------------------
void BillboardRibbonParticleManager::setupRibbon( Particle *part,
									 Particle *next,
									 Particle *prev,
                                     const Point3F &camPos,
                                     const ColorF &ambientColor,
                                     ParticleVertexType *lVerts)
{
   BillboardRibbonParticle* _part = static_cast<BillboardRibbonParticle*>(part);
   BillboardRibbonParticle* _prev = static_cast<BillboardRibbonParticle*>(part);
   BillboardRibbonParticleData* _pDat = static_cast<BillboardRibbonParticleData*>(part->dataBlock);
   Point3F dir, dirFromCam;
   Point3F crossDir, crossDirNext;
   Point3F start, end;
   ColorF prevCol;
   static Point3F crossDirPrev;
   static int position;
   static F32 alphaMod, alphaModEnd;

   const F32 ambientLerp = mClampF( mEmitterDatablock->getAmbientFactor(), 0.0f, 1.0f );
   ColorF partCol = mLerp( _part->color, ( _part->color * ambientColor ), ambientLerp );
   if (part->currentAge > part->totalLifetime)
   {
	   F32 alphaDeath = (part->currentAge - part->totalLifetime) / 200.0f;
	   if (alphaDeath > 1.0f)
		   alphaDeath = 1.0f;
	   alphaDeath = 1.0f - alphaDeath;
	   partCol.alpha *= alphaDeath;
   }

   start = part->pos;
   position++;

   if ( next == NULL && prev == NULL) {
	   // a ribbon of just one particle
      position = 0;

	  if( part->vel.magnitudeSafe() == 0.0 )
         dir = part->orientDir;
      else
   	     dir = part->vel;

	  dir.normalize();
	  dirFromCam = part->pos - camPos;
	  mCross( dirFromCam, dir, &crossDir );
	  crossDir.normalize();
 	  crossDir = crossDir * part->size * 0.5;
	  crossDirPrev = crossDir;

	  partCol.alpha = 0.0f;
	  prevCol = partCol;
   }
   else if (next == NULL && prev != NULL)
   {
      // last link in the chain, also the oldest
	  dir = part->pos - prev->pos;
	  dir.normalize();
	  dirFromCam = part->pos - camPos;
	  mCross( dirFromCam, dir, &crossDir );
	  crossDir.normalize();
 	  crossDir = crossDir * part->size * 0.5;

	  end = prev->pos;
	  partCol.alpha = 0.0f;
      prevCol = mLerp( _prev->color, ( _prev->color * ambientColor ), ambientLerp );
      prevCol.alpha *= alphaModEnd;
   } 
   else if (next != NULL && prev == NULL)
   {
      // first link in chain, newest particle
	  // since we draw from current to previous, this one isn't drawn
	  position = 0;

	  dir = next->pos - part->pos;
	  dir.normalize();

	  dirFromCam = part->pos - camPos;
	  mCross( dirFromCam, dir, &crossDir );
	  crossDir.normalize();
	  crossDir = crossDir * part->size * 0.5f;
      crossDirPrev = crossDir;

	  partCol.alpha = 0.0f;
      prevCol = partCol;
	  alphaModEnd = 0.0f;

	  end = part->pos;
   } 
   else
   {
      // middle of chain
	  dir = next->pos - prev->pos;
	  dir.normalize();
	  dirFromCam = part->pos - camPos;
	  mCross( dirFromCam, dir, &crossDir );
	  crossDir.normalize();

 	  crossDir = crossDir * part->size * 0.5;

      prevCol = mLerp( _prev->color, ( _prev->color * ambientColor ), ambientLerp );

	  if (position == 1)
	  {
	     // the second particle has a few tweaks for alpha, to smoothly match the first particle
	     // we only want to do this once when the particle first fades in, and avoid a strobing effect
         alphaMod = (float(part->currentAge) / float(part->currentAge - prev->currentAge)) - 1.0f;
         if (alphaMod > 1.0f)
            alphaMod = 1.0f;
         partCol.alpha *= alphaMod;
         prevCol.alpha = 0.0f;
         if (next->next == NULL)
            alphaModEnd = alphaMod;
         //Con::printf("alphaMod: %f", alphaMod );
	  }
	  else if (position == 2 )
	  {
         prevCol.alpha *= alphaMod;
		 alphaMod = 0.0f;
	  }

	  if ( next->next == NULL && position > 1)
	  {
         // next to last particle, start the fade out
		 alphaModEnd =  ( float(next->totalLifetime - next->currentAge)) / (float(part->totalLifetime - part->currentAge));
		 alphaModEnd *= 2.0f;
         if (alphaModEnd > 1.0f)
            alphaModEnd = 1.0f;
         partCol.alpha *= alphaModEnd;
		 //Con::printf("alphaMod: %f  Lifetime: %d  Age: %d", alphaMod, part->totalLifetime, part->currentAge );
	  }
	  end = prev->pos;
   }

   // Here we deal with UVs for animated particle (oriented)
   if (_pDat->animateTexture)
   { 
      // Let particle compute the UV indices for current frame
      S32 fm = (S32)(part->currentAge*(1.0f/1000.0f)*_pDat->framesPerSec);
      U8 fm_tile = _pDat->animTexFrames[fm % _pDat->numFrames];
      S32 uv[4];
      uv[0] = fm_tile + fm_tile/_pDat->animTexTiling.x;
      uv[1] = uv[0] + (_pDat->animTexTiling.x + 1);
      uv[2] = uv[1] + 1;
      uv[3] = uv[0] + 1;

     lVerts->point = start + crossDir;
     lVerts->color = partCol;
     // Here and below, we copy UVs from particle datablock's current frame's UVs (oriented)
     lVerts->texCoord = _pDat->animTexUVs[uv[0]];
     ++lVerts;

     lVerts->point = start - crossDir;
     lVerts->color = partCol;
     lVerts->texCoord = _pDat->animTexUVs[uv[1]];
     ++lVerts;

     lVerts->point = end - crossDirPrev;
     lVerts->color = prevCol;
     lVerts->texCoord = _pDat->animTexUVs[uv[2]];
     ++lVerts;

     lVerts->point = end + crossDirPrev;
     lVerts->color = prevCol;
     lVerts->texCoord = _pDat->animTexUVs[uv[3]];
     ++lVerts;

     crossDirPrev = crossDir;
     return;
   }

   lVerts->point = start + crossDir;
   lVerts->color = partCol;
   // Here and below, we copy UVs from particle datablock's texCoords (oriented)
   lVerts->texCoord = _pDat->texCoords[0];
   ++lVerts;

   lVerts->point = start - crossDir;
   lVerts->color = partCol;
   lVerts->texCoord = _pDat->texCoords[1];
   ++lVerts;

   lVerts->point = end - crossDirPrev;
   lVerts->color = prevCol;
   lVerts->texCoord = _pDat->texCoords[2];
   ++lVerts;

   lVerts->point = end + crossDirPrev;
   lVerts->color = prevCol;
   lVerts->texCoord = _pDat->texCoords[3];
   ++lVerts;

   crossDirPrev = crossDir;
}

void BillboardRibbonParticleManager::AllocateParticles()
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
      BillboardRibbonParticle* store_block = new BillboardRibbonParticle[n_part_capacity];
      part_store.push_back(store_block);
      part_freelist = store_block;
      BillboardRibbonParticle* last_part = static_cast<BillboardRibbonParticle*>(part_freelist);
      BillboardRibbonParticle* part = last_part+1;
      for( S32 i = 1; i < n_part_capacity; i++, part++, last_part++ )
      {
         last_part->next = part;
      }
      store_block[n_part_capacity-1].next = NULL;
      part_list_head.next = NULL;
      n_parts = 0;
   }
}