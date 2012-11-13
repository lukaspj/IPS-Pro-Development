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

#include "groundEmitter.h"
#include "groundEmitterNode.h"
#include "IPSCore.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "terrain\terrData.h"

IMPLEMENT_CO_DATABLOCK_V1(GroundEmitterData);
IMPLEMENT_CONOBJECT(GroundEmitter);

ConsoleDocClass( GroundEmitter,
	"@brief This object is responsible for spawning particles.\n\n"

	"@note This class is not normally instantiated directly - to place a simple "
	"particle emitting object in the scene, use a ParticleEmitterNode instead.\n\n"

	"This class is the main interface for creating particles - though it is "
	"usually only accessed from within another object like ParticleEmitterNode "
	"or WheeledVehicle. If using this object class (via C++) directly, be aware "
	"that it does <b>not</b> track changes in source axis or velocity over the "
	"course of a single update, so emitParticles should be called at a fairly "
	"fine grain.  The emitter will potentially track the last particle to be "
	"created into the next call to this function in order to create a uniformly "
	"random time distribution of the particles.\n\n"

	"If the object to which the emitter is attached is in motion, it should try "
	"to ensure that for call (n+1) to this function, start is equal to the end "
	"from call (n). This will ensure a uniform spatial distribution.\n\n"

	"@ingroup FX\n"
	"@see ParticleEmitterData\n"
	"@see ParticleEmitterNode\n"
	);

ConsoleDocClass( GroundEmitterData,
	"@brief Defines particle emission properties such as ejection angle, period "
	"and velocity for a ParticleEmitter.\n\n"

	"@tsexample\n"
	"datablock ParticleEmitterData( GrenadeExpDustEmitter )\n"
	"{\n"
	"   ejectionPeriodMS = 1;\n"
	"   periodVarianceMS = 0;\n"
	"   ejectionVelocity = 15;\n"
	"   velocityVariance = 0.0;\n"
	"   ejectionOffset = 0.0;\n"
	"   thetaMin = 85;\n"
	"   thetaMax = 85;\n"
	"   phiReferenceVel = 0;\n"
	"   phiVariance = 360;\n"
	"   overrideAdvance = false;\n"
	"   lifetimeMS = 200;\n"
	"   particles = \"GrenadeExpDust\";\n"
	"};\n"
	"@endtsexample\n\n"

	"@ingroup FX\n"
	"@see ParticleEmitter\n"
	"@see ParticleData\n"
	"@see ParticleEmitterNode\n"
	);

//-----------------------------------------------------------------------------
// GraphEmitterData
//-----------------------------------------------------------------------------
GroundEmitterData::GroundEmitterData()
{
	radius = 5;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GroundEmitterData::initPersistFields()
{
	addGroup( "GroundEmitterData" );

	addField( "radius", TYPEID< F32 >(), Offset(radius, GroundEmitterData),
		"Distance along ejection Z axis from which to eject particles." );

	endGroup( "GroundEmitterData" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void GroundEmitterData::packData(BitStream* stream)
{
	if( stream->writeFlag( radius != 5 ) )
		stream->writeInt((S32)(radius * 100), 16);
	Parent::packData(stream);
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void GroundEmitterData::unpackData(BitStream* stream)
{
	if( stream->readFlag() )
		radius = stream->readInt(16) / 100.0f;
	else
		radius = 5;
	Parent::unpackData(stream);
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool GroundEmitterData::onAdd()
{
	if( Parent::onAdd() == false )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// SphereEmitter
//-----------------------------------------------------------------------------
GroundEmitter::GroundEmitter()
{
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GroundEmitter::initPersistFields()
{
	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool GroundEmitter::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	Parent::onNewDataBlock(dptr, reload);
	GroundEmitterData* DataBlock = getDataBlock();
	if ( !DataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// addParticle
//-----------------------------------------------------------------------------
bool GroundEmitter::addParticle(const Point3F& pos,
	const Point3F& axis,
	const Point3F& vel,
	const Point3F& axisx)
{
	GroundEmitterData* DataBlock = getDataBlock();
	PROFILE_SCOPE(GroundEmitAddPart);

	parentNodePos = pos;
	U32 rand = gRandGen.randI();
	F32 relx = cos((F32)rand) * gRandGen.randF() * DataBlock->radius;
	F32 rely = sin((F32)rand) * gRandGen.randF() * DataBlock->radius;
	F32 x = parentNodePos.x + relx;
	F32 y = parentNodePos.y + rely;
	F32 z;
	StringTableEntry mat;
	Point3F normal;
	if(GetTerrainHeightAndMat(x,y,z,mat,normal))
	{
		// Need to add layers to the GroundEmitterData
		/*for(int i = 0; i < 5; i++)
			if(nodeDat->layers[i] == mat)
				return false;*/
		n_parts++;
		if (n_parts > n_part_capacity || n_parts > DataBlock->partListInitSize)
		{
			// In an emergency we allocate additional particles in blocks of 16.
			// This should happen rarely.
			Particle* store_block = new Particle[16];
			part_store.push_back(store_block);
			n_part_capacity += 16;
			for (S32 i = 0; i < 16; i++)
			{
				store_block[i].next = part_freelist;
				part_freelist = &store_block[i];
			}
			DataBlock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
		}
		Particle* pNew = part_freelist;
		part_freelist = pNew->next;
		pNew->next = part_list_head.next;
		part_list_head.next = pNew;

		F32 initialVel;
		initialVel = mDataBlock->ejectionVelocity;
		initialVel    += (mDataBlock->velocityVariance * 2.0f * gRandGen.randF()) - mDataBlock->velocityVariance;

		pNew->pos = Point3F(x,y,z) + normal * DataBlock->ejectionOffset;


		// Set the relative position for later use.
		pNew->relPos = Point3F(relx,rely,0+DataBlock->ejectionOffset);
		// Velocity is based on the normal of the vertex
		pNew->vel = Point3F(0,0,1) * initialVel;
		pNew->orientDir = Point3F(0,0,1);

		pNew->acc.set(0, 0, 0);
		pNew->currentAge = 0;

		// Choose a new particle datablack randomly from the list
		U32 dBlockIndex = gRandGen.randI() % DataBlock->particleDataBlocks.size();
		DataBlock->particleDataBlocks[dBlockIndex]->initializeParticle(pNew, Point3F(0,0,DataBlock->ejectionVelocity));
		updateKeyData( pNew );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// addParticle - Node
//-----------------------------------------------------------------------------
bool GroundEmitter::addParticle(const Point3F& pos,
	const Point3F& axis,
	const Point3F& vel,
	const Point3F& axisx,
	ParticleEmitterNode* pnodeDat)
{
	PROFILE_SCOPE(GroundEmitAddPart);
	IPSBenchmarkBegin;
	GroundEmitterNode* nodeDat = static_cast<GroundEmitterNode*>(pnodeDat);
	GroundEmitterData* DataBlock = getDataBlock();

	parentNodePos = nodeDat->getPosition();
	U32 rand = gRandGen.randI();
	F32 relx = cos((F32)rand) * gRandGen.randF() * nodeDat->sa_radius;
	F32 rely = sin((F32)rand) * gRandGen.randF() * nodeDat->sa_radius;
	F32 x = parentNodePos.x + relx;
	F32 y = parentNodePos.y + rely;
	F32 z;
	StringTableEntry mat;
	Point3F normal;
	if(GetTerrainHeightAndMat(x,y,z,mat,normal))
	{
		for(int i = 0; i < 5; i++)
			if(nodeDat->layers[i] == mat)
				return false;
		n_parts++;
		if (n_parts > n_part_capacity || n_parts > DataBlock->partListInitSize)
		{
			// In an emergency we allocate additional particles in blocks of 16.
			// This should happen rarely.
			Particle* store_block = new Particle[16];
			part_store.push_back(store_block);
			n_part_capacity += 16;
			for (S32 i = 0; i < 16; i++)
			{
				store_block[i].next = part_freelist;
				part_freelist = &store_block[i];
			}
			DataBlock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
		}
		Particle* pNew = part_freelist;
		part_freelist = pNew->next;
		pNew->next = part_list_head.next;
		part_list_head.next = pNew;

		F32 initialVel;
		// If it is a standAloneEmitter, then we want it to use the sa values from the node
		if(nodeDat->standAloneEmitter)
		{
			initialVel = nodeDat->sa_ejectionVelocity;
			initialVel    += (nodeDat->sa_velocityVariance * 2.0f * gRandGen.randF()) - nodeDat->sa_velocityVariance;
		}
		else
		{
			initialVel = DataBlock->ejectionVelocity;
			initialVel    += (DataBlock->velocityVariance * 2.0f * gRandGen.randF()) - DataBlock->velocityVariance;
		}
		pNew->pos = Point3F(x,y,z) + normal*nodeDat->sa_ejectionOffset;


		// Set the relative position for later use.
		pNew->relPos = Point3F(relx,rely,0+nodeDat->sa_ejectionOffset);
		// Velocity is based on the normal of the vertex
		pNew->vel = Point3F(0,0,1) * initialVel;
		pNew->orientDir = Point3F(0,0,1);

		pNew->acc.set(0, 0, 0);
		pNew->currentAge = 0;

		// Choose a new particle datablack randomly from the list
		U32 dBlockIndex = gRandGen.randI() % DataBlock->particleDataBlocks.size();
		DataBlock->particleDataBlocks[dBlockIndex]->initializeParticle(pNew, Point3F(0,0,nodeDat->sa_ejectionVelocity));
		updateKeyData( pNew );
		return true;
	}
	
	IPSBenchmarkEnd("----Graph---- addPart");
	return false;
}

ParticleEmitter* GroundEmitterData::createEmitter() { return new GroundEmitter; }


bool GroundEmitter::GetTerrainHeightAndMat(const F32 &x, const F32 &y, F32 &height, StringTableEntry &mat, Point3F &normal)
{
	PROFILE_SCOPE(GroundEmitGetTerHeight);
	Point3F startPnt( x, y, 10000.0f );
	Point3F endPnt( x, y, -10000.0f );

	RayInfo ri;
	bool hit;         

	hit = gClientContainer.castRay(startPnt, endPnt, TerrainObjectType, &ri);   

	if ( hit ){
		height = ri.point.z;
		normal = ri.normal;
		TerrainBlock* TB = dynamic_cast< TerrainBlock* > ( ri.object );  
		Point2I gridPos = TB->getGridPos( ri.point );
		U8 layer = TB->getFile()->getLayerIndex( gridPos.x, gridPos.y );
		mat = TB->getMaterialName(layer);
	}

	return hit;
}