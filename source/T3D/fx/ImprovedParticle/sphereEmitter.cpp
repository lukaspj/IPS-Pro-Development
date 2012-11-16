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

#include "sphereEmitter.h"
#include "sphereEmitterNode.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(SphereEmitterData);
IMPLEMENT_CONOBJECT(SphereEmitter);

ConsoleDocClass( SphereEmitter,
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

ConsoleDocClass( SphereEmitterData,
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

static const float sgDefaultEjectionOffset = 0.f;
static const float sgDefaultPhiReferenceVel = 0.f;
static const float sgDefaultPhiVariance = 360.f;

//-----------------------------------------------------------------------------
// SphereEmitterData
//-----------------------------------------------------------------------------
SphereEmitterData::SphereEmitterData()
{
	thetaMin         = 0.0f;   // All heights
	thetaMax         = 90.0f;

	phiReferenceVel  = sgDefaultPhiReferenceVel;   // All directions
	phiVariance      = sgDefaultPhiVariance;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void SphereEmitterData::initPersistFields()
{
	addGroup( "SphereEmitterData" );

	addField( "thetaMin", TYPEID< F32 >(), Offset(thetaMin, SphereEmitterData),
		"Minimum angle, from the horizontal plane, to eject from." );

	addField( "thetaMax", TYPEID< F32 >(), Offset(thetaMax, SphereEmitterData),
		"Maximum angle, from the horizontal plane, to eject particles from." );

	addField( "phiReferenceVel", TYPEID< F32 >(), Offset(phiReferenceVel, SphereEmitterData),
		"Reference angle, from the vertical plane, to eject particles from." );

	addField( "phiVariance", TYPEID< F32 >(), Offset(phiVariance, SphereEmitterData),
		"Variance from the reference angle, from 0 - 360." );

	endGroup( "SphereEmitterData" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void SphereEmitterData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->writeRangedU32((U32)thetaMin, 0, 180);
	stream->writeRangedU32((U32)thetaMax, 0, 180);
	if( stream->writeFlag( phiReferenceVel != sgDefaultPhiReferenceVel ) )
		stream->writeRangedU32((U32)phiReferenceVel, 0, 360);
	if( stream->writeFlag( phiVariance != sgDefaultPhiVariance ) )
		stream->writeRangedU32((U32)phiVariance, 0, 360);
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void SphereEmitterData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	thetaMin = (F32)stream->readRangedU32(0, 180);
	thetaMax = (F32)stream->readRangedU32(0, 180);
	if( stream->readFlag() )
		phiReferenceVel = (F32)stream->readRangedU32(0, 360);
	else
		phiReferenceVel = sgDefaultPhiReferenceVel;

	if( stream->readFlag() )
		phiVariance = (F32)stream->readRangedU32(0, 360);
	else
		phiVariance = sgDefaultPhiVariance;
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool SphereEmitterData::onAdd()
{
	if( thetaMin < 0.0f )
	{
		Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) thetaMin < 0.0", getName());
		thetaMin = 0.0f;
	}
	if( thetaMax > 180.0f )
	{
		Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) thetaMax > 180.0", getName());
		thetaMax = 180.0f;
	}
	if( thetaMin > thetaMax )
	{
		Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) thetaMin > thetaMax", getName());
		thetaMin = thetaMax;
	}
	if( phiVariance < 0.0f || phiVariance > 360.0f )
	{
		Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) invalid phiVariance", getName());
		phiVariance = phiVariance < 0.0f ? 0.0f : 360.0f;
	}


	if( Parent::onAdd() == false )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// SphereEmitter
//-----------------------------------------------------------------------------
SphereEmitter::SphereEmitter()
{
}

//-----------------------------------------------------------------------------
// addParticle
//-----------------------------------------------------------------------------
bool SphereEmitter::addParticle(const Point3F& pos,
	const Point3F& axis,
	const Point3F& vel,
	const Point3F& axisx)
{
	SphereEmitterData* DataBlock = getDataBlock();
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

	Point3F ejectionAxis = axis;

	F32 theta = (DataBlock->thetaMax - DataBlock->thetaMin) * gRandGen.randF() +
		DataBlock->thetaMin;

	F32 ref  = (F32(mInternalClock) / 1000.0) * DataBlock->phiReferenceVel;
	F32 phi  = ref + gRandGen.randF() * DataBlock->phiVariance;

	// Both phi and theta are in degs.  Create axis angles out of them, and create the
	//  appropriate rotation matrix...
	AngAxisF thetaRot(axisx, theta * (M_PI / 180.0));
	AngAxisF phiRot(axis,    phi   * (M_PI / 180.0));

	MatrixF temp(true);
	thetaRot.setMatrix(&temp);
	temp.mulP(ejectionAxis);
	phiRot.setMatrix(&temp);
	temp.mulP(ejectionAxis);

	F32 initialVel = DataBlock->ejectionVelocity;
	initialVel    += (DataBlock->velocityVariance * 2.0f * gRandGen.randF()) - DataBlock->velocityVariance;

	pNew->pos = pos + (ejectionAxis * DataBlock->ejectionOffset);
	pNew->vel = ejectionAxis * initialVel;
	pNew->orientDir = ejectionAxis;
	pNew->acc.set(0, 0, 0);
	pNew->currentAge = 0;

	// Choose a new particle datablack randomly from the list
	U32 dBlockIndex = gRandGen.randI() % DataBlock->particleDataBlocks.size();
	DataBlock->particleDataBlocks[dBlockIndex]->initializeParticle(pNew, vel);
	updateKeyData( pNew );
	return true;
}

//-----------------------------------------------------------------------------
// addParticle - Node
//-----------------------------------------------------------------------------
bool SphereEmitter::addParticle(const Point3F& pos,
	const Point3F& axis,
	const Point3F& vel,
	const Point3F& axisx,
	ParticleEmitterNode* pnodeDat)
{
	SphereEmitterNode* nodeDat = static_cast<SphereEmitterNode*>(pnodeDat);
	n_parts++;
	SphereEmitterData* DataBlock = getDataBlock();
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

	Point3F ejectionAxis = axis;
	F32 ref;
	F32 phi;
	F32 theta;

	if(nodeDat->standAloneEmitter)
	{
		theta = (nodeDat->sa_thetaMax - nodeDat->sa_thetaMin) * gRandGen.randF() +
			nodeDat->sa_thetaMin;
		ref  = (F32(mInternalClock) / 1000.0) * nodeDat->sa_phiReferenceVel;
		phi  = ref + gRandGen.randF() * nodeDat->sa_phiVariance;
	}
	else{
		theta = (DataBlock->thetaMax - DataBlock->thetaMin) * gRandGen.randF() +
			DataBlock->thetaMin;

		ref  = (F32(mInternalClock) / 1000.0) * DataBlock->phiReferenceVel;
		phi  = ref + gRandGen.randF() * DataBlock->phiVariance;
	}

	// Both phi and theta are in degs.  Create axis angles out of them, and create the
	//  appropriate rotation matrix...
	AngAxisF thetaRot(axisx, theta * (M_PI / 180.0));
	AngAxisF phiRot(axis,    phi   * (M_PI / 180.0));

	MatrixF temp(true);
	thetaRot.setMatrix(&temp);
	temp.mulP(ejectionAxis);
	phiRot.setMatrix(&temp);
	temp.mulP(ejectionAxis);
	F32 initialVel;
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
	if(nodeDat->standAloneEmitter)
		pNew->pos = pos + (ejectionAxis * nodeDat->sa_ejectionOffset);
	else
		pNew->pos = pos + (ejectionAxis * DataBlock->ejectionOffset);
	pNew->vel = ejectionAxis * initialVel;
	pNew->orientDir = ejectionAxis;
	pNew->acc.set(0, 0, 0);
	pNew->currentAge = 0;

	// Choose a new particle datablack randomly from the list
	U32 dBlockIndex = gRandGen.randI() % DataBlock->particleDataBlocks.size();
	DataBlock->particleDataBlocks[dBlockIndex]->initializeParticle(pNew, vel);
	updateKeyData( pNew );
	return true;
}

ParticleEmitter* SphereEmitterData::createEmitter() { return new SphereEmitter; }