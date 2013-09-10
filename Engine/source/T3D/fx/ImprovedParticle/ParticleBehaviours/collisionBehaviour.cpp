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

#include "collisionBehaviour.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "../../particle.h"
#include "../../particleEmitter.h"

IMPLEMENT_CO_DATABLOCK_V1(CollisionBehaviour);

//---------------------------------------------------------------
// Constructor
//---------------------------------------------------------------
CollisionBehaviour::CollisionBehaviour()
{
	mCollisionMask = U32_MAX;
	mDampening = 0.8f;
}

//----------------------IParticleBehaviour-----------------------
//---------------------------------------------------------------
// updateParticle
// --Description
//--------------------------------------------------------------
void CollisionBehaviour::updateParticle(Particle* part, F32 time)
{
	RayInfo rInfo;
	bool accepted = false;
	int bounces = 0;
   Point3F curPos = part->pos;
	// Keep bouncing untill the particles doesn't hit anything
	// or untill they have bounced too many times.
	while(!accepted)
	{
		// Cast a ray from the particle and to their future position
		if(gClientContainer.castRay(curPos, curPos + part->vel * time, mCollisionMask, &rInfo))
		{
			// Project the velocity vector onto the normal of the collided surface
			Point3F proj = mDot(part->vel,rInfo.normal)/(rInfo.normal.len()*rInfo.normal.len())*rInfo.normal;
			// Get the vector between the velocity vector and the projected vector
			Point3F between = (part->vel - proj);
			// Add the vector between the velocity vector and the projected vector
			// to the velocity vector twice so we get the mirrored velocity vector
			// multiply it by -1 to flip it and then multiply it by 0.8 to add some 
			// dampening (TODO: move the dampening to a static field)
			VectorF mirrored = -(part->vel-(between*2)*(1.0f-(mDampening/100.0f)));
			//mirrored.normalize();
			// Add the mirrored vector to the velocity and bounce the particle again
			part->vel = mirrored * (part->pos - rInfo.point).len() * time;
			curPos = rInfo.point;
		}
		else
			accepted = true;
		bounces++;
		if(bounces >= MAX_BOUNCES)
			accepted = true;
	}
	// Set the velocity to be the last collided position
	part->vel = part->pos + (part->pos - (curPos + (part->vel * time)));
}

//----------------------- SimDataBlock -------------------------
//---------------------------------------------------------------
// InitPersistFields
//--------------------------------------------------------------
void CollisionBehaviour::initPersistFields()
{

	addGroup("CollisionBehaviour");

	addField("CollisionMask", TypeS32, Offset(mCollisionMask, CollisionBehaviour), "The typemask to collide against.");

	addField("Dampening", TypeF32, Offset(mDampening, CollisionBehaviour), 
		"Amount of dampening to apply on each collision."
		"A value of 100 brings the particles to a complete stop, a value of 0 does not apply any dampening.");

	endGroup("CollisionBehaviour");

	Parent::initPersistFields();
}

//---------------------------------------------------------------
// OnAdd
//---------------------------------------------------------------
bool CollisionBehaviour::onAdd()
{
	if( !Parent::onAdd() )
		return false;
	return true;
}

//---------------------------------------------------------------
// PackData
//--------------------------------------------------------------
void CollisionBehaviour::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->writeInt(mCollisionMask, 32);
	stream->writeInt(mDampening * 1000, 16);
}

//---------------------------------------------------------------
// UnpackData
//--------------------------------------------------------------
void CollisionBehaviour::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	mCollisionMask = stream->readInt(32);
	mDampening = stream->readInt(16) / 1000.0f;
}

//---------------------------------------------------------------
// Preload
//--------------------------------------------------------------
bool CollisionBehaviour::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;
	// Verify variables

	return true;
}