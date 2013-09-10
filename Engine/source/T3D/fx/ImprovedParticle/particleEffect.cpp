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

#include "ParticleEffect.h"
#include "../particleEmitterNode.h"
#include "../particleEmitter.h"
#include "sphereEmitterNode.h"
#include "meshEmitter.h"
#include "radiusMeshEmitter.h"
#include "groundEmitterNode.h"
#include "groundEmitter.h"
#include "maskEmitterNode.h"
#include "maskEmitter.h"
#include "graphEmitterNode.h"
#include "graphEmitter.h"
#include "IPSCore.h"
#include "localNodes.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "math/mathIO.h"
#include "sim/netConnection.h"
#include "console/engineAPI.h"
#ifndef _PEFFECTREADER_H_
#include "core\util\xml\pEffectReader.h"
#endif

IMPLEMENT_CO_DATABLOCK_V1(ParticleEffectData);
IMPLEMENT_CO_NETOBJECT_V1(ParticleEffect);

//*****************************************************************************
// Documentation
//*****************************************************************************
ConsoleDocClass( ParticleEffectData,
	"@brief Contains the path to the .pEffect file and the lifeTime of a ParticleEffect."
	);

ConsoleDocClass( ParticleEffect,
	"@brief An object which spawns local emitters and interpolates specific values."
	);
//*****************************************************************************
// --Documentation
//*****************************************************************************

//*****************************************************************************
// ParticleEffectData
//*****************************************************************************
ParticleEffectData::ParticleEffectData()
{
	// Instantiate values
	pEffectPath = "";
	lifeTimeMS = 0;
}

ParticleEffectData::~ParticleEffectData()
{
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void ParticleEffectData::initPersistFields()
{
	// Init them here
	Parent::initPersistFields();
	addGroup( "GraphEmitterData" );

	addField("pEffect", TypeFilename, Offset(pEffectPath,   ParticleEffectData),
		"The path to the .pEffect file that defines this ParticleEffect." );

	addField("lifeTimeMS", TYPEID< S32 >(), Offset(lifeTimeMS,   ParticleEffectData),
		"Lifetime of the ParticlEffect" );

	endGroup( "GraphEmitterData" );
}

//-----------------------------------------------------------------------------
// onAdd
// Validate the initial values here.
//-----------------------------------------------------------------------------
bool ParticleEffectData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if(pEffectPath != "")
		pEffectReader::readFile(Torque::Path(pEffectPath), emitters);

	return true;
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool ParticleEffectData::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;

	if(pEffectPath != "")
		pEffectReader::readFile(Torque::Path(pEffectPath), emitters);
	else return false;

	return true;
}

//-----------------------------------------------------------------------------
// packData
// Transmit data
//-----------------------------------------------------------------------------
void ParticleEffectData::packData(BitStream* stream)
{
	Parent::packData(stream);
   if (stream->writeFlag(pEffectPath && pEffectPath[0]))
      stream->writeString(pEffectPath);
   stream->writeInt(lifeTimeMS, 16);
}

//-----------------------------------------------------------------------------
// unpackData
// Recieve data
//-----------------------------------------------------------------------------
void ParticleEffectData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
   pEffectPath = (stream->readFlag()) ? stream->readSTString() : 0;
   lifeTimeMS = stream->readInt(16);
}

//-----------------------------------------------------------------------------
// onStaticModified
// 
//-----------------------------------------------------------------------------
void ParticleEffectData::onStaticModified(const char* slotName, const char*newValue)
{
	// Did we change which .pEffect file this effect uses? Then update it...
	// Nah just kidding, this makes no sense and is not tested.
	if(strcmp(slotName, "pEffect") == 0)
	{
		if(pEffectPath != "")
			pEffectReader::readFile(Torque::Path(pEffectPath), emitters);
	}
}

//*****************************************************************************
// --ParticleEffectData
//*****************************************************************************

//*****************************************************************************
// ParticleEffect
//*****************************************************************************
ParticleEffect::ParticleEffect()
{
	// Instantiate values
	hasSynced = false;
}

ParticleEffect::~ParticleEffect()
{
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void ParticleEffect::initPersistFields()
{
	// Init them here
	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onAdd
// Validate the initial values here.
// All local emitters is spawned here aswell
//-----------------------------------------------------------------------------
bool ParticleEffect::onAdd()
{
	if( !Parent::onAdd() )
		return false;
	PROFILE_SCOPE(pEffectOnAdd);

	mObjBox.minExtents.set(-0.5, -0.5, -0.5);
	mObjBox.maxExtents.set( 0.5,  0.5,  0.5);
	resetWorldBox();
	addToScene();
	startTime = Platform::getVirtualMilliseconds();
	mEmitterNodes.clear();
	if(isServerObject())
		return true;
	// Spawn local emitters if it is a client side call.
	for(int i = 0; i < mDataBlock->emitters.size(); i++)
	{
		pEffectReader::emitter emitter = mDataBlock->emitters[i];
		emitterNode eN;
		eN.node = 0;
		eN.emitterData = emitter;
		eN.state = emitterNode::notActivated;
		if(!isServerObject())
		{
			switch(emitter.type)
			{
			case pEffectReader::emitterType::stockEmitter:
				{
					eN.node = new loc_SphereEmitterNode();
					ParticleEmitterNodeData *nodeDat = dynamic_cast<ParticleEmitterNodeData*>(Sim::findObject(emitter.datablock.c_str()));
					ParticleEmitterData *emitDat = dynamic_cast<ParticleEmitterData*>(Sim::findObject(emitter.emitterblock.c_str()));
					eN.node->onNewDataBlock(nodeDat, true);
					((loc_SphereEmitterNode*)eN.node)->setEmitterDataBlock(emitDat);
					// If spawn failed, clean up the node
					if(!eN.node->registerObject())
					{
						delete eN.node;
						eN.node = 0;
					}
					eN.node->addToScene();
					eN.node->setPosition(getPosition());
					((ParticleEmitterNode*)eN.node)->setActive(false);
					// Get the initial values
					for(int idx = 0; idx < eN.emitterData.values.size(); idx++)
					{
						eN.emitterData.values[idx].initialValue = getValue(eN, eN.emitterData.values[idx].type);
					}
					// We want to change the variables of the emitter and not the whole datablock, so we need it to be a SA emitter
					((loc_SphereEmitterNode*)eN.node)->getEmitter()->standAloneEmitter = true;
					break;
				}
			case pEffectReader::emitterType::GraphEmitter:
				{
					eN.node = new loc_GraphEmitterNode();
					GraphEmitterNodeData *nodeDat = dynamic_cast<GraphEmitterNodeData*>(Sim::findObject(emitter.datablock.c_str()));
					GraphEmitterData *emitDat = dynamic_cast<GraphEmitterData*>(Sim::findObject(emitter.emitterblock.c_str()));
					eN.node->onNewDataBlock(nodeDat, true);
					((loc_GraphEmitterNode*)eN.node)->setEmitterDataBlock(emitDat);
					// If spawn failed, clean up the node
					if(!eN.node->registerObject())
					{
						delete eN.node;
						eN.node = 0;
					}
					eN.node->addToScene();
					eN.node->setPosition(getPosition());
					((loc_GraphEmitterNode*)eN.node)->setActive(false);
					// Get the initial values
					for(int idx = 0; idx < eN.emitterData.values.size(); idx++)
					{
						eN.emitterData.values[idx].initialValue = getValue(eN, eN.emitterData.values[idx].type);
					}
					// We want to change the variables of the emitter and not the whole datablock, so we need it to be a SA emitter
					((loc_GraphEmitterNode*)eN.node)->getEmitter()->standAloneEmitter = true;
					break;
				}
			case pEffectReader::emitterType::MeshEmitter:
				{
					// MeshEmitter is not supported
					break;
				}
			case pEffectReader::emitterType::RadiusMeshEmitter:
				{
					// RadiusMeshEmitter is not supported
					break;
				}
			case pEffectReader::emitterType::GroundEmitter:
				{
					eN.node = new loc_GroundEmitterNode();
					GroundEmitterNodeData *nodeDat = dynamic_cast<GroundEmitterNodeData*>(Sim::findObject(emitter.datablock.c_str()));
					GroundEmitterData *emitDat = dynamic_cast<GroundEmitterData*>(Sim::findObject(emitter.emitterblock.c_str()));
					eN.node->onNewDataBlock(nodeDat, true);
					((loc_GroundEmitterNode*)eN.node)->setEmitterDataBlock(emitDat);
					// If spawn failed, clean up the node
					if(!eN.node->registerObject())
					{
						delete eN.node;
						eN.node = 0;
					}
					eN.node->addToScene();
					eN.node->setPosition(getPosition());
					((GroundEmitterNode*)eN.node)->setActive(false);
					// Get the initial values
					for(int idx = 0; idx < eN.emitterData.values.size(); idx++)
					{
						eN.emitterData.values[idx].initialValue = getValue(eN, eN.emitterData.values[idx].type);
					}
					// We want to change the variables of the emitter and not the whole datablock, so we need it to be a SA emitter
					((GroundEmitterNode*)eN.node)->getEmitter()->standAloneEmitter = true;
					break;
				}
			case pEffectReader::emitterType::MaskEmitter:
				{
					eN.node = new loc_MaskEmitterNode();
					MaskEmitterNodeData *nodeDat = dynamic_cast<MaskEmitterNodeData*>(Sim::findObject(emitter.datablock.c_str()));
					MaskEmitterData *emitDat = dynamic_cast<MaskEmitterData*>(Sim::findObject(emitter.emitterblock.c_str()));
					eN.node->onNewDataBlock(nodeDat, true);
					((loc_MaskEmitterNode*)eN.node)->setEmitterDataBlock(emitDat);
					if(!eN.node->registerObject())
					{
						delete eN.node;
						eN.node = 0;
					}
					eN.node->addToScene();
					eN.node->setPosition(getPosition());
					((MaskEmitterNode*)eN.node)->setActive(false);
					// Get the initial values
					for(int idx = 0; idx < eN.emitterData.values.size(); idx++)
					{
						eN.emitterData.values[idx].initialValue = getValue(eN, eN.emitterData.values[idx].type);
					}
					// We want to change the variables of the emitter and not the whole datablock, so we need it to be a SA emitter
					((MaskEmitterNode*)eN.node)->getEmitter()->standAloneEmitter = true;
					break;
				}
			case pEffectReader::emitterType::PathEmitter:
				// Planned emitter here for templating.
				break;
			}
		}
		mEmitterNodes.push_back(eN);
	}
	return true;
}

//-----------------------------------------------------------------------------
// onRemove
// Clean up the emitters before removing the effect.
//-----------------------------------------------------------------------------
void ParticleEffect::onRemove()
{
	removeFromScene();
	if( isClientObject() )
	{
		for(int i = 0; i < mEmitterNodes.size(); i++)
		{
			emitterNode node = mEmitterNodes[i];
			if(node.node != 0){
				node.node->deleteObject();
			}
		}
	}
	Parent::onRemove();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool ParticleEffect::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	mDataBlock = dynamic_cast<ParticleEffectData*>( dptr );
	if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;
   mLifeTimeMS = mDataBlock->lifeTimeMS;
	// Todo: Uncomment if this is a "leaf" class
	scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// inspectPostApply
//-----------------------------------------------------------------------------
void ParticleEffect::inspectPostApply()
{
	Parent::inspectPostApply();
	setMaskBits(StateMask);
}

//-----------------------------------------------------------------------------
// packData
// Transmit data
//-----------------------------------------------------------------------------
U32 ParticleEffect::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if ( stream->writeFlag( mask & InitialUpdateMask ) )
	{
		mathWrite(*stream, getTransform());
		mathWrite(*stream, getScale());
	}

	if( stream->writeFlag( mask & EmittersMask) )
	{
		// For some reason, if you will ever need it.. 
		//  - This snippet provides support for serverside emitters aswell.
		for(int i = 0; i < mEmitterNodes.size(); i++)
		{
			stream->writeFlag(true);
			if(mEmitterNodes[i].node == 0)
				stream->writeFlag(false);
			else{
				GameBase* bg = mEmitterNodes[i].node;
				S32 ghostIndex = con->getGhostIndex(bg);
				if(ghostIndex < 0){
					hasSynced = false;
					stream->writeFlag(false);
				}
				else{
					stream->writeFlag(true);
					stream->writeRangedU32(U32(con->getGhostIndex(mEmitterNodes[i].node)),0, NetConnection::MaxGhostCount);
				}
			}
		}
		stream->writeFlag(false);
	}
	stream->writeFlag( mask & resetMask );
	return retMask;
}

//-----------------------------------------------------------------------------
// unpackData
// Recieve data
//-----------------------------------------------------------------------------
void ParticleEffect::unpackUpdate(NetConnection* con, BitStream* stream)
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

	if(stream->readFlag())
	{
		U8 i = 0;
		while(stream->readFlag())
		{
			if(stream->readFlag())
				mEmitterNodes[i].node = dynamic_cast<GameBase*>(con->resolveGhost(stream->readRangedU32(0, NetConnection::MaxGhostCount)));
			i++;
		}
		hasSynced = true;
	}
	// Did the server tell the client to reset the emitter?
	// - This is effectively a commandToClient call.
	if(stream->readFlag())
	{
		for(int idx = 0; idx < mEmitterNodes.size(); idx++)
		{
			emitterNode *eN = &mEmitterNodes[idx];
			eN->state = eN->notActivated;
			for(int ivx = 0; ivx < eN->emitterData.values.size(); ivx++)
			{
				setValue(eN,eN->emitterData.values[ivx].type, eN->emitterData.values[ivx].initialValue);
			}
			if(eN->emitterData.type == pEffectReader::emitterType::GraphEmitter){
            GraphEmitter* graphEmitter = ((GraphEmitterNode*)eN->node)->getEmitter();
				if(graphEmitter->Reverse)
					graphEmitter->particleProg = graphEmitter->funcMax;
				else
					graphEmitter->particleProg = graphEmitter->funcMin;
			}
		}
		startTime = Platform::getVirtualMilliseconds();
	}
}

//-----------------------------------------------------------------------------
// processTick
//-----------------------------------------------------------------------------
void ParticleEffect::processTick(const Move* move)
{
	Parent::processTick(move);
	bool server = false;
	if(isServerObject())
		server = true;
	else
		server = false;

	mObjBox.minExtents = Point3F(-0.5, -0.5, -0.5);
	mObjBox.maxExtents = Point3F(0.5, 0.5, 0.5);
	resetWorldBox();

	if(isServerObject() && isProperlyAdded() && !hasSynced){
		setMaskBits(EmittersMask);
		hasSynced = true;
	}

	// Update the transformation of the emitters
	for(int i = 0; i < mEmitterNodes.size(); i++)
	{
		emitterNode node = mEmitterNodes[i];
		if(node.node != NULL){
			Point3F nodePos = Point3F(node.emitterData.x, node.emitterData.y, node.emitterData.z);
			MatrixF mat = getTransform();
			mat.mulP(nodePos);
			mat.setPosition(nodePos);
			node.node->setTransform(mat);
		}
	}

	if ( isMounted() )
	{
		MatrixF mat;
		mMount.object->getMountTransform( mMount.node, mMount.xfm, &mat );
		setTransform( mat );
	}
}

//-----------------------------------------------------------------------------
// advanceTime
// Oh a lot of things happens here.
// First of all we manage the state of the emitters, should they be turned on
//  - off or just left as is?
// Then we update the values on the emitter.
//-----------------------------------------------------------------------------
void ParticleEffect::advanceTime(F32 dt)
{
	PROFILE_SCOPE(pEffectAdvanceTime);
	//IPSBenchmarkBegin;
	Parent::advanceTime(dt);
	U32 timeSpent = Platform::getVirtualMilliseconds() - startTime;
	if(!mDataBlock || !isClientObject())
		return;
	for(int i = 0; i < mEmitterNodes.size(); i++)
	{
		emitterNode *emitter = &mEmitterNodes[i];
		if(emitter->node == NULL || !emitter->node->isProperlyAdded())
			continue;
		// Set the state of the emitter, turn it on/off if necessary.
		// Emitters lifetime is relative to the ParticleEffects lifetime.
		if(emitter->state == emitterNode::notActivated && timeSpent >= mLifeTimeMS * emitter->emitterData.start){
			emitter->state = emitterNode::Activated; setValue(emitter, pEffectReader::Active, 1);}
		if(emitter->state == emitterNode::Activated && timeSpent >= mLifeTimeMS * emitter->emitterData.end){
			emitter->state = emitterNode::hasActivated; setValue(emitter, pEffectReader::Active, 0); }
		// Is the emitter still active? Update it's values!
		if(emitter->state == emitterNode::Activated)
		{
			for(int idv = 0; idv < emitter->emitterData.values.size(); idv++)
			{
				pEffectReader::value val = emitter->emitterData.values[idv];
				if(val.ease)
				{
					pEffectReader::point theP, prevP;
					theP.x = -1;
					prevP.x = -1;
					// Get best points
					for(int idp = 0; idp < val.points.size(); idp++)
					{
						pEffectReader::point p = val.points[idp];
						if(p.x >= (F32)timeSpent / (F32)mLifeTimeMS){
							theP = p; 
							if(idp >= 1)
								prevP = val.points[idp-1]; 
							break;
						}
					}
					// If we didn't get any valid points, theP.x would still be -1.
					if(theP.x >= 0)
					{
						// Convert the values in pEffectReader to values that EaseF understands
						EaseF ease = EaseF(pEffectReader::inOutCompose(theP.easeIn, theP.easeOut), pEffectReader::stringToEase(const_cast<char*>(theP.easing.c_str())));
						// t: current time, b: beginning value, c: change in value, d: duration
						// This is complicated...
						// ES = EmitterStart, EE = EmitterEnd, PS = PointStart (prevP.x), PE = PointEnd (theP.x), TS = TimeSpent, LT = LifeTime
						// TS is the total time spent for the whole duration, we need to subtract ES so that we get the time spent since the emitter started.
						// ES is a relative value however, relative to LT so we need to multiply it by LT
						// TS - (LT * ES)
						// But this is only for the emitter, now we need to get the time spent relative to the point, EE - ES is the range that spans over the
						//  - emitters lifetime, so put adding EE - ES to ES we get EE, now we need to multiply this range by PS to get how much time have passed
						//  - by relative to the point
						// TS - (LT * (ES + ((EE - ES) * PS)))
						F32 t = timeSpent - (mLifeTimeMS * (emitter->emitterData.start + ((emitter->emitterData.end - emitter->emitterData.start) * (prevP.x >= 0 ? prevP.x : 0))));
						// The point begins at initialValue + prevP.y, which is a value relative to DeltaValue so we add the relative value to the initial value.
						F32 b = ((prevP.x >= 0 ? prevP.y : 0) * val.DeltaValue) + val.initialValue;
						F32 c = (theP.y * val.DeltaValue) - b + val.initialValue;
						// Very similar to calculation of t, get the non-relative lifetime of the emitter by 
						// LT * ((ES + ((EE - ES) * PS) - (ES + ((EE - ES) * PE))))
						F32 d = mLifeTimeMS * (
							(emitter->emitterData.start + ((emitter->emitterData.end - emitter->emitterData.start) * theP.x)) -
							(emitter->emitterData.start + ((emitter->emitterData.end - emitter->emitterData.start) * (prevP.x >= 0 ? prevP.x : 0)))
							);
						// Set the value
						setValue(emitter, val.type, ease.getValue(t,b,c,d));
					}
				}
				else // If the value shouldn't be eased, we only have to watch whether we've reached setTime or not.
					if(timeSpent >= mLifeTimeMS * val.setTime)
						setValue(emitter, val.type, val.DeltaValue);
			}
		}
		if(emitter->state == emitterNode::hasActivated)
		{
			//Stop emitting particles and stuff
		}
	}
	//IPSBenchmarkEnd("----pEffect---- advanceTime");
}

//-----------------------------------------------------------------------------
// setValue
// Sets the given value
// Lots of switches here to set the correct value efficiently.
//-----------------------------------------------------------------------------
void ParticleEffect::setValue(emitterNode *node, pEffectReader::valueType value, F32 newValue)
{
	PROFILE_SCOPE(pEffectSetValue);

	switch(value)
	{
		case pEffectReader::xPosition:
			node->emitterData.x = F32(newValue);
			return;
		case pEffectReader::yPosition:
			node->emitterData.y = newValue;
			return;
		case pEffectReader::zPosition:
			node->emitterData.z = newValue;
			return;
		case pEffectReader::Active:
         ((ParticleEmitterNode*)node->node)->setActive(newValue);
			return;
		case pEffectReader::EjectionPeriod:
			((ParticleEmitterNode*)node->node)->getEmitter()->sa_ejectionPeriodMS = newValue >= 1 ? newValue : 1;
			return;
		case pEffectReader::EjectionOffset:
			((ParticleEmitterNode*)node->node)->getEmitter()->sa_ejectionOffset = newValue;
			return;
	}

	switch(node->emitterData.type)
	{
	case pEffectReader::emitterType::stockEmitter:
		switch(value)
		{
      default:
         break;
		}
		break;
	case pEffectReader::emitterType::GraphEmitter:
		switch(value)
		{
		case pEffectReader::UpperBoundary:
			((GraphEmitterNode*)node->node)->getEmitter()->funcMax = newValue;
			return;
		case pEffectReader::LowerBoundary:
			((GraphEmitterNode*)node->node)->getEmitter()->funcMin = newValue;
			return;
		case pEffectReader::TimeScale:
			((GraphEmitterNode*)node->node)->getEmitter()->timeScale = newValue;
			return;
		}
		break;
	case pEffectReader::emitterType::MeshEmitter:
		break;
	case pEffectReader::emitterType::RadiusMeshEmitter:
		break;
	case pEffectReader::emitterType::GroundEmitter:
		switch(value)
		{
		case pEffectReader::Radius:
         ((GroundEmitterNode*)node->node)->getEmitter()->sa_radius = newValue;
			return;
		}
		break;
	case pEffectReader::emitterType::MaskEmitter:
		switch(value)
		{
		case pEffectReader::Scale:
			((MaskEmitterNode*)node->node)->getEmitter()->sa_Radius = newValue;
			return;
		case pEffectReader::Treshold_max:
			((MaskEmitterNode*)node->node)->getEmitter()->sa_Treshold_max = newValue;
			return;
		case pEffectReader::Treshold_min:
			((MaskEmitterNode*)node->node)->getEmitter()->sa_Treshold_min = newValue;
			return;
		}
		break;
	case pEffectReader::emitterType::PathEmitter:
		/*if(value.compare("Travelspeed"))
		return ((GraphEmitterNode*)node.node)->timeScale;*/
		break;
	}
}

//-----------------------------------------------------------------------------
// setValue
// Gets the given value.
//----------------------------------------------------------------------------
F32 ParticleEffect::getValue(emitterNode node, pEffectReader::valueType value)
{
	PROFILE_SCOPE(pEffectGetValue);
	switch(value)
	{
	case pEffectReader::xPosition:
		return node.emitterData.x;
	case pEffectReader::yPosition:
		return node.emitterData.y;
	case pEffectReader::zPosition:
		return node.emitterData.z;
	case pEffectReader::Active:
		return ((ParticleEmitterNode*)node.node)->getActive();
	case pEffectReader::EjectionPeriod:
		return ((ParticleEmitterNode*)node.node)->getEmitter()->sa_ejectionPeriodMS;
	case pEffectReader::EjectionOffset:
		return ((ParticleEmitterNode*)node.node)->getEmitter()->sa_ejectionOffset;
	}
	switch(node.emitterData.type)
	{
	case pEffectReader::emitterType::stockEmitter:
		switch(value)
		{
		}
		break;
	case pEffectReader::emitterType::GraphEmitter:
		switch(value)
		{
		case pEffectReader::UpperBoundary:
			return ((GraphEmitterNode*)node.node)->getEmitter()->funcMax;
		case pEffectReader::LowerBoundary:
			return ((GraphEmitterNode*)node.node)->getEmitter()->funcMin;
		case pEffectReader::TimeScale:
			return ((GraphEmitterNode*)node.node)->getEmitter()->timeScale;
		}
		break;
	case pEffectReader::emitterType::MeshEmitter:
		break;
	case pEffectReader::emitterType::RadiusMeshEmitter:
		//if(value.compare("Radius") == 0)
		//return ((RadiusMeshEmitter*)node.node)->radius;
		break;
	case pEffectReader::emitterType::GroundEmitter:
		switch(value)
		{
		case pEffectReader::Radius:
         return ((GroundEmitterNode*)node.node)->getEmitter()->sa_radius;
		}
		break;
	case pEffectReader::emitterType::MaskEmitter:
		switch(value)
		{
		case pEffectReader::Scale:
			return ((MaskEmitterNode*)node.node)->getEmitter()->sa_Radius;
		case pEffectReader::Treshold_max:
			return ((MaskEmitterNode*)node.node)->getEmitter()->sa_Treshold_max;
		case pEffectReader::Treshold_min:
			return ((MaskEmitterNode*)node.node)->getEmitter()->sa_Treshold_min;
		}
		break;
	case pEffectReader::emitterType::PathEmitter:
		/*if(value.compare("Travelspeed"))
		return ((GraphEmitterNode*)node.node)->timeScale;*/
		break;
	}
	return NULL;
}

//*****************************************************************************
// --ParticleEffect
//*****************************************************************************

//*****************************************************************************
// Engine methods
//*****************************************************************************

DefineConsoleMethod(ParticleEffect,dumpEmitters,void,(),,"")
{
	Point3F pos = object->getPosition();
	Con::printf("Position: %f %f %f",pos.x,pos.y,pos.z);
	for(int i = 0; i < object->mEmitterNodes.size(); i++)
	{
		if(object->mEmitterNodes[i].node != 0)
		{
			pos = object->mEmitterNodes[i].node->getPosition();
			Con::printf("Position[%i]: %f %f %f",i,pos.x,pos.y,pos.z);
		}
	}
	for(int i = 0; i < object->mEmitterNodes.size(); i++)
	{
		if(object->mEmitterNodes[i].node != 0)
		{
			object->mEmitterNodes[i].node->setPosition(object->getPosition());
			pos = object->mEmitterNodes[i].node->getPosition();
			Con::printf("Position[%i]: %f %f %f",i,pos.x,pos.y,pos.z);
		}
	}
}

DefineConsoleMethod(ParticleEffect,reset,void,(),,"")
{
	object->setMaskBits( object->resetMask );
}

//*****************************************************************************
// --Engine methods
//*****************************************************************************