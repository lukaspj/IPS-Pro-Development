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

#ifndef ATTRACTION_BEHAVIOUR_H
#define ATTRACTION_BEHAVIOUR_H

#ifndef PARTICLE_BEHAVIOUR_H
#include "particleBehaviour.h"
#endif

#ifndef _H_PARTICLE_EMITTER
#include "../../particleEmitter.h"
#endif

#ifndef _H_MESH_EMITTER
#include "../meshEmitter.h"
#endif

#ifndef attrobjectCount
#define attrobjectCount 2
#endif

class AttractionBehaviour : public IParticleBehaviour
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
	void updateParticle(ParticleEmitter* emitter, Particle* part, F32 time) { updateParticle(part, time); };
	void updateParticle(MeshEmitter* emitter, Particle* part, F32 time) { updateParticle(part, time); };
	void updateParticle(Particle* part, F32 time);
	U8 getPriority() { return 20; };
	virtual behaviourType::behaviourType getType() { return behaviourType::Acceleration; };
	virtual behaviourClass::behaviourClass getBehaviourType() { return behaviourClass::AttractionBehaviour; };
	
	//--------------------------------------------
	// AttractionBehaviour
	//--------------------------------------------
	AttractionBehaviour();
	F32					attractionrange;						///< Range of influence, any objects further away than this length will not attract or repulse the particles.
	F32					Amount[attrobjectCount];				///< Amount of attraction
	StringTableEntry	Attraction_offset[attrobjectCount];		///< The offset relative to the target objects position to be attracted to
	S32					AttractionMode[attrobjectCount];		///< How the objects should interact with the associated objects
	StringTableEntry	attractedObjectID[attrobjectCount];		///< The targets that the particles are attracted to
	
	enum EnumAttractionMode{
		none = 0,
		attract = 1,
		repulse = 2
	};

	DECLARE_CONOBJECT(AttractionBehaviour);
};

#endif // ATTRACTION_BEHAVIOUR_H
