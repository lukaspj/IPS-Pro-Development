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

#include "MaskEmitter.h"
#include "MaskEmitterNode.h"
#include "IPSCore.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(MaskEmitterData);
IMPLEMENT_CONOBJECT(MaskEmitter);

ConsoleDocClass( MaskEmitter,
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

ConsoleDocClass( MaskEmitterData,
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
// MaskEmitterData
//-----------------------------------------------------------------------------
MaskEmitterData::MaskEmitterData()
{
	Threshold_min = 0;
	Threshold_max = 255;
	radius = 1;
	pMask = NULL;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void MaskEmitterData::initPersistFields()
{
	addGroup( "MaskEmitter" );

	addField( "radius", TYPEID< F32 >(), Offset(radius, MaskEmitterData),
		"Size of the inbound circle of the emission mask." );

	addField( "Threshold_min", TypeS8, Offset(Threshold_min, MaskEmitterData),
		"Minimum alpha value of the pixels in the mask to eject particles on." );

	addField( "Threshold_max", TypeS8, Offset(Threshold_max, MaskEmitterData),
		"Maximum alpha value of the pixels in the mask to eject particles on." );

	addField( "PixelMask", TYPEID<PixelMask>(), Offset(pMask, MaskEmitterData), "");

	endGroup( "MaskEmitter" );
	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void MaskEmitterData::packData(BitStream* stream)
{
	Parent::packData(stream);

	if( stream->writeFlag( radius != 1 ) )
		stream->writeInt((S32)(radius * 100), 16);

	stream->writeInt(Threshold_min, 10);

	stream->writeInt(Threshold_max, 10);

	if( stream->writeFlag(pMask != NULL) )
	{
		stream->writeInt(pMask->getId(), 32);
	}
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void MaskEmitterData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
	if(stream->readFlag())
		radius = stream->readInt(16) / 100;

	Threshold_min = stream->readInt(10);
	Threshold_max = stream->readInt(10);

	if(stream->readFlag())
		pMask = dynamic_cast<PixelMask*>(Sim::findObject(stream->readInt(32)));
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool MaskEmitterData::onAdd()
{
	if( Parent::onAdd() == false )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// SphereEmitter
//-----------------------------------------------------------------------------
MaskEmitter::MaskEmitter()
{	
	sa_Treshold_min = 0;
	sa_Treshold_max = 255;
	sa_Grounded = false;
	sa_Radius = 1;

	maskCache.firstRun = true;
	maskCache.Size = 0;
	maskCache.Treshold_min = 0;
	maskCache.Treshold_max = 0;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void MaskEmitter::initPersistFields()
{
	addField( "Treshold_min", TypeS8, Offset(sa_Treshold_min, MaskEmitter),
		"Distance along ejection Z axis from which to eject particles." );

	addField( "Treshold_max", TypeS8, Offset(sa_Treshold_max, MaskEmitter),
		"Distance along ejection Z axis from which to eject particles." );

	addField( "sa_Grounded", TypeS8, Offset(sa_Grounded, MaskEmitter),
		"Distance along ejection Z axis from which to eject particles." );

	addField( "sa_Radius", TypeS8, Offset(sa_Radius, MaskEmitter),
		"Distance along ejection Z axis from which to eject particles." );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 MaskEmitter::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if(stream->writeFlag( mask & sa_Mask ) )
	{
		stream->writeInt(sa_Treshold_min, 10);
		stream->writeInt(sa_Treshold_max, 10);
	}

	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void MaskEmitter::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);
	if( stream->readFlag() )
	{
		sa_Treshold_min = stream->readInt(10);
		sa_Treshold_max = stream->readInt(10);
	}
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool MaskEmitter::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	Parent::onNewDataBlock(dptr, reload);
	MaskEmitterData* DataBlock = getDataBlock();
	if ( !DataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	sa_Treshold_max = DataBlock->Threshold_max;
	sa_Treshold_min = DataBlock->Threshold_min;

	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// addParticle
//-----------------------------------------------------------------------------
bool MaskEmitter::addParticle(const Point3F& pos,
							  const Point3F& axis,
							  const Point3F& vel,
							  const Point3F& axisx, 
							  const MatrixF &trans)
{
	PROFILE_SCOPE(MaskEmitAddPart);
	IPSBenchmarkBegin;
	MaskEmitterData* DataBlock = getDataBlock();
	oldTime = mInternalClock;

	parentNodePos =  pos;
	Point2F pxPt = DataBlock->pMask->getRandomUnitPixel(sa_Treshold_min, sa_Treshold_max, maskCache);
	if(pxPt == Point2F::Max)
		return false;
	F32 relx, rely;
	relx = pxPt.x * DataBlock->radius;
	rely = pxPt.y * DataBlock->radius;
	F32 x = relx;
	F32 y = rely;
	F32 z = 0;
	Point3F p;

	Point3F axisZ(axis.x,axis.y,0.0f);  
	if(axisZ.isZero())  
		axisZ.set(1.0,0.0,0.0);  
	else  
		axisZ.normalize();  

	mCross(axis,axisx,&axisZ);  

	MatrixF rotMat;
	rotMat.setColumn(0,axisx);  
	rotMat.setColumn(1,axis);  
	rotMat.setColumn(2,axisZ);

	rotMat.mulV(Point3F(x,y,z), &p);
	p = pos+p;
	Point3F normal;
   Particle* pNew = mParticleManager->AddParticle();

	F32 initialVel;
	// If it is a standAloneEmitter, then we want it to use the sa values from the node
	/*if(nodeDat->standAloneEmitter)
	{
	initialVel = nodeDat->sa_ejectionVelocity;
	initialVel    += (nodeDat->sa_velocityVariance * 2.0f * gRandGen.randF()) - nodeDat->sa_velocityVariance;

	if(nodeDat->grounded){
	pNew->pos = p + normal*nodeDat->sa_ejectionOffset;
	// Set the relative position for later use.
	pNew->relPos = Point3F(relx, rely, 0+nodeDat->sa_ejectionOffset);
	}
	else{
	pNew->pos = p;
	// Set the relative position for later use.
	pNew->relPos = Point3F(relx, rely, 0);
	}
	}
	else
	{*/
	initialVel = mDataBlock->ejectionVelocity;
	initialVel    += (mDataBlock->velocityVariance * 2.0f * gRandGen.randF()) - mDataBlock->velocityVariance;

	/*if(nodeDat->grounded){
	pNew->pos = p + normal * mDataBlock->ejectionOffset;
	// Set the relative position for later use.
	pNew->relPos = Point3F(relx, rely, 0 + mDataBlock->ejectionOffset);
	}
	else{*/
   pNew->pos = p;
	// Set the relative position for later use.
	pNew->relPos = Point3F(relx, rely, 0);
	//}
	//}
	// Velocity is based on the normal of the vertex
	pNew->vel = Point3F(0,0,1) * initialVel;
   pNew->orientDir = Point3F(0,0,1);

	pNew->acc.set(0, 0, 0);
	pNew->currentAge = 0;

	// Choose a new particle datablack randomly from the list
	U32 dBlockIndex = gRandGen.randI() % mDataBlock->particleDataBlocks.size();
   mParticleManager->initializeParticle(pNew, vel, mDataBlock->particleDataBlocks[dBlockIndex]);
	updateKeyData( pNew );
	IPSBenchmarkEnd("----Graph---- addPart");
	return true;
}

//-----------------------------------------------------------------------------
// addParticle - Node
//-----------------------------------------------------------------------------
bool MaskEmitter::addParticle(const Point3F& pos,
							  const Point3F& axis,
							  const Point3F& vel,
							  const Point3F& axisx,
							  ParticleEmitterNode* pnodeDat)
{
	PROFILE_SCOPE(MaskEmitAddPart);
	IPSBenchmarkBegin;
	MaskEmitterNode* nodeDat = static_cast<MaskEmitterNode*>(pnodeDat);
	MaskEmitterData* DataBlock = getDataBlock();
	oldTime = mInternalClock;

	parentNodePos = nodeDat->getPosition();
	Point2F pxPt;
	if(standAloneEmitter)
		pxPt = DataBlock->pMask->getRandomUnitPixel(sa_Treshold_min, sa_Treshold_max, maskCache);
	else
		pxPt = DataBlock->pMask->getRandomUnitPixel(DataBlock->Threshold_min, DataBlock->Threshold_max, maskCache);
	if(pxPt == Point2F::Max)
		return false;
	F32 relx, rely;
	if(standAloneEmitter){
		relx = pxPt.x * sa_Radius;
		rely = pxPt.y * sa_Radius;
	}
	else{
		relx = pxPt.x * DataBlock->radius;
		rely = pxPt.y * DataBlock->radius;
	}
	F32 x = relx;
	F32 y = rely;
	F32 z = 0;
	Point3F p;
	MatrixF nodeTrans = nodeDat->getTransform();
	nodeTrans.mulP(Point3F(x,y,z), &p);
	Point3F normal;
	if(sa_Grounded)
		IPSCore::GetTerrainInfoAtPosition(p.x,p.y,p.z,normal);
   Particle* pNew = mParticleManager->AddParticle();

	F32 initialVel;
	// If it is a standAloneEmitter, then we want it to use the sa values from the node
	if(standAloneEmitter)
	{
		initialVel = sa_ejectionVelocity;
		initialVel    += (sa_velocityVariance * 2.0f * gRandGen.randF()) - sa_velocityVariance;

		if(sa_Grounded){
         pNew->pos = p + normal*sa_ejectionOffset;
			// Set the relative position for later use.
			pNew->relPos = Point3F(relx, rely, 0+sa_ejectionOffset);
		}
		else{
         pNew->pos = p;
			// Set the relative position for later use.
			pNew->relPos = Point3F(relx, rely, 0);
		}
	}
	else
	{
		initialVel = mDataBlock->ejectionVelocity;
		initialVel    += (mDataBlock->velocityVariance * 2.0f * gRandGen.randF()) - mDataBlock->velocityVariance;

		if(sa_Grounded){
         pNew->pos = p + normal * mDataBlock->ejectionOffset;
			// Set the relative position for later use.
			pNew->relPos = Point3F(relx, rely, 0 + mDataBlock->ejectionOffset);
		}
		else{
         pNew->pos = p;
			// Set the relative position for later use.
			pNew->relPos = Point3F(relx, rely, 0);
		}
	}
	// Velocity is based on the normal of the vertex
	pNew->vel = Point3F(0,0,1) * initialVel;
   pNew->orientDir = Point3F(0,0,1);

	pNew->acc.set(0, 0, 0);
	pNew->currentAge = 0;

	// Choose a new particle datablack randomly from the list
	U32 dBlockIndex = gRandGen.randI() % mDataBlock->particleDataBlocks.size();
   mParticleManager->initializeParticle(pNew, Point3F(0,0,sa_ejectionVelocity), mDataBlock->particleDataBlocks[dBlockIndex]);
	updateKeyData( pNew );
	IPSBenchmarkEnd("----Graph---- addPart");
	return true;
}

void MaskEmitter::onStaticModified(const char* slotName, const char*newValue)
{
	if(strcmp(slotName, "sa_Treshold_min") == 0 ||
		strcmp(slotName, "sa_Treshold_max") == 0)
		setMaskBits( sa_Mask );
	Parent::onStaticModified(slotName, newValue);
}

ParticleEmitter* MaskEmitterData::createEmitter() { return new MaskEmitter; }

DefineEngineMethod(MaskEmitterData, reload, void,(),,
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