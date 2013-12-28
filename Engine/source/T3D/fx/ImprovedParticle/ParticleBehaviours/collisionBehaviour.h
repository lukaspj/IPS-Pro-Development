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

#ifndef COLLISION_BEHAVIOUR_H
#define COLLISION_BEHAVIOUR_H

#ifndef PARTICLE_BEHAVIOUR_H
#include "particleBehaviour.h"
#endif

#ifndef MAX_BOUNCES
#	define MAX_BOUNCES (U8)5
#endif

class CollisionBehaviour : public IParticleBehaviour
{
	typedef IParticleBehaviour Parent;
public:
	//--------------------------------------------
	// SimDataBlock
	//--------------------------------------------
	bool onAdd();
	void packData(BitStream*);
	void unpackData(BitStream*);
   virtual void packUpdate(BitStream* stream, NetConnection*) { packData(stream); };
   virtual void unpackUpdate(BitStream* stream, NetConnection*) { unpackData(stream); };
	bool preload(bool server, String &errorStr);
	static void initPersistFields();
	
	//--------------------------------------------
	// IParticleBehaviour
	//--------------------------------------------
	void updateParticle(ParticleEmitter* emitter, Particle* part, F32 time) { updateParticle(part, time); };
	void updateParticle(MeshEmitter* emitter, Particle* part, F32 time) { updateParticle(part, time); };
	void updateParticle(Particle* part, F32 time);
	U8 getPriority() { return 21; };
	virtual behaviourType::behaviourType getType() { return behaviourType::Velocity; };

	//--------------------------------------------
	// CollisionBehaviour
	//--------------------------------------------
	// Nothing yet
	CollisionBehaviour();
	
	U32 mCollisionMask;
	F32 mDampening;

	DECLARE_CONOBJECT(CollisionBehaviour);
};

#endif // ATTRACTION_BEHAVIOUR_H
