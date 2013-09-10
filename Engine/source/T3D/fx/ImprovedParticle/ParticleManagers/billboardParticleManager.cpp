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

#include "billboardParticleManager.h"
#include "billboardParticle.h"
#include "T3D\fx\particle.h"

BillboardParticleManager::BillboardParticleManager()
{
   part_list_head.next = NULL;
}

BillboardParticleManager::~BillboardParticleManager()
{
   for( S32 i = 0; i < part_store.size(); i++ )
   {
      delete [] ((BillboardParticle*)part_store[i]);
   }
}

Particle* BillboardParticleManager::AddParticle()
{
	n_parts++;
	if (n_parts > n_part_capacity || n_parts > mEmitterDatablock->getPartListInitSize())
	{
		// In an emergency we allocate additional particles in blocks of 16.
		// This should happen rarely.
      BillboardParticle* store_block = new BillboardParticle[16];
		part_store.push_back(store_block);
		n_part_capacity += 16;
		for (S32 i = 0; i < 16; i++)
		{
			store_block[i].next = part_freelist;
			part_freelist = &store_block[i];
		}
      mEmitterDatablock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
	}
   BillboardParticle* pNew = static_cast<BillboardParticle*>(part_freelist);
	part_freelist = pNew->next;
	pNew->next = part_list_head.next;
	part_list_head.next = pNew;
   return pNew;
}

//-----------------------------------------------------------------------------
// Initialize particle
//-----------------------------------------------------------------------------
void BillboardParticleManager::initializeParticle(Particle* init, const Point3F& inheritVelocity, ParticleData* pDat)
{
   ParticleManager::initializeParticle(init, inheritVelocity, pDat);

   BillboardParticle* _init = static_cast<BillboardParticle*>(init);
   BillboardParticleData* _pDat = static_cast<BillboardParticleData*>(pDat);

   // assign spin amount
   _init->spinSpeed = _pDat->spinSpeed * gRandGen.randF( _pDat->spinRandomMin, _pDat->spinRandomMax );
}

void BillboardParticleManager::UpdateKeyData( Particle* part, U32 numMilliseconds )
{
   ParticleManager::UpdateKeyData(part);

   BillboardParticle* _part = static_cast<BillboardParticle*>(part);
   BillboardParticleData* _data = static_cast<BillboardParticleData*>(_part->dataBlock);

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

void BillboardParticleManager::RenderParticles( SceneRenderState* state )
{
   if( state->isReflectPass() && !mEmitterDatablock->isRenderingReflections() )
      return;

   // Never render into shadows.
   if (state->isShadowPass())
      return;

   PROFILE_SCOPE(ParticleEmitter_prepRenderImage);

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
      BillboardParticleData* headnextdata = static_cast<BillboardParticleData*>(part_list_head.next->dataBlock);
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
int QSORT_CALLBACK cmpSortParticles(const void* p1, const void* p2)
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

void BillboardParticleManager::copyToVB( const Point3F &camPos, const ColorF &ambientColor )
{
   static Vector<SortParticle> orderedVector(__FILE__, __LINE__);

   PROFILE_START(ParticleEmitter_copyToVB);

   PROFILE_START(ParticleEmitter_copyToVB_Sort);
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
      dQsort(orderedVector.address(), orderedVector.size(), sizeof(SortParticle), cmpSortParticles);
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

   if (mEmitterDatablock->useOrientedParticles())
   {
      PROFILE_START(ParticleEmitter_copyToVB_Orient);

      if (mEmitterDatablock->useReversedOrder())
      {
         buffPtr += 4*(n_parts-1);
         // do sorted-oriented particles
         if (mEmitterDatablock->useSortedParticles())
         {
            SortParticle* partPtr = orderedVector.address();
            for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr-=4 )
               setupOriented(partPtr->p, camPos, ambientColor, buffPtr);
         }
         // do unsorted-oriented particles
         else
         {
            for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr-=4)
               setupOriented(partPtr, camPos, ambientColor, buffPtr);
         }
      }
      else
      {
         // do sorted-oriented particles
         if (mEmitterDatablock->useSortedParticles())
         {
            SortParticle* partPtr = orderedVector.address();
            for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr+=4 )
               setupOriented(partPtr->p, camPos, ambientColor, buffPtr);
         }
         // do unsorted-oriented particles
         else
         {
            for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr+=4)
               setupOriented(partPtr, camPos, ambientColor, buffPtr);
         }
      }
      PROFILE_END();
   }
   else if (mEmitterDatablock->useAlignedParticles())
   {
      PROFILE_START(ParticleEmitter_copyToVB_Aligned);

      if (mEmitterDatablock->useReversedOrder())
      {
         buffPtr += 4*(n_parts-1);

         // do sorted-oriented particles
         if (mEmitterDatablock->useSortedParticles())
         {
            SortParticle* partPtr = orderedVector.address();
            for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr-=4 )
               setupAligned(partPtr->p, ambientColor, buffPtr);
         }
         // do unsorted-oriented particles
         else
         {
            Particle *partPtr = part_list_head.next;
            for (; partPtr != NULL; partPtr = partPtr->next, buffPtr-=4)
               setupAligned(partPtr, ambientColor, buffPtr);
         }
      }
      else
      {
         // do sorted-oriented particles
         if (mEmitterDatablock->useSortedParticles())
         {
            SortParticle* partPtr = orderedVector.address();
            for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr+=4 )
               setupAligned(partPtr->p, ambientColor, buffPtr);
         }
         // do unsorted-oriented particles
         else
         {
            Particle *partPtr = part_list_head.next;
            for (; partPtr != NULL; partPtr = partPtr->next, buffPtr+=4)
               setupAligned(partPtr, ambientColor, buffPtr);
         }
      }
      PROFILE_END();
   }
   else
   {
      PROFILE_START(ParticleEmitter_copyToVB_NonOriented);
      // somewhat odd ordering so that texture coordinates match the oriented
      // particles
      Point3F basePoints[4];
      basePoints[0] = Point3F(-1.0, 0.0,  1.0);
      basePoints[1] = Point3F(-1.0, 0.0, -1.0);
      basePoints[2] = Point3F( 1.0, 0.0, -1.0);
      basePoints[3] = Point3F( 1.0, 0.0,  1.0);

      MatrixF camView = GFX->getWorldMatrix();
      camView.transpose();  // inverse - this gets the particles facing camera

      if (mEmitterDatablock->useReversedOrder())
      {
         buffPtr += 4*(n_parts-1);
         // do sorted-billboard particles
         if (mEmitterDatablock->useSortedParticles())
         {
            SortParticle *partPtr = orderedVector.address();
            for( U32 i=0; i<n_parts; i++, partPtr++, buffPtr-=4 )
               setupBillboard( partPtr->p, basePoints, camView, ambientColor, buffPtr );
         }
         // do unsorted-billboard particles
         else
         {
            for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr-=4)
               setupBillboard( partPtr, basePoints, camView, ambientColor, buffPtr );
         }
      }
      else
      {
         // do sorted-billboard particles
         if (mEmitterDatablock->useSortedParticles())
         {
            SortParticle *partPtr = orderedVector.address();
            for( U32 i=0; i<n_parts; i++, partPtr++, buffPtr+=4 )
               setupBillboard( partPtr->p, basePoints, camView, ambientColor, buffPtr );
         }
         // do unsorted-billboard particles
         else
         {
            for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr+=4)
               setupBillboard( partPtr, basePoints, camView, ambientColor, buffPtr );
         }
      }

      PROFILE_END();
   }

#if defined(TORQUE_OS_XENON)
   mVertBuff.unlock();
#else
   PROFILE_START(ParticleEmitter_copyToVB_LockCopy);
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
// Set up particle for billboard style render
//-----------------------------------------------------------------------------
void BillboardParticleManager::setupBillboard( Particle *part,
                                     Point3F *basePts,
                                     const MatrixF &camView,
                                     const ColorF &ambientColor,
                                     ParticleVertexType *lVerts )
{
   BillboardParticle* _part = static_cast<BillboardParticle*>(part);
   F32 width     = part->size * 0.5f;
   F32 spinAngle = _part->spinSpeed * part->currentAge * mEmitter->getAgedSpinToRadians();

   F32 sy, cy;
   mSinCos(spinAngle, sy, cy);

   const F32 ambientLerp = mClampF( mEmitterDatablock->getAmbientFactor(), 0.0f, 1.0f );
   ColorF partCol = mLerp( _part->color, ( _part->color  * ambientColor ), ambientLerp );

   // fill four verts, use macro and unroll loop
#define fillVert(){ \
   lVerts->point.x = cy * basePts->x - sy * basePts->z;  \
   lVerts->point.y = 0.0f;                                \
   lVerts->point.z = sy * basePts->x + cy * basePts->z;  \
   camView.mulV( lVerts->point );                        \
   lVerts->point *= width;                               \
   lVerts->point += part->pos;                           \
   lVerts->color = partCol; } \

   BillboardParticleData* partdatablock = static_cast<BillboardParticleData*>(part->dataBlock);
   // Here we deal with UVs for animated particle (billboard)
   if (partdatablock->animateTexture)
   { 
      S32 fm = (S32)(part->currentAge*(1.0/1000.0)*partdatablock->framesPerSec);
      U8 fm_tile = partdatablock->animTexFrames[fm % partdatablock->numFrames];
      S32 uv[4];
      uv[0] = fm_tile + fm_tile/partdatablock->animTexTiling.x;
      uv[1] = uv[0] + (partdatablock->animTexTiling.x + 1);
      uv[2] = uv[1] + 1;
      uv[3] = uv[0] + 1;

      fillVert();
      // Here and below, we copy UVs from particle datablock's current frame's UVs (billboard)
      lVerts->texCoord = partdatablock->animTexUVs[uv[0]];
      ++lVerts;
      ++basePts;

      fillVert();
      lVerts->texCoord = partdatablock->animTexUVs[uv[1]];
      ++lVerts;
      ++basePts;

      fillVert();
      lVerts->texCoord = partdatablock->animTexUVs[uv[2]];
      ++lVerts;
      ++basePts;

      fillVert();
      lVerts->texCoord = partdatablock->animTexUVs[uv[3]];
      ++lVerts;
      ++basePts;

      return;
   }

   fillVert();
   // Here and below, we copy UVs from particle datablock's texCoords (billboard)
   lVerts->texCoord = partdatablock->texCoords[0];
   ++lVerts;
   ++basePts;

   fillVert();
   lVerts->texCoord = partdatablock->texCoords[1];
   ++lVerts;
   ++basePts;

   fillVert();
   lVerts->texCoord = partdatablock->texCoords[2];
   ++lVerts;
   ++basePts;

   fillVert();
   lVerts->texCoord = partdatablock->texCoords[3];
   ++lVerts;
   ++basePts;
}

//-----------------------------------------------------------------------------
// Set up oriented particle
//-----------------------------------------------------------------------------
void BillboardParticleManager::setupOriented( Particle *part,
                                    const Point3F &camPos,
                                    const ColorF &ambientColor,
                                    ParticleVertexType *lVerts )
{
   BillboardParticle* _part = static_cast<BillboardParticle*>(part);
   Point3F dir;

   if( mEmitterDatablock->isOrientedOnVelocity() )
   {
      // don't render oriented particle if it has no velocity
      if( part->vel.magnitudeSafe() == 0.0 ) return;
      dir = part->vel;
   }
   else
   {
      dir = part->orientDir;
   }

   Point3F dirFromCam = part->pos - camPos;
   Point3F crossDir;
   mCross( dirFromCam, dir, &crossDir );
   crossDir.normalize();
   dir.normalize();

   F32 width = part->size * 0.5f;
   dir *= width;
   crossDir *= width;
   Point3F start = part->pos - dir;
   Point3F end = part->pos + dir;

   const F32 ambientLerp = mClampF( mEmitterDatablock->getAmbientFactor(), 0.0f, 1.0f );
   ColorF partCol = mLerp( _part->color, ( _part->color * ambientColor ), ambientLerp );
   
   BillboardParticleData* partdatablock = static_cast<BillboardParticleData*>(part->dataBlock);

   // Here we deal with UVs for animated particle (oriented)
   if (partdatablock->animateTexture)
   { 
      // Let particle compute the UV indices for current frame
      S32 fm = (S32)(part->currentAge*(1.0f/1000.0f)*partdatablock->framesPerSec);
      U8 fm_tile = partdatablock->animTexFrames[fm % partdatablock->numFrames];
      S32 uv[4];
      uv[0] = fm_tile + fm_tile/partdatablock->animTexTiling.x;
      uv[1] = uv[0] + (partdatablock->animTexTiling.x + 1);
      uv[2] = uv[1] + 1;
      uv[3] = uv[0] + 1;

      lVerts->point = start + crossDir;
      lVerts->color = partCol;
      // Here and below, we copy UVs from particle datablock's current frame's UVs (oriented)
      lVerts->texCoord = partdatablock->animTexUVs[uv[0]];
      ++lVerts;

      lVerts->point = start - crossDir;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->animTexUVs[uv[1]];
      ++lVerts;

      lVerts->point = end - crossDir;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->animTexUVs[uv[2]];
      ++lVerts;

      lVerts->point = end + crossDir;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->animTexUVs[uv[3]];
      ++lVerts;

      return;
   }

   lVerts->point = start + crossDir;
   lVerts->color = partCol;
   // Here and below, we copy UVs from particle datablock's texCoords (oriented)
   lVerts->texCoord = partdatablock->texCoords[0];
   ++lVerts;

   lVerts->point = start - crossDir;
   lVerts->color = partCol;
   lVerts->texCoord = partdatablock->texCoords[1];
   ++lVerts;

   lVerts->point = end - crossDir;
   lVerts->color = partCol;
   lVerts->texCoord = partdatablock->texCoords[2];
   ++lVerts;

   lVerts->point = end + crossDir;
   lVerts->color = partCol;
   lVerts->texCoord = partdatablock->texCoords[3];
   ++lVerts;
}

void BillboardParticleManager::setupAligned( Particle *part, 
                                   const ColorF &ambientColor,
                                   ParticleVertexType *lVerts )
{
   BillboardParticle* _part = static_cast<BillboardParticle*>(part);
   // The aligned direction will always be normalized.
   Point3F dir = mEmitterDatablock->getAlignDirection();

   // Find a right vector for this particle.
   Point3F right;
   if (mFabs(dir.y) > mFabs(dir.z))
      mCross(Point3F::UnitZ, dir, &right);
   else
      mCross(Point3F::UnitY, dir, &right);
   right.normalize();
   
   // If we have a spin velocity.
   if ( !mIsZero( _part->spinSpeed ) )
   {
      F32 spinAngle = _part->spinSpeed * part->currentAge * mEmitter->getAgedSpinToRadians();

      // This is an inline quaternion vector rotation which
      // is faster that QuatF.mulP(), but generates different
      // results and hence cannot replace it right now.

      F32 sin, qw;
      mSinCos( spinAngle * 0.5f, sin, qw );
      F32 qx = dir.x * sin;
      F32 qy = dir.y * sin;
      F32 qz = dir.z * sin;

      F32 vx = ( right.x * qw ) + ( right.z * qy ) - ( right.y * qz );
      F32 vy = ( right.y * qw ) + ( right.x * qz ) - ( right.z * qx );
      F32 vz = ( right.z * qw ) + ( right.y * qx ) - ( right.x * qy );
      F32 vw = ( right.x * qx ) + ( right.y * qy ) + ( right.z * qz );

      right.x = ( qw * vx ) + ( qx * vw ) + ( qy * vz ) - ( qz * vy );
      right.y = ( qw * vy ) + ( qy * vw ) + ( qz * vx ) - ( qx * vz );
      right.z = ( qw * vz ) + ( qz * vw ) + ( qx * vy ) - ( qy * vx );
   }

   // Get the cross vector.
   Point3F cross;
   mCross(right, dir, &cross);

   F32 width = part->size * 0.5f;
   right *= width;
   cross *= width;
   Point3F start = part->pos - right;
   Point3F end = part->pos + right;

   const F32 ambientLerp = mClampF( mEmitterDatablock->getAmbientFactor(), 0.0f, 1.0f );
   ColorF partCol = mLerp( _part->color, ( _part->color * ambientColor ), ambientLerp );
   
   BillboardParticleData* partdatablock = static_cast<BillboardParticleData*>(part->dataBlock);

   // Here we deal with UVs for animated particle
   if (partdatablock->animateTexture)
   { 
      // Let particle compute the UV indices for current frame
      S32 fm = (S32)(part->currentAge*(1.0f/1000.0f)*partdatablock->framesPerSec);
      U8 fm_tile = partdatablock->animTexFrames[fm % partdatablock->numFrames];
      S32 uv[4];
      uv[0] = fm_tile + fm_tile/partdatablock->animTexTiling.x;
      uv[1] = uv[0] + (partdatablock->animTexTiling.x + 1);
      uv[2] = uv[1] + 1;
      uv[3] = uv[0] + 1;

      lVerts->point = start + cross;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->animTexUVs[uv[0]];
      ++lVerts;

      lVerts->point = start - cross;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->animTexUVs[uv[1]];
      ++lVerts;

      lVerts->point = end - cross;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->animTexUVs[uv[2]];
      ++lVerts;

      lVerts->point = end + cross;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->animTexUVs[uv[3]];
      ++lVerts;
   }
   else
   {
      // Here and below, we copy UVs from particle datablock's texCoords
      lVerts->point = start + cross;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->texCoords[0];
      ++lVerts;

      lVerts->point = start - cross;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->texCoords[1];
      ++lVerts;

      lVerts->point = end - cross;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->texCoords[2];
      ++lVerts;

      lVerts->point = end + cross;
      lVerts->color = partCol;
      lVerts->texCoord = partdatablock->texCoords[3];
      ++lVerts;
   }
}

void BillboardParticleManager::AllocateParticles()
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
      BillboardParticle* store_block = new BillboardParticle[n_part_capacity];
      part_store.push_back(store_block);
      part_freelist = store_block;
      BillboardParticle* last_part = static_cast<BillboardParticle*>(part_freelist);
      BillboardParticle* part = last_part+1;
      for( S32 i = 1; i < n_part_capacity; i++, part++, last_part++ )
      {
         last_part->next = part;
      }
      store_block[n_part_capacity-1].next = NULL;
      part_list_head.next = NULL;
      n_parts = 0;
   }
}