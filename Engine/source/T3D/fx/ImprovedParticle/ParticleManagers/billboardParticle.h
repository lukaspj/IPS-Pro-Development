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

#ifndef BILLBOARD_PARTICLE_H_
#define BILLBOARD_PARTICLE_H_

#include "T3D\fx\particle.h"

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif

#include "billboardParticleManager.h"
//#include "core\util\str.h"

class BillboardParticleData : public ParticleData
{
   typedef ParticleData Parent;
public:
   BillboardParticleData();
   ~BillboardParticleData();

   static void initPersistFields();
   void packData(BitStream* stream);
   void unpackData(BitStream* stream);
   bool onAdd();
   bool preload(bool server, String &errorStr);
   bool reload(char errorBuffer[256]);
   void preloadVector(Vector<ParticleData*> particleDataBlocks, U32 mask, ParticleEmitterData* emitterData);

   Point2F*          animTexUVs;
   Point2F           texCoords[4];			  // default: {{0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0}} 
   Point2I           animTexTiling;
   StringTableEntry  animTexFramesString;
   Vector<U8>        animTexFrames;
   StringTableEntry  hResTextureName;        ///< Emitter texture file to override particle textures
   GFXTexHandle      hResTextureHandle;      ///< Emitter texture handle from txrName
   StringTableEntry  mResTextureName;		  ///< Emitter texture file to override particle textures
   GFXTexHandle      mResTextureHandle;	  ///< Emitter texture handle from txrName
   StringTableEntry  lResTextureName;		  ///< Emitter texture file to override particle textures
   GFXTexHandle      lResTextureHandle;	  ///< Emitter texture handle from txrName
   
   bool  useInvAlpha;

   bool  animateTexture;
   U32   numFrames;
   U32   framesPerSec;

   F32   spinSpeed;        // degrees per second
   F32   spinRandomMin;
   F32   spinRandomMax;

   ColorF colors[ PDC_NUM_KEYS ];

   static bool protectedSetTimes( void *object, const char *index, const char *data );
   
   virtual BillboardParticleManager* CreateManager() { return new BillboardParticleManager; };

   DECLARE_CONOBJECT(BillboardParticleData);
};

struct BillboardParticle : Particle
{
public:

   // are these necessary to store here? - they are interpolated in real time
   ColorF           color;

   F32              spinSpeed;
};


#endif // BILLBOARD_PARTICLE_H_
