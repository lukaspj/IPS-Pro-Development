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

#include <algorithm>

// This is a small hack to prevent including in tAlgorithm.h's swap template function
// that would have broken STL's existing swap template causing compiling errors upon
// usage of std::sort() within this source file. --Nathan Martin
#define _TALGORITHM_H_

// simSet needs this from tAlgorithm.h so we will have a copy of it here.
/// Finds the first matching value within the container
/// returning the the element or last if its not found.
template <class Iterator, class Value>
Iterator find(Iterator first, Iterator last, Value value)
{
	while (first != last && *first != value)
		++first;
	return first;
}

#include "meshEmitter.h"

#include "scene/sceneRenderState.h"
#include "renderInstance/renderPassManager.h"

#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "T3D/gameBase/gameConnection.h"

#include "ts\tsShapeInstance.h"
#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "T3D\staticShape.h"
#include "ts\tsMeshIntrinsics.h"

#include "core/strings/stringUnit.h"
#include "math/mathIO.h"

#include <string>
#include <vector>

#ifndef _BITSTREAM_H_
#include "core/stream/bitStream.h"
#endif

#if defined(TORQUE_OS_XENON)
#  include "gfx/D3D9/360/gfx360MemVertexBuffer.h"
#endif

Point3F MeshEmitter::mWindVelocity( 0.0, 0.0, 0.0 );
const F32 MeshEmitter::AgedSpinToRadians = (1.0f/1000.0f) * (1.0f/360.0f) * M_PI_F * 2.0f;

IMPLEMENT_CO_DATABLOCK_V1(MeshEmitterData);
//IMPLEMENT_CONOBJECT
IMPLEMENT_CO_NETOBJECT_V1(MeshEmitter);

ConsoleDocClass( MeshEmitter,
				"@brief This object is responsible for spawning particles.\n\n"

				"@note This class is not normally instantiated directly - to place a simple "
				"particle emitting object in the scene, use a MeshEmitterNode instead.\n\n"

				"This class is the main interface for creating particles - though it is "
				"usually only accessed from within another object like MeshEmitterNode "
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
				"@see MeshEmitterData\n"
				"@see MeshEmitterNode\n"
				);

ConsoleDocClass( MeshEmitterData,
				"@brief Defines particle emission properties such as ejection angle, period "
				"and velocity for a MeshEmitter.\n\n"

				"@tsexample\n"
				"datablock MeshEmitterData( GrenadeExpDustEmitter )\n"
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
				"@see MeshEmitter\n"
				"@see ParticleData\n"
				"@see MeshEmitterNode\n"
				);
//-----------------------------------------------------------------------------
// MeshEmitterData
//-----------------------------------------------------------------------------

typedef ParticleRenderInst::BlendStyle mParticleBlendStyle;
DefineEnumType( mParticleBlendStyle );

const F32 sgDefaultEjectionOffset = 0.0f;

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
MeshEmitterData::MeshEmitterData()
{
	VECTOR_SET_ASSOCIATION(particleDataBlocks);
	VECTOR_SET_ASSOCIATION(dataBlockIds);

	ejectionPeriodMS = 100;    // 10 Particles Per second
	periodVarianceMS = 0;      // exactly

	ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	velocityVariance = 1.0f;
	ejectionOffset   = sgDefaultEjectionOffset;   // ejection from the emitter point

	softnessDistance = 1.0f;
	ambientFactor = 0.0f;

	lifetimeMS           = 0;
	lifetimeVarianceMS   = 0;

	overrideAdvance  = true;
	orientParticles  = false;
	orientOnVelocity = true;
	useEmitterSizes  = false;
	useEmitterColors = false;
	particleString   = NULL;
	partListInitSize = 0;

	// These members added for support of user defined blend factors
	// and optional particle sorting.
	blendStyle = ParticleRenderInst::BlendUndefined;
	sortParticles = false;
	renderReflection = true;
	reverseOrder = false;
	textureName = 0;
	textureHandle = 0;
	highResOnly = true;

	alignParticles = false;
	alignDirection = Point3F(0.0f, 1.0f, 0.0f);

	ejectionLODStartDistance	= 15.0f;
	ejectionLODEndDistance		= 100.0f;
	lodEjectionPeriod			= 400;
	mediumResDistance			= 10.0f;
	lowResDistance				= 20.0f;
	SimulationLODBegin			= 10;
	SimulationLODEnd			= 100;

	for(int i = 0; i < ParticleBehaviourCount; i++)
		ParticleBHVs[i] = NULL;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void MeshEmitterData::initPersistFields()
{

	addGroup( "MeshEmitterData" );

	addField("ejectionPeriodMS", TYPEID< S32 >(), Offset(ejectionPeriodMS,   MeshEmitterData),
		"Time (in milliseconds) between each particle ejection." );

	addField("periodVarianceMS", TYPEID< S32 >(), Offset(periodVarianceMS,   MeshEmitterData),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "ejectionVelocity", TYPEID< F32 >(), Offset(ejectionVelocity, MeshEmitterData),
		"Particle ejection velocity." );

	addField( "velocityVariance", TYPEID< F32 >(), Offset(velocityVariance, MeshEmitterData),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "ejectionOffset", TYPEID< F32 >(), Offset(ejectionOffset, MeshEmitterData),
		"Distance along ejection Z axis from which to eject particles." );

	addField( "softnessDistance", TYPEID< F32 >(), Offset(softnessDistance, MeshEmitterData),
		"For soft particles, the distance (in meters) where particles will be "
		"faded based on the difference in depth between the particle and the "
		"scene geometry." );

	addField( "ambientFactor", TYPEID< F32 >(), Offset(ambientFactor, MeshEmitterData),
		"Used to generate the final particle color by controlling interpolation "
		"between the particle color and the particle color multiplied by the "
		"ambient light color." );

	addField( "overrideAdvance", TYPEID< bool >(), Offset(overrideAdvance, MeshEmitterData),
		"If false, particles emitted in the same frame have their positions "
		"adjusted. If true, adjustment is skipped and particles will clump "
		"together." );

	addField( "orientParticles", TYPEID< bool >(), Offset(orientParticles, MeshEmitterData),
		"If true, Particles will always face the camera." );

	addField( "orientOnVelocity", TYPEID< bool >(), Offset(orientOnVelocity, MeshEmitterData),
		"If true, particles will be oriented to face in the direction they are moving." );

	addField( "particles", TYPEID< StringTableEntry >(), Offset(particleString, MeshEmitterData),
		"@brief List of space or TAB delimited ParticleData datablock names.\n\n"
		"A random one of these datablocks is selected each time a particle is "
		"emitted." );

	addField( "lifetimeMS", TYPEID< S32 >(), Offset(lifetimeMS, MeshEmitterData),
		"Lifetime of emitted particles (in milliseconds)." );

	addField("lifetimeVarianceMS", TYPEID< S32 >(), Offset(lifetimeVarianceMS, MeshEmitterData),
		"Variance in particle lifetime from 0 - lifetimeMS." );

	addField( "useEmitterSizes", TYPEID< bool >(), Offset(useEmitterSizes, MeshEmitterData),
		"@brief If true, use emitter specified sizes instead of datablock sizes.\n"
		"Useful for Debris particle emitters that control the particle size." );

	addField( "useEmitterColors", TYPEID< bool >(), Offset(useEmitterColors, MeshEmitterData),
		"@brief If true, use emitter specified colors instead of datablock colors.\n\n"
		"Useful for ShapeBase dust and WheeledVehicle wheel particle emitters that use "
		"the current material to control particle color." );

	/// These fields added for support of user defined blend factors and optional particle sorting.
	//@{

	addField( "blendStyle", TYPEID< ParticleRenderInst::BlendStyle >(), Offset(blendStyle, MeshEmitterData),
		"String value that controls how emitted particles blend with the scene." );

	addField( "sortParticles", TYPEID< bool >(), Offset(sortParticles, MeshEmitterData),
		"If true, particles are sorted furthest to nearest.");

	addField( "reverseOrder", TYPEID< bool >(), Offset(reverseOrder, MeshEmitterData),
		"@brief If true, reverses the normal draw order of particles.\n\n"
		"Particles are normally drawn from newest to oldest, or in Z order "
		"(furthest first) if sortParticles is true. Setting this field to "
		"true will reverse that order: oldest first, or nearest first if "
		"sortParticles is true." );

	addField( "textureName", TYPEID< StringTableEntry >(), Offset(textureName, MeshEmitterData),
		"Optional texture to override ParticleData::textureName." );

	addField( "alignParticles", TYPEID< bool >(), Offset(alignParticles, MeshEmitterData),
		"If true, particles always face along the axis defined by alignDirection." );

	addProtectedField( "alignDirection", TYPEID< Point3F>(), Offset(alignDirection, MeshEmitterData), &MeshEmitterData::_setAlignDirection, &defaultProtectedGetFn,
		"The direction aligned particles should face, only valid if alignParticles is true." );

	addField( "highResOnly", TYPEID< bool >(), Offset(highResOnly, MeshEmitterData),
		"This particle system should not use the mixed-resolution renderer. "
		"If your particle system has large amounts of overdraw, consider "
		"disabling this option." );

	addField( "renderReflection", TYPEID< bool >(), Offset(renderReflection, MeshEmitterData),
		"Controls whether particles are rendered onto reflective surfaces like water." );

	//@}

	endGroup( "MeshEmitterData" );

	addGroup( "ParticleBehaviours" );

	addField("ParticleBehaviour", TYPEID<IParticleBehaviour>(), Offset(ParticleBHVs, MeshEmitterData), ParticleBehaviourCount,
		"Null");

	endGroup( "ParticleBehaviours" );

	addGroup( "LevelOfDetail" );

	addField( "MediumResolutionDistance", TypeF32, Offset(mediumResDistance, MeshEmitterData),
		"@brief How far the node must be from the camera before the emitter switches to the medium resolution texture.");

	addField( "LowResolutionDistance", TypeF32, Offset(lowResDistance, MeshEmitterData),
		"@brief How far the node must be from the camera before the emitter switches to the low resolution texture.");

	addField( "EjectionLODStartDistance", TypeF32, Offset(ejectionLODStartDistance, MeshEmitterData),
		"@brief How far the node must be from the camera before the emitter interpolates the ejectionPeriod towards LODEjectionPeriod.");

	addField( "EjectionLODEndDistance", TypeF32, Offset(ejectionLODEndDistance, MeshEmitterData),
		"@brief How far the node must be from the camera before the emitter stops interpolating the ejection period and stops emitting particles.");

	addField( "LODEjectionPeriod", TypeS32, Offset(lodEjectionPeriod, MeshEmitterData),
		"@brief What level the ejection period interpolates to based on distance.");

	addField( "SimulationLODBegin", TypeS32, Offset(SimulationLODBegin, MeshEmitterData),
		"@brief What ");

	addField( "SimulationLODEnd", TypeS32, Offset(SimulationLODEnd, MeshEmitterData),
		"@brief What ");

	endGroup( "LevelOfDetail" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// _setAlignDirection
//-----------------------------------------------------------------------------
bool MeshEmitterData::_setAlignDirection( void *object, const char *index, const char *data )
{
	MeshEmitterData *p = static_cast<MeshEmitterData*>( object );

	Con::setData( TypePoint3F, &p->alignDirection, 0, 1, &data );
	p->alignDirection.normalizeSafe();

	// we already set the field
	return false;
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void MeshEmitterData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->writeInt(ejectionPeriodMS, 10);
	stream->writeInt(periodVarianceMS, 10);
	stream->writeInt((S32)(ejectionVelocity * 100), 16);
	stream->writeInt((S32)(velocityVariance * 100), 14);
	if( stream->writeFlag( ejectionOffset != sgDefaultEjectionOffset ) )
		stream->writeInt((S32)(ejectionOffset * 100), 16);

	stream->write( softnessDistance );
	stream->write( ambientFactor );

	stream->writeFlag(overrideAdvance);
	stream->writeFlag(orientParticles);
	stream->writeFlag(orientOnVelocity);
	stream->write( lifetimeMS );
	stream->write( lifetimeVarianceMS );
	stream->writeFlag(useEmitterSizes);
	stream->writeFlag(useEmitterColors);

	stream->write(dataBlockIds.size());
	for (U32 i = 0; i < dataBlockIds.size(); i++)
		stream->write(dataBlockIds[i]);
	stream->writeFlag(sortParticles);
	stream->writeFlag(reverseOrder);
	if (stream->writeFlag(textureName != 0))
		stream->writeString(textureName);

	if (stream->writeFlag(alignParticles))
	{
		stream->write(alignDirection.x);
		stream->write(alignDirection.y);
		stream->write(alignDirection.z);
	}
	stream->writeFlag(highResOnly);
	stream->writeFlag(renderReflection);
	stream->writeInt( blendStyle, 4 );

	stream->writeInt(mediumResDistance * 1000, 18); 
	stream->writeInt(lowResDistance * 1000, 18);
	stream->writeInt(ejectionLODStartDistance * 1000, 18);
	stream->writeInt(ejectionLODEndDistance * 1000, 18);
	stream->writeInt(lodEjectionPeriod, 11);

	stream->writeInt(SimulationLODBegin * 1000, 18);
	stream->writeInt(SimulationLODEnd * 1000, 18);

	for(int i = 0; i < ParticleBehaviourCount; i++)
	{
		if(stream->writeFlag(ParticleBHVs[i]))
		{
			stream->writeRangedU32( ParticleBHVs[i]->getId(),
				DataBlockObjectIdFirst,
				DataBlockObjectIdLast );
		}
	}
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void MeshEmitterData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	ejectionPeriodMS = stream->readInt(10);
	periodVarianceMS = stream->readInt(10);
	ejectionVelocity = stream->readInt(16) / 100.0f;
	velocityVariance = stream->readInt(14) / 100.0f;
	if( stream->readFlag() )
		ejectionOffset = stream->readInt(16) / 100.0f;
	else
		ejectionOffset = sgDefaultEjectionOffset;

	stream->read( &softnessDistance );
	stream->read( &ambientFactor );

	overrideAdvance = stream->readFlag();
	orientParticles = stream->readFlag();
	orientOnVelocity = stream->readFlag();
	stream->read( &lifetimeMS );
	stream->read( &lifetimeVarianceMS );
	useEmitterSizes = stream->readFlag();
	useEmitterColors = stream->readFlag();

	U32 size; stream->read(&size);
	dataBlockIds.setSize(size);
	for (U32 i = 0; i < dataBlockIds.size(); i++)
		stream->read(&dataBlockIds[i]);
	sortParticles = stream->readFlag();
	reverseOrder = stream->readFlag();
	textureName = (stream->readFlag()) ? stream->readSTString() : 0;

	alignParticles = stream->readFlag();
	if (alignParticles)
	{
		stream->read(&alignDirection.x);
		stream->read(&alignDirection.y);
		stream->read(&alignDirection.z);
	}
	highResOnly = stream->readFlag();
	renderReflection = stream->readFlag();
	blendStyle = stream->readInt( 4 );

	mediumResDistance			= stream->readInt(18) / 1000.0f;
	lowResDistance				= stream->readInt(18) / 1000.0f;
	ejectionLODStartDistance	= stream->readInt(18) / 1000.0f;
	ejectionLODEndDistance		= stream->readInt(18) / 1000.0f;
	lodEjectionPeriod			= stream->readInt(11);
	SimulationLODBegin			= stream->readInt(18) / 1000.0f;
	SimulationLODEnd			= stream->readInt(18) / 1000.0f;

	for(int i = 0; i < ParticleBehaviourCount; i++)
	{
		if ( stream->readFlag() )
		{
			SimDataBlock *dptr = 0;
			SimObjectId id = stream->readRangedU32( DataBlockObjectIdFirst,
				DataBlockObjectIdLast );
			if ( !Sim::findObject( id, dptr ) )
				ParticleBHVs[i] = dptr;
		}
	}
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool MeshEmitterData::onAdd()
{
	if( Parent::onAdd() == false )
		return false;

	//   if (overrideAdvance == true) {
	//      Con::errorf(ConsoleLogEntry::General, "MeshEmitterData: Not going to work.  Fix it!");
	//      return false;
	//   }

	// Validate the parameters...
	//
	if( ejectionPeriodMS < 1 )
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) period < 1 ms", getName());
		ejectionPeriodMS = 1;
	}
	if( periodVarianceMS >= ejectionPeriodMS )
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) periodVariance >= period", getName());
		periodVarianceMS = ejectionPeriodMS - 1;
	}
	if( ejectionVelocity < 0.0f )
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) ejectionVelocity < 0.0f", getName());
		ejectionVelocity = 0.0f;
	}
	if( velocityVariance < 0.0f )
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) velocityVariance < 0.0f", getName());
		velocityVariance = 0.0f;
	}
	if( velocityVariance > ejectionVelocity )
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) velocityVariance > ejectionVelocity", getName());
		velocityVariance = ejectionVelocity;
	}
	if( ejectionOffset < 0.0f )
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) ejectionOffset < 0", getName());
		ejectionOffset = 0.0f;
	}

	if ( softnessDistance < 0.0f )
	{
		Con::warnf( ConsoleLogEntry::General, "MeshEmitterData(%s) invalid softnessDistance", getName() );
		softnessDistance = 0.0f;
	}

	if (particleString == NULL && dataBlockIds.size() == 0) 
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) no particleString, invalid datablock", getName());
		return false;
	}
	if (particleString && particleString[0] == '\0') 
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) no particleString, invalid datablock", getName());
		return false;
	}
	if (particleString && dStrlen(particleString) > 255) 
	{
		Con::errorf(ConsoleLogEntry::General, "MeshEmitterData(%s) particle string too long [> 255 chars]", getName());
		return false;
	}

	if( lifetimeMS < 0 )
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) lifetimeMS < 0.0f", getName());
		lifetimeMS = 0;
	}
	if( lifetimeVarianceMS > lifetimeMS )
	{
		Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) lifetimeVarianceMS >= lifetimeMS", getName());
		lifetimeVarianceMS = lifetimeMS;
	}


	// load the particle datablocks...
	//
	if( particleString != NULL )
	{
		//   particleString is once again a list of particle datablocks so it
		//   must be parsed to extract the particle references.

		// First we parse particleString into a list of particle name tokens 
		Vector<char*> dataBlocks(__FILE__, __LINE__);
		char* tokCopy = new char[dStrlen(particleString) + 1];
		dStrcpy(tokCopy, particleString);

		char* currTok = dStrtok(tokCopy, " \t");
		while (currTok != NULL) 
		{
			dataBlocks.push_back(currTok);
			currTok = dStrtok(NULL, " \t");
		}
		if (dataBlocks.size() == 0) 
		{
			Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) invalid particles string.  No datablocks found", getName());
			delete [] tokCopy;
			return false;
		}    

		// Now we convert the particle name tokens into particle datablocks and IDs 
		particleDataBlocks.clear();
		dataBlockIds.clear();

		for (U32 i = 0; i < dataBlocks.size(); i++) 
		{
         ParticleData* pData = NULL;
			if (Sim::findObject(dataBlocks[i], pData) == false) 
			{
				Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) unable to find particle datablock: %s", getName(), dataBlocks[i]);
			}
			else 
			{
				particleDataBlocks.push_back(pData);
				dataBlockIds.push_back(pData->getId());
			}
		}

		// cleanup
		delete [] tokCopy;

		// check that we actually found some particle datablocks
		if (particleDataBlocks.size() == 0) 
		{
			Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) unable to find any particle datablocks", getName());
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool MeshEmitterData::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;

	particleDataBlocks.clear();
	for (U32 i = 0; i < dataBlockIds.size(); i++) 
	{
      ParticleData* pData = NULL;
		if (Sim::findObject(dataBlockIds[i], pData) == false)
			Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) unable to find particle datablock: %d", getName(), dataBlockIds[i]);
		else
			particleDataBlocks.push_back(pData);
	}

	if (!server)
	{
		// load emitter texture if specified
		if (textureName && textureName[0])
		{
			textureHandle = GFXTexHandle(textureName, &GFXDefaultStaticDiffuseProfile, avar("%s() - textureHandle (line %d)", __FUNCTION__, __LINE__));
			if (!textureHandle)
			{
				errorStr = String::ToString("Missing particle emitter texture: %s", textureName);
				return false;
			}
		}/*
		// otherwise, check that all particles refer to the same texture
		else if (particleDataBlocks.size() > 1)
		{
			StringTableEntry htxr_name, mtxr_name, ltxr_name;
			htxr_name = particleDataBlocks[0]->hResTextureName;
			mtxr_name = particleDataBlocks[0]->mResTextureName;
			ltxr_name = particleDataBlocks[0]->lResTextureName;
			for (S32 i = 1; i < particleDataBlocks.size(); i++)
			{
				// warn if particle textures are inconsistent
				if (particleDataBlocks[i]->hResTextureName != htxr_name &&
					particleDataBlocks[i]->mResTextureName != mtxr_name &&
					particleDataBlocks[i]->lResTextureName != ltxr_name)
				{
					Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) particles reference different textures.", getName());
					break;
				}
			}
		}*/
	}

   /*
	// if blend-style is undefined check legacy useInvAlpha settings
	if (blendStyle == ParticleRenderInst::BlendUndefined && particleDataBlocks.size() > 0)
	{
		bool useInvAlpha = particleDataBlocks[0]->useInvAlpha;
		for (S32 i = 1; i < particleDataBlocks.size(); i++)
		{
			// warn if blend-style legacy useInvAlpha settings are inconsistent
			if (particleDataBlocks[i]->useInvAlpha != useInvAlpha)
			{
				Con::warnf(ConsoleLogEntry::General, "MeshEmitterData(%s) particles have inconsistent useInvAlpha settings.", getName());
				break;
			}
		}
		blendStyle = (useInvAlpha) ? ParticleRenderInst::BlendNormal : ParticleRenderInst::BlendAdditive;
	}*/

	if( !server )
	{
		allocPrimBuffer();
	}

	return true;
}

//-----------------------------------------------------------------------------
// alloc PrimitiveBuffer
// The datablock allocates this static index buffer because it's the same
// for all of the emitters - each particle quad uses the same index ordering
//-----------------------------------------------------------------------------
void MeshEmitterData::allocPrimBuffer( S32 overrideSize )
{
	// calculate particle list size
	AssertFatal(particleDataBlocks.size() > 0, "Error, no particles found." );
	U32 maxPartLife = particleDataBlocks[0]->lifetimeMS + particleDataBlocks[0]->lifetimeVarianceMS;
	for (S32 i = 1; i < particleDataBlocks.size(); i++)
	{
		U32 mpl = particleDataBlocks[i]->lifetimeMS + particleDataBlocks[i]->lifetimeVarianceMS;
		if (mpl > maxPartLife)
			maxPartLife = mpl;
	}

	partListInitSize = maxPartLife / (ejectionPeriodMS - periodVarianceMS);
	partListInitSize += 8; // add 8 as "fudge factor" to make sure it doesn't realloc if it goes over by 1

	// if override size is specified, then the emitter overran its buffer and needs a larger allocation
	if( overrideSize != -1 )
	{
		partListInitSize = overrideSize;
	}

	// create index buffer based on that size
	U32 indexListSize = partListInitSize * 6; // 6 indices per particle
	U16 *indices = new U16[ indexListSize ];

	for( U32 i=0; i<partListInitSize; i++ )
	{
		// this index ordering should be optimal (hopefully) for the vertex cache
		U16 *idx = &indices[i*6];
		volatile U32 offset = i * 4;  // set to volatile to fix VC6 Release mode compiler bug
		idx[0] = 0 + offset;
		idx[1] = 1 + offset;
		idx[2] = 3 + offset;
		idx[3] = 1 + offset;
		idx[4] = 3 + offset;
		idx[5] = 2 + offset;
	}


	U16 *ibIndices;
	GFXBufferType bufferType = GFXBufferTypeStatic;

#ifdef TORQUE_OS_XENON
	// Because of the way the volatile buffers work on Xenon this is the only
	// way to do this.
	bufferType = GFXBufferTypeVolatile;
#endif
	primBuff.set( GFX, indexListSize, 0, bufferType );
	primBuff.lock( &ibIndices );
	dMemcpy( ibIndices, indices, indexListSize * sizeof(U16) );
	primBuff.unlock();

	delete [] indices;
}

//-----------------------------------------------------------------------------
// MeshEmitter
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
MeshEmitter::MeshEmitter()
{
	// MeshEmitter serves as both a node and an emitter
	// - Therefore it has to be on both server and client.
	mNetFlags.set(ScopeAlways | Ghostable);

	mTypeMask |= EnvironmentObjectType;

	mDeleteWhenEmpty  = false;
	mDeleteOnTick     = false;

	mInternalClock    = 0;
	mNextParticleTime = 0;

	mLastPosition.set(0, 0, 0);
	mHasLastPosition = false;

	mLifetimeMS = 0;
	mElapsedTimeMS = 0;

	mDead = false;

	// Standard particle variables
	//  - These should maybe be removed her and handled in datablock only
	ejectionPeriodMS = 100;    // 10 Particles Per second
	periodVarianceMS = 0;      // exactly

	ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	velocityVariance = 1.0f;
	ejectionOffset   = 0.0f;   // ejection from the emitter point

	softnessDistance = 1.0f;
	ambientFactor = 0.0f;

	lifetimeMS           = 0;
	lifetimeVarianceMS   = 0;

	overrideAdvance  = true;
	orientParticles  = false;
	orientOnVelocity = true;
	useEmitterSizes  = false;
	useEmitterColors = false;

	// These members added for support of user defined blend factors
	// and optional particle sorting.
	blendStyle = ParticleRenderInst::BlendUndefined;
	sortParticles = false;
	renderReflection = true;
	reverseOrder = false;
	textureName = 0;
	textureHandle = 0;
	highResOnly = true;

	alignParticles = false;
	alignDirection = Point3F(0.0f, 1.0f, 0.0f);

	// Mesh variables
	evenEmission = true;
	emitOnFaces = true;
	emitMesh = "";
	vertexCount = 0;

	parentNodePos = Point3F(0);
	RenderEjectionPeriodMS = 0;

	enableDebugRender = false;

	mDataBlock = NULL;
   mParticleManager = NULL;
}

//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
MeshEmitter::~MeshEmitter()
{
   if(mParticleManager){
      delete mParticleManager->mEmitter;
      delete mParticleManager->mEmitterDatablock;
      delete mParticleManager;
   }
}

//-----------------------------------------------------------------------------
// initPersistFields
// Changed
//-----------------------------------------------------------------------------
void MeshEmitter::initPersistFields()
{

	addGroup( "MeshEmitter" );

	addField( "emitMesh", TYPEID< StringTableEntry >(), Offset(emitMesh, MeshEmitter),
		"The object that the emitter will use to emit particles on." );

	addField( "evenEmission", TYPEID< bool >(), Offset(evenEmission, MeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	addField( "emitOnFaces", TYPEID< bool >(), Offset(emitOnFaces, MeshEmitter),
		"If true, particles will be emitted along the faces of the mesh. "
		"If false, particles will be emitted along the vertices of mesh. " );

	endGroup( "MeshEmitter" );

	addGroup( "Debug" );

	addField( "enableDebugRender", TYPEID< bool >(), Offset(enableDebugRender, MeshEmitter),
		"If true, particle emission will be spread evenly along the whole model "
		"if false then there will be more particles where the geometry is more dense. "
		"Different effects for per vertex and per triangle emission - Read docs!." );

	endGroup( "Debug" );

	addGroup( "Particles" );

	addField("ejectionPeriodMS", TYPEID< S32 >(), Offset(ejectionPeriodMS,   MeshEmitter),
		"Time (in milliseconds) between each particle ejection." );

	addField("periodVarianceMS", TYPEID< S32 >(), Offset(periodVarianceMS,   MeshEmitter),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "ejectionVelocity", TYPEID< F32 >(), Offset(ejectionVelocity, MeshEmitter),
		"Particle ejection velocity." );

	addField( "velocityVariance", TYPEID< F32 >(), Offset(velocityVariance, MeshEmitter),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "ejectionOffset", TYPEID< F32 >(), Offset(ejectionOffset, MeshEmitter),
		"Distance along ejection Z axis from which to eject particles." );

	addField( "softnessDistance", TYPEID< F32 >(), Offset(softnessDistance, MeshEmitter),
		"For soft particles, the distance (in meters) where particles will be "
		"faded based on the difference in depth between the particle and the "
		"scene geometry." );

	addField( "ambientFactor", TYPEID< F32 >(), Offset(ambientFactor, MeshEmitter),
		"Used to generate the final particle color by controlling interpolation "
		"between the particle color and the particle color multiplied by the "
		"ambient light color." );

	addField( "overrideAdvance", TYPEID< bool >(), Offset(overrideAdvance, MeshEmitter),
		"If false, particles emitted in the same frame have their positions "
		"adjusted. If true, adjustment is skipped and particles will clump "
		"together." );

	addField( "orientParticles", TYPEID< bool >(), Offset(orientParticles, MeshEmitter),
		"If true, Particles will always face the camera." );

	addField( "orientOnVelocity", TYPEID< bool >(), Offset(orientOnVelocity, MeshEmitter),
		"If true, particles will be oriented to face in the direction they are moving." );

	addField( "lifetimeMS", TYPEID< S32 >(), Offset(lifetimeMS, MeshEmitter),
		"Lifetime of emitted particles (in milliseconds)." );

	addField("lifetimeVarianceMS", TYPEID< S32 >(), Offset(lifetimeVarianceMS, MeshEmitter),
		"Variance in particle lifetime from 0 - lifetimeMS." );

	addField( "useEmitterSizes", TYPEID< bool >(), Offset(useEmitterSizes, MeshEmitter),
		"@brief If true, use emitter specified sizes instead of datablock sizes.\n"
		"Useful for Debris particle emitters that control the particle size." );

	addField( "useEmitterColors", TYPEID< bool >(), Offset(useEmitterColors, MeshEmitter),
		"@brief If true, use emitter specified colors instead of datablock colors.\n\n"
		"Useful for ShapeBase dust and WheeledVehicle wheel particle emitters that use "
		"the current material to control particle color." );

	/// These fields added for support of user defined blend factors and optional particle sorting.
	//@{

	addField( "blendStyle", TYPEID< ParticleRenderInst::BlendStyle >(), Offset(blendStyle, MeshEmitter),
		"String value that controls how emitted particles blend with the scene." );

	addField( "sortParticles", TYPEID< bool >(), Offset(sortParticles, MeshEmitter),
		"If true, particles are sorted furthest to nearest.");

	addField( "reverseOrder", TYPEID< bool >(), Offset(reverseOrder, MeshEmitter),
		"@brief If true, reverses the normal draw order of particles.\n\n"
		"Particles are normally drawn from newest to oldest, or in Z order "
		"(furthest first) if sortParticles is true. Setting this field to "
		"true will reverse that order: oldest first, or nearest first if "
		"sortParticles is true." );

	addField( "textureName", TYPEID< StringTableEntry >(), Offset(textureName, MeshEmitter),
		"Optional texture to override ParticleData::textureName." );

	addField( "alignParticles", TYPEID< bool >(), Offset(alignParticles, MeshEmitter),
		"If true, particles always face along the axis defined by alignDirection." );

	addProtectedField( "alignDirection", TYPEID< Point3F>(), Offset(alignDirection, MeshEmitter), &MeshEmitter::_setAlignDirection, &defaultProtectedGetFn,
		"The direction aligned particles should face, only valid if alignParticles is true." );

	addField( "highResOnly", TYPEID< bool >(), Offset(highResOnly, MeshEmitter),
		"This particle system should not use the mixed-resolution renderer. "
		"If your particle system has large amounts of overdraw, consider "
		"disabling this option." );

	addField( "renderReflection", TYPEID< bool >(), Offset(renderReflection, MeshEmitter),
		"Controls whether particles are rendered onto reflective surfaces like water." );

	endGroup( "Particles" );

	Parent::initPersistFields();
}
//-----------------------------------------------------------------------------
// _setAlignDirection
// Not changed
//-----------------------------------------------------------------------------
bool MeshEmitter::_setAlignDirection( void *object, const char *index, const char *data )
{
	MeshEmitter *p = static_cast<MeshEmitter*>( object );

	Con::setData( TypePoint3F, &p->alignDirection, 0, 1, &data );
	p->alignDirection.normalizeSafe();

	// we already set the field
	return false;
}

//-----------------------------------------------------------------------------
// onAdd
// Not changed
//-----------------------------------------------------------------------------
bool MeshEmitter::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if(isServerObject())
		setMaskBits( StateMask );

	F32 radius = 0.5;
	mObjBox.minExtents = Point3F(-radius, -radius, -radius);
	mObjBox.maxExtents = Point3F(radius, radius, radius);
	resetWorldBox();
	addToScene();
	setRenderEnabled(true);

	return true;
}


//-----------------------------------------------------------------------------
// onRemove
// Not changed
//-----------------------------------------------------------------------------
void MeshEmitter::onRemove()
{
	removeFromScene();
	Parent::onRemove();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
// Changed
//-----------------------------------------------------------------------------
bool MeshEmitter::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	mDataBlock = dynamic_cast<MeshEmitterData*>( dptr );
	if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	mLifetimeMS = lifetimeMS;
	if( lifetimeVarianceMS )
	{
		mLifetimeMS += S32( gRandGen.randI() % (2 * lifetimeVarianceMS + 1)) - S32(lifetimeVarianceMS );
	}

	
   if(mParticleManager){
      delete mParticleManager->mEmitter;
      delete mParticleManager->mEmitterDatablock;
      delete mParticleManager;
   }
   mParticleManager = mDataBlock->particleDataBlocks[0]->CreateManager();
   mParticleManager->mEmitter = new MeshEmitterWrapper(this);
   mParticleManager->mEmitterDatablock = new MeshEmitterDataWrapper(mDataBlock);
   mParticleManager->AllocateParticles();

	// Copy values from DB -----
	ejectionPeriodMS = mDataBlock->ejectionPeriodMS;    // 10 Particles Per second
	periodVarianceMS = mDataBlock->periodVarianceMS;      // exactly

	ejectionVelocity = mDataBlock->ejectionVelocity;   // From 1.0 - 3.0 meters per sec
	velocityVariance = mDataBlock->velocityVariance;
	ejectionOffset   = mDataBlock->ejectionOffset;   // ejection from the emitter point

	softnessDistance = mDataBlock->softnessDistance;
	ambientFactor	 = mDataBlock->ambientFactor;

	lifetimeMS           = mDataBlock->lifetimeMS;
	lifetimeVarianceMS   = mDataBlock->lifetimeVarianceMS;

	overrideAdvance  = mDataBlock->overrideAdvance;
	orientParticles  = mDataBlock->orientParticles;
	orientOnVelocity = mDataBlock->orientOnVelocity;
	useEmitterSizes  = mDataBlock->useEmitterSizes;
	useEmitterColors = mDataBlock->useEmitterColors;

	RenderEjectionPeriodMS = ejectionPeriodMS;

	// These members added for support of user defined blend factors
	// and optional particle sorting.
	blendStyle = mDataBlock->blendStyle;
	sortParticles = mDataBlock->sortParticles;
	renderReflection = mDataBlock->renderReflection;
	reverseOrder = mDataBlock->reverseOrder;
	textureName = mDataBlock->textureName;
	textureHandle = mDataBlock->textureHandle;
	highResOnly = mDataBlock->highResOnly;

	alignParticles = mDataBlock->alignParticles;
	alignDirection = mDataBlock->alignDirection;
	// ----- Copy values from DB

	for(int i = 0; i < initialValues.size(); i=i+2)
	{
		// Should these be datablock only values?
		if(strcmp("ejectionPeriodMS",initialValues[i].c_str()) == 0)
			ejectionPeriodMS = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("periodVarianceMS",initialValues[i].c_str()) == 0)
			periodVarianceMS = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("ejectionVelocity",initialValues[i].c_str()) == 0)
			ejectionVelocity = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("velocityVariance",initialValues[i].c_str()) == 0)
			velocityVariance = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("ejectionOffset",initialValues[i].c_str()) == 0)
			ejectionOffset = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("softnessDistance",initialValues[i].c_str()) == 0)
			softnessDistance = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("ambientFactor",initialValues[i].c_str()) == 0)
			ambientFactor = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("lifetimeMS",initialValues[i].c_str()) == 0)
			lifetimeMS = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("lifetimeVarianceMS",initialValues[i].c_str()) == 0)
			lifetimeVarianceMS = atoi(const_cast<char*>(initialValues[i+1].c_str()));

		// These values can de defined on a per-emitter basis and therefore
		//  - We have to reload them here or the datablock will override them.
	}

	scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// getCollectiveColor
// Not changed
//-----------------------------------------------------------------------------
ColorF MeshEmitter::getCollectiveColor()
{
   return mParticleManager->getCollectiveColor();
}

//-----------------------------------------------------------------------------
// prepRenderImage
// Not changed
//-----------------------------------------------------------------------------
void MeshEmitter::prepRenderImage(SceneRenderState* state)
{
	if(enableDebugRender)
	{
		ObjectRenderInst *ori = state->getRenderPass()->allocInst<ObjectRenderInst>();
		bindDelegate(ori);
		ori->type = RenderPassManager::RIT_Editor;
		state->getRenderPass()->addInst(ori);
	}
   // Check if the emitMesh matches a name
   SceneObject* SB = dynamic_cast<SceneObject*>(Sim::findObject(emitMesh));
	// If not then check if it matches an ID
	if(!SB)
		SB = dynamic_cast<SceneObject*>(Sim::findObject(atoi(emitMesh)));
   if(SB && !state->getCullingState().getFrustum().isCulled(mObjBox))
		isObjectCulled = false;
	else
		isObjectCulled = true;

	if(isObjectCulled)
		return;
   mParticleManager->RenderParticles(state);

}

//-----------------------------------------------------------------------------
// setSizes
// Not changed
//-----------------------------------------------------------------------------
void MeshEmitter::setSizes( F32 *sizeList )
{
	for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
	{
		sizes[i] = sizeList[i];
	}
}

//-----------------------------------------------------------------------------
// setColors
// Not changed
//-----------------------------------------------------------------------------
void MeshEmitter::setColors( ColorF *colorList )
{
	for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
	{
		colors[i] = colorList[i];
	}
}

//-----------------------------------------------------------------------------
// deleteWhenEmpty
// Not changed
//-----------------------------------------------------------------------------
void MeshEmitter::deleteWhenEmpty()
{
	// if the following asserts fire, there is a reasonable chance that you are trying to delete a particle emitter
	// that has already been deleted (possibly by ClientMissionCleanup). If so, use a SimObjectPtr to the emitter and check it
	// for null before calling this function.
	AssertFatal(isProperlyAdded(), "MeshEmitter must be registed before calling deleteWhenEmpty");
	AssertFatal(!mDead, "MeshEmitter already deleted");
	AssertFatal(!isDeleted(), "MeshEmitter already deleted");
	AssertFatal(!isRemoved(), "MeshEmitter already removed");

	// this check is for non debug case, so that we don't write in to freed memory
	bool okToDelete = !mDead && isProperlyAdded() && !isDeleted() && !isRemoved();
	if (okToDelete)
	{
		mDeleteWhenEmpty = true;
		if( !mParticleManager->n_parts )
		{
			// We're already empty, so delete us now.

			mDead = true;
			deleteObject();
		}
		else
			AssertFatal( getSceneManager() != NULL, "MeshEmitter not on process list and won't get ticked to death" );
	}
}

//-----------------------------------------------------------------------------
// emitParticles
// Changed
//-----------------------------------------------------------------------------
void MeshEmitter::emitParticles( 
	const F32 velocity,
	const U32      numMilliseconds)
{
	if( mDead || !mDataBlock ) return;

	if( mDataBlock->particleDataBlocks.empty() )
		return;

	// lifetime over - no more particles
	if( mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS )
	{
		return;
	}

	U32 currTime = 0;
	bool particlesAdded = false;

	if( mNextParticleTime != 0 )
	{
		// Need to handle next particle
		//
		if( mNextParticleTime > numMilliseconds )
		{
			// Defer to next update
			//  (Note that this introduces a potential spatial irregularity if the owning
			//   object is accelerating, and updating at a low frequency)
			//
			mNextParticleTime -= numMilliseconds;
			mInternalClock += numMilliseconds;
			getTransform().getColumn(3, &mLastPosition);
			mHasLastPosition = true;
			return;
		}
		else
		{
			currTime       += mNextParticleTime;
			mInternalClock += mNextParticleTime;
			// Emit particle at curr time
			// Also send the node on addParticle
			addParticle(velocity);
			particlesAdded = true;
			mNextParticleTime = 0;
		}
	}

	while( currTime < numMilliseconds )
	{
		S32 nextTime = RenderEjectionPeriodMS;
		if( periodVarianceMS != 0 )
		{
			nextTime += S32(gRandGen.randI() % (2 * periodVarianceMS + 1)) -
				S32(periodVarianceMS);
		}
		AssertFatal(nextTime > 0, "Error, next particle ejection time must always be greater than 0");

		if( currTime + nextTime > numMilliseconds )
		{
			mNextParticleTime = (currTime + nextTime) - numMilliseconds;
			mInternalClock   += numMilliseconds - currTime;
			AssertFatal(mNextParticleTime > 0, "Error, should not have deferred this particle!");
			break;
		}

		currTime       += nextTime;
		mInternalClock += nextTime;

		addParticle(velocity);
		particlesAdded = true;

		//   This override-advance code is restored in order to correctly adjust
		//   animated parameters of particles allocated within the same frame
		//   update. Note that ordering is important and this code correctly 
		//   adds particles in the same newest-to-oldest ordering of the link-list.
		//
		// NOTE: We are assuming that the just added particle is at the head of our
		//  list.  If that changes, so must this...
		U32 advanceMS = numMilliseconds - currTime;
		if (overrideAdvance == false && advanceMS != 0) 
		{
			Particle* last_part = mParticleManager->part_list_head.next;
			if (advanceMS > last_part->totalLifetime) 
			{
				mParticleManager->part_list_head.next = last_part->next;
				mParticleManager->n_parts--;
				last_part->next = mParticleManager->part_freelist;
				mParticleManager->part_freelist = last_part;
			} 
			else 
			{
				if (advanceMS != 0)
				{
					F32 t = F32(advanceMS) / 1000.0;

					Point3F a = last_part->acc;
					a -= last_part->vel * last_part->dataBlock->dragCoefficient;
					a -= mWindVelocity * last_part->dataBlock->windCoefficient;
					a += Point3F(0.0f, 0.0f, -9.81f) * last_part->dataBlock->gravityCoefficient;

					last_part->vel += a * t;
               last_part->pos = last_part->pos + last_part->vel * t;

               updateKeyData( last_part, numMilliseconds );
				}
			}
		}
	}

	// DMMFIX: Lame and slow...
	if( particlesAdded == true )
		updateBBox();


	if( mParticleManager->n_parts > 0 && getSceneManager() == NULL )
	{
		gClientSceneGraph->addObjectToScene(this);
		//ClientProcessList::get()->addObject(this);
	}

	getTransform().getColumn(3, &mLastPosition);
	mHasLastPosition = true;
}

//-----------------------------------------------------------------------------
// updateBBox - SLOW, bad news
// Not changed
//-----------------------------------------------------------------------------
void MeshEmitter::updateBBox()
{
	Point3F minPt(1e10,   1e10,  1e10);
	Point3F maxPt(-1e10, -1e10, -1e10);

	for (Particle* part = mParticleManager->part_list_head.next; part != NULL; part = part->next)
	{
      Point3F particleSize(part->size * 0.5f, 0.0f, part->size * 0.5f);
      minPt.setMin( part->pos - particleSize );
      maxPt.setMax( part->pos + particleSize );
	}

	mObjBox = Box3F(minPt, maxPt);
	MatrixF temp = getTransform();
	setTransform(temp);

	mBBObjToWorld.identity();
	Point3F boxScale = mObjBox.getExtents();
	boxScale.x = getMax(boxScale.x, 1.0f);
	boxScale.y = getMax(boxScale.y, 1.0f);
	boxScale.z = getMax(boxScale.z, 1.0f);
	mBBObjToWorld.scale(boxScale);
}

void MeshEmitter::addParticle(const F32 &vel)
{
	// This should never happen
	//  - But if it happens it will slow down the server.
	if(isServerObject())
		return;
	PROFILE_SCOPE(meshEmitAddPart);
   
   Particle* pNew = mParticleManager->AddParticle();

	// Set the time since this code was last run
	oldTime = mInternalClock;

	// Check if the emitMesh matches a name
	SimObject* SB = dynamic_cast<SimObject*>(Sim::findObject(emitMesh));
	// If not then check if it matches an ID
	if(!SB)
		SB = dynamic_cast<SimObject*>(Sim::findObject(atoi(emitMesh)));
	// We define these here so we can reduce the amount of dynamic_cast 'calls'
	ShapeBase* SS = NULL;
	TSStatic* TS = NULL;
	if(SB){
		SS = dynamic_cast<ShapeBase*>(SB);
		TS = dynamic_cast<TSStatic*>(SB);
	}
	// Make sure that we are dealing with some proper objects
	if(SB && (SS || TS)){

		bool coHandled = false;
		// -------------------------------------------------------------------------------------------------
		// -- Per vertex BEGIN -----------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------------
		if(!emitOnFaces)
		{
			coHandled = getPointOnVertex(SB, SS, TS, pNew);
		}
		// -------------------------------------------------------------------------------------------------
		// -- Per vertex END -------------------------------------------------------------------------------
		// -- Per triangle BEGIN ---------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------------
		if(emitOnFaces)
		{
			coHandled = getPointOnFace(SB, SS, TS, pNew);
		}
		// -------------------------------------------------------------------------------------------------
		// -- Per triangle END -----------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------------

		if(evenEmission && mainTime == U32_MAX)
			mainTime = 0;
		if(!evenEmission && !coHandled)
			mainTime = 0;

	}

	pNew->acc.set(0, 0, 0);
	pNew->currentAge = 0;

	// Choose a new particle datablack randomly from the list
	U32 dBlockIndex = gRandGen.randI() % mDataBlock->particleDataBlocks.size();
   mParticleManager->initializeParticle(pNew, pNew->vel, mDataBlock->particleDataBlocks[dBlockIndex]);
	updateKeyData( pNew );
}

bool MeshEmitter::getPointOnVertex(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew)
{
	PROFILE_SCOPE(meshEmitVertex);

	F32 initialVel = ejectionVelocity;
	initialVel    += (velocityVariance * 2.0f * gRandGen.randF()) - velocityVariance;
	// Seed the random generator - this should maybe be swapped out in favor for the gRandGen.
	//  - Although they both work fine.
	srand(mainTime);
	// Throw out some trash results. Apparently the first 1-2 results fails to give proper random numbers.
	int trash = rand();
	trash = rand();
	// Set our count value to mainTime.
	U32 co = mainTime;
	// If evenEmission is on, set the co to a random number for the per vertex emission.
	if(evenEmission && vertexCount != 0)
		co = gRandGen.randI() % vertexCount;
	mainTime++;

	const TSShapeInstance* model;
	TSShape* shape;
	if(SS)
		model = SS->getShapeInstance();
	else{
		model = TS->getShapeInstance();
	}
	shape = model->getShape();
	const TSShape::Detail& det = shape->details[model->getCurrentDetail()];
	S32 od = det.objectDetailNum;
	S32 start = shape->subShapeFirstObject[det.subShapeNum];
	S32 end   = start + shape->subShapeNumObjects[det.subShapeNum];
	for (S32 meshIndex = start; meshIndex < end; meshIndex++)
	{
		const TSShape::Object &obj = shape->objects[meshIndex];
		TSMesh* mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
		if(!mesh)
			continue;

		TSSkinMesh* sMesh = dynamic_cast<TSSkinMesh*>(mesh);
		//TSMesh::TSMeshVertexArray vertexList = shape->meshes[meshIndex]->mVertexData;
		S32 numVerts;
		numVerts = mesh->mVertexData.size();
		if (sMesh)
			numVerts = sMesh[meshIndex].mVertexData.size();

		if(!numVerts)
			continue;

		vertexCount = numVerts;

		if(co >= numVerts)
		{
			co -= numVerts;
			continue;
		}

		// Apparently, creating a TSMesh::TSMeshVertexArray vertexList instead of 
		//  - accessing the vertexdata directly can cause the program to crash on shutdown and startup.
		//  - It calls it's deconstructor when it goes out of scope. Seems like a bug.
		const Point3F *vertPos;
		const Point3F *vertNorm;
		if (sMesh)
		{
			vertPos = new const Point3F(sMesh->mVertexData[co].vert());
			vertNorm = new const Point3F(sMesh->mVertexData[co].normal());
		}
		else
		{
			vertPos = new const Point3F(mesh->mVertexData[co].vert());
			vertNorm = new const Point3F(mesh->mVertexData[co].normal());
		}

		// Get the transform of the object to get the transform matrix.
		//  - If it is a TSStatic we need to access the rootnode aswell
		//  - Which contains the rotation information.
		MatrixF trans;
		MatrixF nodetrans;
		MatrixF mat;
		if(SS)
		{
			trans = SS->getTransform();
		}
		else
		{
			trans = TS->getTransform();
			nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
			mat.mul(trans, nodetrans);
		}

		Point3F* p = new Point3F();
		if(SS)
		{
			trans.mulV(*vertPos,p);
         pNew->pos = SS->getPosition() + *p + (*vertNorm * ejectionOffset);
		}
		else{
			mat.mulV((*vertPos * TS->getScale()),p);
         pNew->pos = TS->getPosition() + *p + (*vertNorm * ejectionOffset);
		}
		// Set the relative position for later use.
		pNew->relPos = *p +(*vertNorm * ejectionOffset);
		// Velocity is based on the normal of the vertex
		pNew->vel = *vertNorm * initialVel;
      pNew->orientDir = *vertNorm;

		// Clean up
		delete(*p);
		delete(*vertPos);
		// Exit the loop
		return true;
	}
	return false;
}

bool MeshEmitter::getPointOnFace(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew)
{
	PROFILE_SCOPE(meshEmitFace);

	F32 initialVel = ejectionVelocity;
	initialVel    += (velocityVariance * 2.0f * gRandGen.randF()) - velocityVariance;
	// Seed the random generator - this should maybe be swapped out in favor for the gRandGen.
	//  - Although they both work fine.
	srand(mainTime);
	// Throw out some trash results. Apparently the first 1-2 results fails to give proper random numbers.
	int trash = rand();
	trash = rand();
	// Set our count value to mainTime.
	U32 co = mainTime;
	// If evenEmission is on, set the co to a random number for the per vertex emission.
	if(evenEmission && vertexCount != 0)
		co = gRandGen.randI() % vertexCount;
	mainTime++;
	const TSShapeInstance* model;
	TSShape* shape;
	if(SS)
		model = SS->getShapeInstance();
	else{
		model = TS->getShapeInstance();
	}
	shape = model->getShape();
	const TSShape::Detail& det = shape->details[model->getCurrentDetail()];
	S32 od = det.objectDetailNum;
	S32 start = shape->subShapeFirstObject[det.subShapeNum];
	S32 end   = start + shape->subShapeNumObjects[det.subShapeNum];

#pragma region perTriangle
	S32 meshIndex;

	TSSkinMesh* sMesh = NULL;
	TSMesh* Mesh = NULL;
	bool accepted = false;
	bool skinmesh = false;
	for(meshIndex = start; meshIndex < end; meshIndex++)
	{
		const TSShape::Object &obj = shape->objects[meshIndex];
		TSMesh* mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
		sMesh = dynamic_cast<TSSkinMesh*>(mesh);
		if(sMesh)
		{
			if(sMesh->mVertexData.size()){
				skinmesh = true;
				break;
			}
		}
	}
	// We don't want to run with partly skinmesh, partly static mesh.
	//  - So here we filter out skinmeshes from static meshes.
	while(!accepted)
	{
		accepted = false;
		// Pick a random mesh and test it.
		//  - This prevents the uneven emission from 
		//  - being as linear as it is with per vertex.
		meshIndex = (rand() % (end - start)) + start;
		const TSShape::Object &obj = shape->objects[meshIndex];
		TSMesh* Mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
		if(Mesh)
			accepted = true;
		if(skinmesh)
		{
			sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
			if(sMesh)
			{
				if(sMesh->mVertexData.size()){
					accepted = true;
					skinmesh = true;
				}
				else
					accepted = false;
			}
			else
				accepted = false;
		}
		if(!skinmesh && Mesh)
		{
			if(Mesh->verts.size() > 0)
				accepted = true;
			if(Mesh->verts.size() <= 0)
				accepted = false;
			if(Mesh->mVertexData.size() > 0)
				accepted = true;
			if(Mesh->mVertexData.size() <= 0)
				accepted = false;
		}
	}
	if(!evenEmission)
	{
		PROFILE_SCOPE(meshEmitOdd);
#pragma region skinMesh
		if(skinmesh)
		{
			Vector<Point3F> normList = shape->meshes[meshIndex]->norms;
			if (sMesh)
			{
				S32 numVerts = sMesh->mVertexData.size();
				if(numVerts)
				{
					S32 numPrims = sMesh->primitives.size();
					if(numPrims)
					{
						S32 numIndices = sMesh->indices.size();
						if(numIndices)
						{
							// Get a random primitive
							S32 primIndex = rand() % numPrims;
							S32 start = sMesh->primitives[primIndex].start;
							S16 numElements = sMesh->primitives[primIndex].numElements;

							// Define some variables we will use later
							TSMesh::__TSMeshVertexBase v1, v2, v3;
							Point3F vp1;

							// Test if the primitive is a triangle. Which it should be.
							//  - Theres no handler for other primitives than triangles,
							//  - if such is needed email me at LukasPJ@FuzzyVoidStudio.com
							if ( (shape->meshes[meshIndex]->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
							{
								// Get a random triangle
								U32 triStart = (rand() % (numElements/3));
								// This is not really necessary due to the way we handle the
								//  - triangles, but it is an useful snippet for modifications!
								//  - due to some rendering thing, every other triangle is 
								//  - counter clock wise. Read about it in the official DX9 docs.
								U8 indiceBool = (triStart * 3) % 2;
								if(indiceBool == 0)
								{
									v1 = sMesh->mVertexData[sMesh->indices[start + (triStart*3)]];
									v2 = sMesh->mVertexData[sMesh->indices[start + (triStart*3) + 1]];
									v3 = sMesh->mVertexData[sMesh->indices[start + (triStart*3) + 2]];
								}
								else
								{
									v3 = sMesh->mVertexData[sMesh->indices[start + (triStart*3)]];
									v2 = sMesh->mVertexData[sMesh->indices[start + (triStart*3) + 1]];
									v1 = sMesh->mVertexData[sMesh->indices[start + (triStart*3) + 2]];
									/*
									v1
									v3
									v2
									*/
								}
								// Create 2 vectors from the 3 points that make up the triangle
								Point3F p1 = v1.vert();
								Point3F p2 = v2.vert();
								Point3F p3 = v3.vert();
								Point3F vec1;
								Point3F vec2;
								vec1 = p2-p1;
								vec2 = p3-p2;
								// Get 2 random coefficients
								F32 K1 = rand() % 1000 + 1;
								F32 K2 = rand() % 1000 + 1;
								Point3F planeVec;
								// If the point is outside of the triangle, mirror it in so it fits
								//  - into the triangle. This is for a perfectly even result on a 
								//  - per face basis.
								if(K2 <= K1)
									planeVec = p1 + (vec1 * (K1 / 1000)) + (vec2 * (K2 / 1000));
								else
									planeVec = p1 + (vec1 * (1-(K1 / 1000))) + (vec2 * (1-(K2 / 1000)));

								// Add up the normals of the three vertices and normalize them to get
								//  - the correct normal of the plane.
								Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
								normalV->normalize();

								// Get the transform of the object to get the transform matrix.
								//  - If it is a TSStatic we need to access the rootnode aswell
								//  - Which contains the rotation information.
								MatrixF trans;
								MatrixF nodetrans;
								MatrixF mat;
								if(SS)
								{
									trans = SS->getTransform();
								}
								else
								{
									trans = TS->getTransform();
									nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
									mat.mul(trans, nodetrans);
								}

								Point3F* p = new Point3F();

								if(SS)
								{
									trans.mulV(planeVec,p);
                           pNew->pos = SS->getPosition() + *p + (*normalV * ejectionOffset);
								}
								else{
									mat.mulV((*planeVec * TS->getScale()),p);
                           pNew->pos = TS->getPosition() + *p + (*normalV * ejectionOffset);
								}
								delete(*p);
								delete(*normalV);
								return true;
							}
							else
							{
								if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Fan) 
									Con::warnf("Was a fan DrawPrimitive not TrisDrawPrimitive");
								else if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Strip) 
									Con::warnf("Was a Strip DrawPrimitive not TrisDrawPrimitive");
								else
									Con::warnf("Couldn't determine primitive type");
							}
						}
					}
				}
			}
		}
#pragma endregion
#pragma region staticmesh
		// Same procedure as above
		if(!skinmesh)
		{
			if(Mesh)
			{
				S32 numVerts = Mesh->mVertexData.size();
				if(numVerts)
				{
					S32 numPrims = Mesh->primitives.size();
					if(numPrims)
					{
						S32 numIndices = Mesh->indices.size();
						if(numIndices)
						{
							S32 primIndex = rand() % numPrims;
							S32 start = Mesh->primitives[primIndex].start;
							S16 numElements = Mesh->primitives[primIndex].numElements;

							TSMesh::__TSMeshVertexBase v1, v2, v3;
							Point3F vp1;

							if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
							{
								U32 triStart = (rand() % (numElements/3));
								U8 indiceBool = (triStart * 3) % 2;
								if(indiceBool == 0)
								{
									v1 = Mesh->mVertexData[Mesh->indices[start + (triStart*3)]];
									v2 = Mesh->mVertexData[Mesh->indices[start + (triStart*3) + 1]];
									v3 = Mesh->mVertexData[Mesh->indices[start + (triStart*3) + 2]];
								}
								else
								{
									v3 = Mesh->mVertexData[Mesh->indices[start + (triStart*3)]];
									v2 = Mesh->mVertexData[Mesh->indices[start + (triStart*3) + 1]];
									v1 = Mesh->mVertexData[Mesh->indices[start + (triStart*3) + 2]];
									/*
									v1
									v3
									v2
									*/
								}
								Point3F p1 = v1.vert();
								Point3F p2 = v2.vert();
								Point3F p3 = v3.vert();
								Point3F vec1;
								Point3F vec2;
								vec1 = p2-p1;
								vec2 = p3-p2;
								F32 K1 = rand() % 1000 + 1;
								F32 K2 = rand() % 1000 + 1;
								Point3F planeVec;
								if(K2 <= K1)
									planeVec = p1 + (vec1 * (K1 / 1000)) + (vec2 * (K2 / 1000));
								else
									planeVec = p1 + (vec1 * (1-(K1 / 1000))) + (vec2 * (1-(K2 / 1000)));

								// Construct a vector from the 3 results
								const Point3F *vertPos = new const Point3F(planeVec);

								Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
								normalV->normalize();

								MatrixF trans;
								MatrixF nodetrans;
								MatrixF mat;
								if(SS)
								{
									trans = SS->getTransform();
								}
								else
								{
									trans = TS->getTransform();
									nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
									mat.mul(trans, nodetrans);
								}
								// Rotate our point by the rotation matrix
								Point3F* p = new Point3F();

								if(SS)
								{
									trans.mulV(*vertPos,p);
                           pNew->pos = SS->getPosition() + *p + (*normalV * ejectionOffset);
								}
								else{
									mat.mulV((*vertPos * TS->getScale()),p);
                           pNew->pos = TS->getPosition() + *p + (*normalV * ejectionOffset);
								}
								return true;
							}
							else
							{
								if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Fan) 
									Con::warnf("Was a fan DrawPrimitive not TrisDrawPrimitive");
								else if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Strip) 
									Con::warnf("Was a Strip DrawPrimitive not TrisDrawPrimitive");
								else
									Con::warnf("Couldn't determine primitive type");
							}
						}
					}
				}
			}
		}
#pragma endregion
	}
	if(evenEmission)
	{
		if(emitfaces.size())
		{
			PROFILE_SCOPE(meshEmitEven);
			// Get a random face from our emitfaces vector.
			//  - then follow basically the same procedure as above.
			//  - Just slightly simplified
			S32 faceIndex = rand() % emitfaces.size();
			IPSCore::face tris = emitfaces[faceIndex];
			const TSShape::Object &obj = shape->objects[tris.meshIndex];
			TSMesh* Mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
			sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
			TSMesh::__TSMeshVertexBase v1, v2, v3;
			Point3F p1, p2, p3;
			if(tris.skinMesh)
			{
				sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
				U8 indiceBool = tris.triStart % 2;
				if(indiceBool == 0)
				{
					v1 = sMesh->mVertexData[sMesh->indices[tris.triStart]];
					v2 = sMesh->mVertexData[sMesh->indices[tris.triStart + 1]];
					v3 = sMesh->mVertexData[sMesh->indices[tris.triStart + 2]];
				}
				else
				{
					v3 = sMesh->mVertexData[sMesh->indices[tris.triStart]];
					v2 = sMesh->mVertexData[sMesh->indices[tris.triStart + 1]];
					v1 = sMesh->mVertexData[sMesh->indices[tris.triStart + 2]];
				}
				p1 = v1.vert();
				p2 = v2.vert();
				p3 = v3.vert();
			}
			else{
				U8 indiceBool = tris.triStart % 2;
				if(indiceBool == 0)
				{
					v1 = Mesh->mVertexData[Mesh->indices[tris.triStart]];
					v2 = Mesh->mVertexData[Mesh->indices[tris.triStart + 1]];
					v3 = Mesh->mVertexData[Mesh->indices[tris.triStart + 2]];
				}
				else
				{
					v3 = Mesh->mVertexData[Mesh->indices[tris.triStart]];
					v2 = Mesh->mVertexData[Mesh->indices[tris.triStart + 1]];
					v1 = Mesh->mVertexData[Mesh->indices[tris.triStart + 2]];
				}
				p1 = v1.vert();
				p2 = v2.vert();
				p3 = v3.vert();
			}
			Point3F vec1;
			Point3F vec2;
			vec1 = p2-p1;
			vec2 = p3-p2;
			F32 K1 = rand() % 1000 + 1;
			F32 K2 = rand() % 1000 + 1;
			Point3F planeVec;
			if(K2 <= K1)
				planeVec = p1 + (vec1 * (K1 / 1000)) + (vec2 * (K2 / 1000));
			else
				planeVec = p1 + (vec1 * (1-(K1 / 1000))) + (vec2 * (1-(K2 / 1000)));
			// Construct a vector from the 3 results
			const Point3F *vertPos = new const Point3F(planeVec);

			Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
			normalV->normalize();

			MatrixF trans;
			MatrixF nodetrans;
			MatrixF mat;
			if(SS)
			{
				trans = SS->getTransform();
			}
			else
			{
				trans = TS->getTransform();
				nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
				mat.mul(trans, nodetrans);
			}
			// Rotate our point by the rotation matrix
			Point3F* p = new Point3F();

			if(SS)
			{
				trans.mulV(*vertPos,p);
            pNew->pos = SS->getPosition() + *p + (*normalV * ejectionOffset);
			}
			else{
				mat.mulV((*vertPos * TS->getScale()),p);
				mat.mulV(*normalV);
            pNew->pos = TS->getPosition() + *p + (*normalV * ejectionOffset);
			}
			pNew->relPos = *p +(*normalV * ejectionOffset);
			pNew->vel = *normalV * initialVel;
         pNew->orientDir = *normalV;
			delete(*p);
			delete(*vertPos);
			delete(*normalV);
			return true;
		}
	}
	return false;
#pragma endregion
}

//-----------------------------------------------------------------------------
// processTick
//-----------------------------------------------------------------------------
void MeshEmitter::processTick(const Move* move)
{
	Parent::processTick(move);
	isObjectCulled = false;
	// If the object has been culled out we don't want to render the particles. 
	// If it haven't, set the bounds to global so the particles will always be rendered regardless of whether the object is seen or not.
	if(isObjectCulled)
	{
		mGlobalBounds = false;

		if( mSceneManager )
			mSceneManager->notifyObjectDirty( this );

	}
	else
	{
		setGlobalBounds();

		// LOD
		if(isClientObject() && !mDeleteOnTick && !mDead && mDataBlock)
		{
			if(Con::getBoolVariable("$IPS::ParticleLOD"))
			{
				// Check if the emitMesh matches a name
				SceneObject* SB = dynamic_cast<SceneObject*>(Sim::findObject(emitMesh));

				// If not then check if it matches an ID
				if(!SB)
					SB = dynamic_cast<SceneObject*>(Sim::findObject(atoi(emitMesh)));
				GameConnection* gConnection = GameConnection::getLocalClientConnection();
				MatrixF camTrans;
				gConnection->getControlCameraTransform(0, &camTrans);
				F32 dist = (SB->getPosition() - camTrans.getPosition()).len();
				if(SB->getWorldBox().isValidBox())
					dist = SB->getWorldBox().getDistanceToPoint(camTrans.getPosition());
				if(dist >= mDataBlock->SimulationLODBegin)
				{
					F32 lodCoeff = (dist - mDataBlock->SimulationLODBegin) / (mDataBlock->SimulationLODEnd - mDataBlock->SimulationLODBegin);
					MaxUpdateIndex = lodCoeff * 100;
					if(MaxUpdateIndex == 0)
						MaxUpdateIndex = 1;
					if(MaxUpdateIndex > 100)
						MaxUpdateIndex = U32_MAX;
				}
				if(dist >= mDataBlock->ejectionLODStartDistance)
				{
					S32 ejPeriod;
					ejPeriod = mDataBlock->ejectionPeriodMS;
					if(dist >= mDataBlock->ejectionLODEndDistance)
						mActive = false;
					else
						mActive = true;
					F32 lodCoeff = (dist - mDataBlock->ejectionLODStartDistance) / (mDataBlock->ejectionLODEndDistance - mDataBlock->ejectionLODStartDistance);
					RenderEjectionPeriodMS = ejPeriod + ((mDataBlock->lodEjectionPeriod - ejPeriod) * lodCoeff);
				}
				else
					RenderEjectionPeriodMS = mDataBlock->ejectionPeriodMS;
			}
			else
				RenderEjectionPeriodMS = mDataBlock->ejectionPeriodMS;
		}
	}
}

//-----------------------------------------------------------------------------
// advanceTime
//-----------------------------------------------------------------------------
void MeshEmitter::advanceTime(F32 dt)
{
	if( dt < 0.00001 ) return;

	Parent::advanceTime(dt);

	if( dt > 0.5 ) dt = 0.5;

	if( mDead ) return;

	mElapsedTimeMS += (S32)(dt * 1000.0f);

	U32 numMSToUpdate = (U32)(dt * 1000.0f);
	if( numMSToUpdate == 0 ) return;

	// TODO: Prefetch

	// remove dead particles
   mParticleManager->DeleteDeadParticles(numMSToUpdate);

	AssertFatal( mParticleManager->n_parts >= 0, "MeshEmitter: negative part count!" );

	if (mParticleManager->n_parts < 1 && mDeleteWhenEmpty)
	{
		mDeleteOnTick = true;
		return;
	}

	if( numMSToUpdate != 0 && mParticleManager->n_parts > 0 )
	{
		update( numMSToUpdate );
	}
	emitParticles(ejectionVelocity, (U32)(dt * 1000.0f));
}

//-----------------------------------------------------------------------------
// Update key related particle data
//-----------------------------------------------------------------------------
void MeshEmitter::updateKeyData( Particle *part, U32 numMilliseconds )
{
   mParticleManager->UpdateKeyData(part, numMilliseconds);
}

//-----------------------------------------------------------------------------
// Update particles
//-----------------------------------------------------------------------------
void MeshEmitter::update( U32 ms )
{
	// Check if the emitMesh matches a name
	SceneObject* SB = dynamic_cast<SceneObject*>(Sim::findObject(emitMesh));
	// If not then check if it matches an ID
	if(!SB)
		SB = dynamic_cast<SceneObject*>(Sim::findObject(atoi(emitMesh)));
	if(SB)
		parentNodePos = SB->getRenderPosition();
	// TODO: Prefetch
	if(Con::getBoolVariable("$IPS::ParticleLOD"))
	{
		if(MaxUpdateIndex < 1000)
		{
			if(CurrentUpdateIndex == 0)
				CurrentUpdateIndex = 1;
			if(CurrentUpdateIndex < MaxUpdateIndex)
			{
				CurrentUpdateIndex++;
				TSU += ms;
				return;
			}
			if(CurrentUpdateIndex >= MaxUpdateIndex)
			{
				CurrentUpdateIndex = 1;
				ms += TSU;
				TSU = 0;
			}
		}
		else
			return;
	}
	PROFILE_SCOPE(MeshEmitUpdate);
	SimDataBlock** BHVs = getDataBlock()->ParticleBHVs;
	//WHAT SO MANY SORTS PER SECOND? WTF DO A DIRTY FLAG!
	std::sort(BHVs, BHVs + ParticleBehaviourCount);

	for (Particle* part = mParticleManager->part_list_head.next; part != NULL; part = part->next)
	{
		F32 t = F32(ms) / 1000.0;
		part->acc.zero();

		for(int i = 0; i < ParticleBehaviourCount; i++)
		{
			IParticleBehaviour* bhv = (IParticleBehaviour*)BHVs[i];
			if(!bhv)
				continue;
			if(bhv->getType() == behaviourType::Acceleration)
				bhv->updateParticle(this, part, t);
		}

		Point3F a = part->acc;
		a -= part->vel * part->dataBlock->dragCoefficient;
		a -= mWindVelocity * part->dataBlock->windCoefficient;
		a += Point3F(0.0f, 0.0f, -9.81f) * part->dataBlock->gravityCoefficient;
		part->vel += a * t;

		for(int i = 0; i < ParticleBehaviourCount; i++)
		{
			IParticleBehaviour* bhv = (IParticleBehaviour*)BHVs[i];
			if(!bhv)
				continue;
			if(bhv->getType() == behaviourType::Velocity)
				bhv->updateParticle(this, part, t);
		}
		part->relPos += part->vel * t;
      part->pos = part->pos + part->vel * t;
		for(int i = 0; i < ParticleBehaviourCount; i++)
		{
			IParticleBehaviour* bhv = (IParticleBehaviour*)BHVs[i];
			if(!bhv)
				continue;
			if(bhv->getType() == behaviourType::Position)
				bhv->updateParticle(this, part, t);
		}

		updateKeyData( part, ms );
	}
}

//-----------------------------------------------------------------------------
// Copy particles to vertex buffer
//-----------------------------------------------------------------------------

// structure used for particle sorting.
struct SortParticle
{
	Particle* p;
	F32       k;
};

// qsort callback function for particle sorting
int QSORT_CALLBACK m_cmpSortParticles(const void* p1, const void* p2)
{
	const SortParticle* sp1 = (const SortParticle*)p1;
	const SortParticle* sp2 = (const SortParticle*)p2;

	if (sp2->k > sp1->k)
		return 1;
	else if (sp2->k == sp1->k)
		return 0;
	else
		return -1;
}

//-----------------------------------------------------------------------------
// reload
// Not changed
//-----------------------------------------------------------------------------
bool MeshEmitterData::reload()
{
	// Clear out current particle data.

	dataBlockIds.clear();
	particleDataBlocks.clear();

	// Parse out particle string.

	U32 numUnits = 0;
	if( particleString )
		numUnits = StringUnit::getUnitCount( particleString, " \t" );
	if( !particleString || !particleString[ 0 ] || !numUnits )
	{
		Con::errorf( "MeshEmitterData(%s) has an empty particles string.", getName() );
		mReloadSignal.trigger();
		return false;
	}

	for( U32 i = 0; i < numUnits; ++ i )
	{
		const char* dbName = StringUnit::getUnit( particleString, i, " \t" );

		ParticleData* data = NULL;
		if( !Sim::findObject( dbName, data ) )
		{
			Con::errorf( ConsoleLogEntry::General, "MeshEmitterData(%s) unable to find particle datablock: %s", getName(), dbName );
			continue;
		}

		particleDataBlocks.push_back( data );
		dataBlockIds.push_back( data->getId() );
	}

	// Check that we actually found some particle datablocks.

	if( particleDataBlocks.empty() )
	{
		Con::errorf( ConsoleLogEntry::General, "MeshEmitterData(%s) unable to find any particle datablocks", getName() );
		mReloadSignal.trigger();
		return false;
	}

	// Trigger reload.

	mReloadSignal.trigger();

	return true;
}

//-----------------------------------------------------------------------------
// packUpdate
// Changed
//-----------------------------------------------------------------------------
U32 MeshEmitter::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if ( stream->writeFlag( mask & InitialUpdateMask ) )
	{
		mathWrite(*stream, getTransform());
		mathWrite(*stream, getScale());
	}

   if( stream->writeFlag( mask & GeometryMask ) )
	{
		stream->writeString(emitMesh);
      stream->writeFlag( mask & SkipReloadMask );
	}
   
   if( stream->writeFlag( mask & MeshEmitterMask ) )
	{
      stream->writeFlag(mActive);
		stream->writeFlag(evenEmission);
		stream->writeFlag(emitOnFaces);
		stream->writeFlag(enableDebugRender);
	}

	if( stream->writeFlag( mask & particleMask ) )
	{
		stream->write(ejectionPeriodMS);
		stream->write(periodVarianceMS);
		stream->write(ejectionVelocity);
		stream->write(velocityVariance);
		stream->write(ejectionOffset);
		stream->write(lifetimeMS);
		stream->write(softnessDistance);
		stream->write(ambientFactor);
		stream->write(overrideAdvance);
		stream->write(orientParticles);
		stream->write(orientOnVelocity);
		stream->write(useEmitterSizes);
		stream->write(useEmitterColors);
		mathWrite(*stream, alignDirection);
		stream->write(blendStyle);
		stream->write(sortParticles);
		stream->write(reverseOrder);
		stream->write(highResOnly);
		stream->write(renderReflection);
	}

	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
// Changed
//-----------------------------------------------------------------------------
void MeshEmitter::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);

	if ( stream->readFlag() )
	{
		MatrixF temp;
		Point3F tempScale;
		mathRead(*stream, &temp);
		mathRead(*stream, &tempScale);

		setScale(tempScale);
		setTransform(temp);
	}

   // Geometry mask
   if( stream->readFlag() )
   {
		char buf[256];
		stream->readString(buf);
		emitMesh = dStrdup(buf);
      if(!stream->readFlag())
		loadFaces();
   }

	// Meshemitter mask
	if ( stream->readFlag() )
	{
      mActive = stream->readFlag();
		evenEmission = stream->readFlag();
		emitOnFaces = stream->readFlag();
		enableDebugRender = stream->readFlag();
	}

	// Particles mask
	if( stream->readFlag() )
	{
		stream->read(&ejectionPeriodMS);
		stream->read(&periodVarianceMS);
		stream->read(&ejectionVelocity);
		stream->read(&velocityVariance);
		stream->read(&ejectionOffset);
		stream->read(&lifetimeMS);
		stream->read(&softnessDistance);
		stream->read(&ambientFactor);
		stream->read(&overrideAdvance);
		stream->read(&orientParticles);
		stream->read(&orientOnVelocity);
		stream->read(&useEmitterSizes);
		stream->read(&useEmitterColors);
		mathRead(*stream, &alignDirection);
		stream->read(&blendStyle);
		stream->read(&sortParticles);
		stream->read(&reverseOrder);
		stream->read(&highResOnly);
		stream->read(&renderReflection);
	}
}

//-----------------------------------------------------------------------------
// onStaticModified
// Added
//-----------------------------------------------------------------------------
void MeshEmitter::onStaticModified(const char* slotName, const char*newValue)
{
   if(strcmp(slotName, "emitMesh") == 0)
      setMaskBits(GeometryMask);

	// Was it an sa_value that was edited? Then set the emitterEdited bit.
	if(strcmp(slotName, "evenEmission") == 0||
		strcmp(slotName, "enableDebugRender") == 0||
		strcmp(slotName, "emitOnFaces") == 0)
      setMaskBits(MeshEmitterMask);

	if(	strcmp(slotName, "ejectionPeriodMS") == 0 ||
		strcmp(slotName, "periodVarianceMS") == 0 ||
		strcmp(slotName, "ejectionVelocity") == 0 ||
		strcmp(slotName, "velocityVariance") == 0 ||
		strcmp(slotName, "ejectionOffset") == 0 ||
		strcmp(slotName, "lifetimeMS") == 0 ||
		strcmp(slotName, "softnessDistance") == 0 ||
		strcmp(slotName, "ambientFactor") == 0 ||
		strcmp(slotName, "overrideAdvance") == 0 ||
		strcmp(slotName, "orientParticles") == 0 ||
		strcmp(slotName, "orientOnVelocity") == 0 ||
		strcmp(slotName, "useEmitterSizes") == 0 ||
		strcmp(slotName, "useEmitterColors") == 0 ||
		strcmp(slotName, "alignParticles") == 0 ||
		strcmp(slotName, "alignDirection") == 0 ||
		strcmp(slotName, "blendStyle") == 0 ||
		strcmp(slotName, "sortParticles") == 0 ||
		strcmp(slotName, "reverseOrder") == 0 ||
		strcmp(slotName, "textureName") == 0 ||
		strcmp(slotName, "textureHandle") == 0 ||
		strcmp(slotName, "highResOnly") == 0 ||
		strcmp(slotName, "renderReflection") == 0)
		setMaskBits(particleMask);


	if(!isProperlyAdded())
	{
		//Save the values and add them onNewDataBlock
		initialValues.push_back(String(slotName));
		initialValues.push_back(String(newValue));
	}
}

//-----------------------------------------------------------------------------
// inspectPostApply
// Not changed
//-----------------------------------------------------------------------------
void MeshEmitter::inspectPostApply()
{
	Parent::inspectPostApply();
	setMaskBits( StateMask );
}

//-----------------------------------------------------------------------------
// loadFaces
//  - This function calculates the area of all the triangles in the mesh
//  - finds the average area, and adds the triangles one or more times
//  - to the emitfaces vector based on their area relative to the other faces
//  - not a perfect way to do it, but it works fine.
// Custom
//-----------------------------------------------------------------------------
void MeshEmitter::loadFaces()
{
	SimObject* SB = dynamic_cast<SimObject*>(Sim::findObject(emitMesh));
	if(!SB)
		SB = dynamic_cast<SimObject*>(Sim::findObject(atoi(emitMesh)));
	ShapeBase* SS = NULL;
	TSStatic* TS = NULL;
	if(SB){
		SS = dynamic_cast<ShapeBase*>(SB);
		TS = dynamic_cast<TSStatic*>(SB);
	}
	// Make sure that we are dealing with some proper objects
	if(SB && (SS || TS)){
		loadFaces(SB, SS, TS);
	}
}

void MeshEmitter::loadFaces(SimObject *SB, ShapeBase *SS, TSStatic* TS)
{
	PROFILE_SCOPE(MeshEmitLoadFaces);
	emitfaces.clear();
	vertexCount = 0;
	if(SB && (SS || TS)){
		TSShapeInstance* model;
		if(SS)
			model = SS->getShapeInstance();
		else{
			model = TS->getShapeInstance();
		}
		std::vector<IPSCore::face> triangles;
		bool skinmesh = false;
		for(S32 meshIndex = 0; meshIndex < model->getShape()->meshes.size(); meshIndex++)
		{
			TSSkinMesh* sMesh = dynamic_cast<TSSkinMesh*>(model->getShape()->meshes[meshIndex]);
			if(sMesh)
			{
				if(sMesh->mVertexData.size()){
					skinmesh = true;
					break;
				}
			}
		}
		TSShape* shape = model->getShape();
		const TSShape::Detail& det = shape->details[model->getCurrentDetail()];
		S32 od = det.objectDetailNum;
		S32 start = shape->subShapeFirstObject[det.subShapeNum];
		S32 end = start + shape->subShapeNumObjects[det.subShapeNum];
		for (S32 meshIndex = start; meshIndex < end; meshIndex++)  
		{
			TSSkinMesh* sMesh;

			const TSShape::Object &obj = shape->objects[meshIndex];
			TSMesh* Mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;

			sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
			if(!Mesh)
				continue;

			S32 numVerts = Mesh->mVertexData.size();
			if(!numVerts)
				continue;
			vertexCount += numVerts;

			S32 numPrims = Mesh->primitives.size();
			if(!numPrims)
				continue;

			S32 numIndices = Mesh->indices.size();
			if(!numIndices)
				continue;
			for( U32 primIndex = 0; primIndex < numPrims; primIndex++ )
			{
				U32 start = Mesh->primitives[primIndex].start;

				U32 numElements = Mesh->primitives[primIndex].numElements;

				if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles) 
				{

					TSMesh::__TSMeshVertexBase v1, v2, v3;
					Point3F p1, p2, p3;

					for (U32 triIndex = 0; triIndex < numElements; triIndex+=3)
					{
						U32 triStart = start + triIndex;
						v1 = Mesh->mVertexData[Mesh->indices[triStart]];
						v2 = Mesh->mVertexData[Mesh->indices[triStart + 1]];
						v3 = Mesh->mVertexData[Mesh->indices[triStart + 2]];

						Point3F scale;
						if(SS)
							scale = SS->getScale();
						else
							scale = (dynamic_cast<TSStatic*>(SB))->getScale();
						p1 = v1.vert() * scale;
						p2 = v2.vert() * scale;
						p3 = v3.vert() * scale;

						Point3F veca = p1 - p2;
						Point3F vecb = p2 - p3;
						Point3F vecc = p3 - p1;

						IPSCore::face tris;
						tris.meshIndex = meshIndex;
						tris.skinMesh = skinmesh;
						tris.triStart = triStart;
						tris.area = IPSCore::HeronsF(veca, vecb, vecc);
						triangles.push_back(tris);
					}
				}
				else
				{
					if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Fan) 
						Con::warnf("Was a fan DrawPrimitive not TrisDrawPrimitive");
					else if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Strip) 
						Con::warnf("Was a Strip DrawPrimitive not TrisDrawPrimitive");
					else
						Con::warnf("Couldn't determine primitive type");
				}
			}
		}

		F32 averageArea = 0;
		for(S32 index = 0; index < triangles.size(); index++)
		{
			averageArea += triangles[index].area;
		}
		averageArea = averageArea / triangles.size();
      // Reserve some space to prevent numerous
      //  reallocs, which takes a lot of time.
      // Note: Each face is 224 bits, memory
      //  usage can quickly raise to the skies.
      emitfaces.reserve(triangles.size() * 2);
		for(S32 index = 0; index < triangles.size(); index++)
		{
			float n = triangles[index].area / averageArea;
			float t;
			t = n - floor(n);
			if (t >= 0.5)    
				n = ceil(n);
			else 
				n = floor(n);
			for(int i = -1; i < n; i++)
			{
				emitfaces.push_back(triangles[index]);
			}
		}
	}
}

DefineEngineMethod(MeshEmitterData, reload, void,(),,
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
