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

#ifndef POINTLIGHT_PARTICLE_H_
#define POINTLIGHT_PARTICLE_H_

#include "T3D\fx\particle.h"

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif

#include "pointlightParticleManager.h"
#include "T3D\pointLight.h"
//#include "core\util\str.h"

class PointLightParticleData : public ParticleData
{
   typedef ParticleData Parent;
public:
   PointLightParticleData();
   ~PointLightParticleData();

   static void initPersistFields();
   void packData(BitStream* stream);
   void unpackData(BitStream* stream);
   bool onAdd();
   bool preload(bool server, String &errorStr);
   bool reload(char errorBuffer[256]);
   void preloadVector(Vector<ParticleData*> particleDataBlocks, U32 mask, ParticleEmitterData* emitterData);

   static bool protectedSetTimes( void *object, const char *index, const char *data );
   static bool protectedSetRanges( void *object, const char *index, const char *data );
   
   virtual PointLightParticleManager* CreateManager() { return new PointLightParticleManager; };

   // Fields -----
   bool mUseParticleColors;
   ColorF mColor;
   ColorF mColorVariance;

   F32 mBrightness;
   F32 mFlareScale;

   LightFlareData* mFlareData;
   
   ColorF colors[ PDC_NUM_KEYS ];
   F32 ranges[ PDC_NUM_KEYS ];

   // Declares -----
   DECLARE_CONOBJECT(PointLightParticleData);
   DECLARE_CALLBACK(S32,CreateSceneObject,());
};

struct PointLightParticle : Particle
{
   SimObjectPtr<PointLight> object;
   F32 range;
};


#endif // POINTLIGHT_PARTICLE_H_
