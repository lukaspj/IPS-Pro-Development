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

#ifndef PARTICLE_MANAGER_H_
#define PARTICLE_MANAGER_H_

#include "console\consoleTypes.h"

#include "T3D\fx\particleEmitter.h"
#include "../IPSCore.h"

class ParticleManager
{
public:
   ParticleManager();
   virtual ~ParticleManager();

   virtual Particle* AddParticle() = 0;
   //virtual void DeleteParticle();
   virtual void DeleteDeadParticles(U32 numMSToUpdate = 0);
   virtual void RenderParticles( SceneRenderState* state ) = 0;
   virtual void UpdateKeyData( Particle* part, U32 milliseconds = 0 );
   virtual void AllocateParticles() = 0;
   // move this procedure to Particle
   virtual void initializeParticle(Particle*, const Point3F&, ParticleData*);

	ColorF getCollectiveColor();

   IParticleEmitter* mEmitter;
   IParticleEmitterData* mEmitterDatablock;

	//   These members are for implementing a link-list of the active emitter 
	//   particles. Member part_store contains blocks of particles that can be
	//   chained in a link-list. Usually the first part_store block is large
	//   enough to contain all the particles but it can be expanded in emergency
	//   circumstances.
	Vector <Particle*> part_store;
	Particle*  part_freelist;
	Particle   part_list_head;
	S32        n_part_capacity;
	S32        n_parts;
	S32       mCurBuffSize;
};

#endif // PARTICLE_MANAGER_H_
