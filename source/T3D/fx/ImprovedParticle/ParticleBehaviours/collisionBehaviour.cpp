//-----------------------------------------------------------------------------
// IPS Pro
// Copyright Lukas Jørgensen 2012 - FuzzyVoidStudio
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

}

//----------------------IParticleBehaviour-----------------------
//---------------------------------------------------------------
// updateParticle
// --Description
//--------------------------------------------------------------
void CollisionBehaviour::updateParticle(ParticleEmitter* emitter, Particle* part, F32 time)
{
	RayInfo rInfo;
	if(gClientContainer.castRay(part->pos, part->pos + part->vel * time, TerrainObjectType | InteriorObjectType | VehicleObjectType | PlayerObjectType | StaticShapeObjectType, &rInfo))
	{
		Point3F proj = mDot(part->vel,rInfo.normal)/(rInfo.normal.len()*rInfo.normal.len())*rInfo.normal;
		Point3F between = (part->vel - proj);
		part->vel = -(part->vel-(between*2)*0.8);
	}
}

//----------------------- SimDataBlock -------------------------
//---------------------------------------------------------------
// InitPersistFields
//--------------------------------------------------------------
void CollisionBehaviour::initPersistFields()
{
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
}

//---------------------------------------------------------------
// UnpackData
//--------------------------------------------------------------
void CollisionBehaviour::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
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