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

#ifndef PARTICLE_BEHAVIOUR_H
#define PARTICLE_BEHAVIOUR_H

#include "../../particle.h"
#include "console\consoleTypes.h"
#include "console\simDatablock.h"

class MeshEmitter;
class ParticleEmitter;

namespace behaviourClass{
	enum behaviourClass{
		AttractionBehaviour = 0,
		CollisionBehaviour,
		StickyBehaviour,
		Error
	};
}

namespace behaviourType{
	enum behaviourType{
		Acceleration,
		Position,
		Velocity,
		Error
	};
}

class IParticleBehaviour : public SimDataBlock
{
	typedef SimDataBlock Parent;
public:

	virtual U8 getPriority() { return 0; };
	virtual void updateParticle(ParticleEmitter* emitter, Particle* part, F32 time);
	virtual void updateParticle(MeshEmitter* emitter, Particle* part, F32 time);
	virtual behaviourType::behaviourType getType() { return behaviourType::Error; };
	bool operator<(IParticleBehaviour IPB) { return getPriority() < IPB.getPriority(); };
	virtual behaviourClass::behaviourClass getBehaviourType() { return behaviourClass::Error; };

	//--------------------------------------------
	// SimDataBlock
	//--------------------------------------------
	virtual bool onAdd();
	virtual void packData(BitStream* stream);
	virtual void unpackData(BitStream* stream);
   virtual void packUpdate(BitStream*, NetConnection*);
   virtual void unpackUpdate(BitStream*, NetConnection*);
	virtual bool preload(bool server, String &errorStr);
	static void initPersistFields();
	DECLARE_CONOBJECT(IParticleBehaviour);
};

#endif // PARTICLE_BEHAVIOUR_H
