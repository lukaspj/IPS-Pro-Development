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

#include "sphereEmitter.h"

#include "console\engineAPI.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

#include "console/typeValidators.h"

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

   ejectionOffsetVariance = 0.0f;
}

FRangeValidator ejectionVarFValidator(0.f, 655.35f);

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void SphereEmitterData::initPersistFields()
{
	addGroup( "SphereEmitterData" );

   addFieldV( "ejectionOffsetVariance", TYPEID< F32 >(), Offset(ejectionOffsetVariance, SphereEmitterData), &ejectionVarFValidator,
      "Distance Padding along ejection Z axis from which to eject particles." );

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

   if( stream->writeFlag( ejectionOffsetVariance != 0.0f ) )
      stream->writeInt((S32)(ejectionOffsetVariance * 100), 16);
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

   if( stream->readFlag() )
      ejectionOffsetVariance = stream->readInt(16) / 100.0f;
   else
      ejectionOffsetVariance = 0.0f;
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
	const Point3F& axisx, 
	const MatrixF &trans)
{
	SphereEmitterData* DataBlock = getDataBlock();
   Particle* pNew = mParticleManager->AddParticle();
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

	pNew->pos = pos + (ejectionAxis * (DataBlock->ejectionOffset + DataBlock->ejectionOffsetVariance* gRandGen.randF()));
	pNew->relPos = pNew->pos - pos;
	pNew->vel = ejectionAxis * initialVel;
	pNew->orientDir = ejectionAxis;
	pNew->acc.set(0, 0, 0);
	pNew->currentAge = 0;

	// Choose a new particle datablack randomly from the list
	U32 dBlockIndex = gRandGen.randI() % DataBlock->particleDataBlocks.size();
   mParticleManager->initializeParticle(pNew, vel, mDataBlock->particleDataBlocks[dBlockIndex]);
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
	SphereEmitterData* DataBlock = getDataBlock();
   Particle* pNew = mParticleManager->AddParticle();

	Point3F ejectionAxis = axis;
	F32 ref;
	F32 phi;
	F32 theta;

	if(standAloneEmitter)
	{
		theta = (sa_thetaMax - sa_thetaMin) * gRandGen.randF() +
			sa_thetaMin;
		ref  = (F32(mInternalClock) / 1000.0) * sa_phiReferenceVel;
		phi  = ref + gRandGen.randF() * sa_phiVariance;
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
	if(standAloneEmitter)
	{
		initialVel = sa_ejectionVelocity;
		initialVel    += (sa_velocityVariance * 2.0f * gRandGen.randF()) - sa_velocityVariance;
	}
	else
	{
		initialVel = DataBlock->ejectionVelocity;
		initialVel    += (DataBlock->velocityVariance * 2.0f * gRandGen.randF()) - DataBlock->velocityVariance;
	}
	if(standAloneEmitter)
		pNew->pos = pos + (ejectionAxis * sa_ejectionOffset);
	else
		pNew->pos = pos + (ejectionAxis * (DataBlock->ejectionOffset + DataBlock->ejectionOffsetVariance* gRandGen.randF()));
	pNew->relPos = pNew->pos - pos;
	pNew->vel = ejectionAxis * initialVel;
	pNew->orientDir = ejectionAxis;
	pNew->acc.set(0, 0, 0);
	pNew->currentAge = 0;

	// Choose a new particle datablack randomly from the list
	U32 dBlockIndex = gRandGen.randI() % DataBlock->particleDataBlocks.size();
   mParticleManager->initializeParticle(pNew, vel, mDataBlock->particleDataBlocks[dBlockIndex]);
	updateKeyData( pNew );
	return true;
}

ParticleEmitter* SphereEmitterData::createEmitter() { return new SphereEmitter; }

DefineEngineMethod(SphereEmitterData, reload, void,(),,
				   "Reloads the ParticleData datablocks and other fields used by this emitter.\n"
				   "@tsexample\n"
				   "// Get the editor's current particle emitter\n"
				   "%emitter = PE_EmitterEditor.currEmitter\n\n"
				   "// Change a field value\n"
				   "%emitter.setFieldValue( %propertyField, %value );\n\n"
				   "// Reload this emitter\n"
				   "%emitter.reload();\n"
				   "@endtsexample\n")
{
	object->reload();
}