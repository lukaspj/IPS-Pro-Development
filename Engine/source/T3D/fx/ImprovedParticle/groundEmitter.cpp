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
	radius   = 5;   // ejection from the emitter point
	
	for(int i = 0; i < GroundEmitterLayers; i++)
	{
		layers[i] = StringTable->EmptyString();
	}
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GroundEmitterData::initPersistFields()
{
	// Add our variables to the worldeditor
	addGroup("GroundEmitterData");

	addField( "layers", TypeTerrainMaterialName, Offset(layers, GroundEmitterData), GroundEmitterLayers,
		"Terrain material name to limit coverage to, or blank to not limit." );

	addField( "radius", TypeF32, Offset(radius, GroundEmitterData),
		"Radius of the circle from the ejection position to emit particles in." );

	endGroup( "GroundEmitterData");
   Con::setVariable("$IPS::GroundEmitterLayers", avar("%u", GroundEmitterLayers));

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void GroundEmitterData::packData(BitStream* stream)
{
	if( stream->writeFlag( radius != 5 ) )
		stream->writeInt((S32)(radius * 100), 16);

	for(int i = 0; i < GroundEmitterLayers; i++)
	{
      if(stream->writeFlag(layers[i] != ""))
		   stream->writeString(layers[i]);
	}
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
   
	for(int i = 0; i < GroundEmitterLayers; i++)
	{
      if(stream->readFlag())
		   layers[i] = stream->readSTString();
	}
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
	sa_radius   = 5;   // ejection from the emitter point
	
	for(int i = 0; i < GroundEmitterLayers; i++)
	{
		sa_layers[i] = StringTable->EmptyString();
	}
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GroundEmitter::initPersistFields()
{

	// Add our variables to the worldeditor
	addGroup("GroundEmitter");

	addField( "layers", TypeTerrainMaterialName, Offset(sa_layers, GroundEmitter), GroundEmitterLayers,
		"Terrain material name to limit coverage to, or blank to not limit." );

	addField( "sa_radius", TypeF32, Offset(sa_radius, GroundEmitter),
		"Radius of the circle from the ejection position to emit particles in." );

	endGroup( "GroundEmitter");

	Parent::initPersistFields();
}

/**PACKUPADTE


	if( stream->writeFlag( mask & emitterEdited) )
	{
		stream->write(sa_radius);
	}

	if( stream->writeFlag( mask & layerEdited) )
	{
		for(int i = 0; i < GroundEmitterLayers; i++)
		{
			stream->writeString(sa_layers[i]);
		}
	}

   **/

/**UNPACKUPDATE


	//EmitterEdited
	if ( stream->readFlag() )
	{
		stream->read((F32 *)&sa_radius);
	}

	if ( stream->readFlag() )
	{
		for(int i = 0; i < GroundEmitterLayers; i++)
		{
			sa_layers[i] = stream->readSTString();
		}
	}

   **/

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
// @description Spawns new particles, calculates their new position and
// initial velocity.
// Ground Emitters take a random point within a circle around the position 
// and tests if there is a terrain at that point.
//-----------------------------------------------------------------------------
bool GroundEmitter::addParticle(const Point3F& pos,
								const Point3F& axis,
								const Point3F& vel,
								const Point3F& axisx, 
								const MatrixF &trans)
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
	if(IPSCore::GetTerrainInfoAtPosition(x,y,z,mat,normal))
	{
		// Need to add layers to the GroundEmitterData
      Particle* pNew = mParticleManager->AddParticle();

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
      mParticleManager->initializeParticle(pNew, Point3F(0,0,DataBlock->ejectionVelocity), mDataBlock->particleDataBlocks[dBlockIndex]);
		updateKeyData( pNew );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// addParticle - Node
// @description Spawns new particles, calculates their new position and
// initial velocity.
// Ground Emitters take a random point within a circle around the node 
// and tests if there is a terrain at that point.
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
   StringTableEntry* _layers = standAloneEmitter ? sa_layers : DataBlock->layers;
	// Get a random point inside a radius of the Emitter Node using a uniform point distribution algorithm.
   F32 r = (standAloneEmitter ? sa_radius : DataBlock->radius) * sqrt(gRandGen.randF());
	F32 theta = 2*M_PI*gRandGen.randF();
	F32 relx = r * cos(theta);
	F32 rely = r * sin(theta);
	F32 x = parentNodePos.x + relx;
	F32 y = parentNodePos.y + rely;
	F32 z;
	StringTableEntry mat;
	Point3F normal;
	// Test if the x, y coordinates specifies a position on top of a terrain.
	if(IPSCore::GetTerrainInfoAtPosition(x,y,z,mat,normal))
	{
		// If the terrain at the position have one of the terrain layers we ant to exclude then return false
		for(int i = 0; i < GroundEmitterLayers; i++)
			if(_layers[i] == mat)
				return false;
      Particle* pNew = mParticleManager->AddParticle();
      F32 _ejectionOffset = standAloneEmitter ? sa_ejectionOffset : DataBlock->ejectionOffset;
      F32 _ejectionVelocity = standAloneEmitter ? sa_ejectionVelocity : DataBlock->ejectionVelocity;
		F32 initialVel;
		// If it is a standAloneEmitter, then we want it to use the sa values from the node
		if(standAloneEmitter)
		{
			initialVel = _ejectionVelocity;
			initialVel += (sa_velocityVariance * 2.0f * gRandGen.randF()) - sa_velocityVariance;
		}
		else
		{
			initialVel = _ejectionVelocity;
			initialVel += (DataBlock->velocityVariance * 2.0f * gRandGen.randF()) - DataBlock->velocityVariance;
		}
      pNew->pos = Point3F(x,y,z) + normal*_ejectionOffset;


		// Set the relative position for later use.
		pNew->relPos = Point3F(relx,rely,0+_ejectionOffset);
		// Velocity is based on the normal of the vertex
		pNew->vel = Point3F(0,0,1) * initialVel;
      pNew->orientDir = Point3F(0,0,1);

		pNew->acc.set(0, 0, 0);
		pNew->currentAge = 0;

		// Choose a new particle datablack randomly from the list
		U32 dBlockIndex = gRandGen.randI() % DataBlock->particleDataBlocks.size();
      mParticleManager->initializeParticle(pNew, Point3F(0,0,_ejectionVelocity), mDataBlock->particleDataBlocks[dBlockIndex]);
		updateKeyData( pNew );
		return true;
	}

	IPSBenchmarkEnd("----Graph---- addPart");
	return false;
}

ParticleEmitter* GroundEmitterData::createEmitter() { return new GroundEmitter; }

DefineEngineMethod(GroundEmitterData, reload, void,(),,
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