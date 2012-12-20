//-----------------------------------------------------------------------------
// IPS Pro
// Copyright Lukas J�rgensen 2012 - FuzzyVoidStudio
// License:
// 'the product' refers to the IPS Pro.
// If you have not paid for the product you are not allowed to use it
//  - personally nor commercially.
// Assuming you have paid for the product you are allowed to
//  - include this in any commercial product as long as the IPS Pro
//  - source code is never released along with the commercial product.
// You may not resell or redistribute the product.
// You'll need one license for each man on your team, unless you fall under
//  - the indie license.
// You may not use the code to create new products in the same category.
//  - e.g. you may not use this code to construct a mathEmitter and sell that
//  - as a new product. Not if it is released for another GameEngine either.
// The indie license: As long as your products using IPS Pro has a revenue of
//  - less than 40.000$ annually you fall under this license.
// Email me at LukasPJ@FuzzyVoidStudio.com 
//  - if you have further questions regarding license
// http://fuzzyvoidstudio.com
//-----------------------------------------------------------------------------

#ifndef COLLISION_BEHAVIOUR_H
#define COLLISION_BEHAVIOUR_H

#ifndef PARTICLE_BEHAVIOUR_H
#include "particleBehaviour.h"
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
	bool preload(bool server, String &errorStr);
	static void initPersistFields();
	
	//--------------------------------------------
	// IParticleBehaviour
	//--------------------------------------------
	void updateParticle(ParticleEmitter* emitter, Particle* part, F32 time);
	U8 getPriority() { return 21; };
	virtual behaviourType getType() { return behaviourType::Velocity; };

	//--------------------------------------------
	// CollisionBehaviour
	//--------------------------------------------
	// Nothing yet
	CollisionBehaviour();

	DECLARE_CONOBJECT(CollisionBehaviour);
};

#endif // ATTRACTION_BEHAVIOUR_H