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
	"@brief Contains additional data to be associated with a GroundEmitterNode."
	);

ConsoleDocClass( ParticleEffect,
	"@brief A particle emitter object that can be positioned in the world and "
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
		"" );

	addField("lifeTimeMS", TYPEID< S32 >(), Offset(lifeTimeMS,   ParticleEffectData),
		"" );

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
}

//-----------------------------------------------------------------------------
// unpackData
// Recieve data
//-----------------------------------------------------------------------------
void ParticleEffectData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
}

//-----------------------------------------------------------------------------
// onStaticModified
// 
//-----------------------------------------------------------------------------
void ParticleEffectData::onStaticModified(const char* slotName, const char*newValue)
{
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
					if(!eN.node->registerObject())
					{
						delete eN.node;
						eN.node = 0;
					}
					eN.node->addToScene();
					eN.node->setPosition(getPosition());
					((ParticleEmitterNode*)eN.node)->setActive(false);
					for(int idx = 0; idx < eN.emitterData.values.size(); idx++)
					{
						eN.emitterData.values[idx].initialValue = getValue(eN, eN.emitterData.values[idx].type);
						Con::printf("Value %i = %f", idx, eN.emitterData.values[idx].initialValue);
					}
					((loc_SphereEmitterNode*)eN.node)->standAloneEmitter = true;
					break;
				}
			case pEffectReader::emitterType::GraphEmitter:
				{
					eN.node = new loc_GraphEmitterNode();
					GraphEmitterNodeData *nodeDat = dynamic_cast<GraphEmitterNodeData*>(Sim::findObject(emitter.datablock.c_str()));
					GraphEmitterData *emitDat = dynamic_cast<GraphEmitterData*>(Sim::findObject(emitter.emitterblock.c_str()));
					eN.node->onNewDataBlock(nodeDat, true);
					((loc_GraphEmitterNode*)eN.node)->setEmitterDataBlock(emitDat);
					if(!eN.node->registerObject())
					{
						delete eN.node;
						eN.node = 0;
					}
					eN.node->addToScene();
					eN.node->setPosition(getPosition());
					((loc_GraphEmitterNode*)eN.node)->setActive(false);
					pEffectReader::value val1 = eN.emitterData.values[0];
					for(int idx = 0; idx < eN.emitterData.values.size(); idx++)
					{
						eN.emitterData.values[idx].initialValue = getValue(eN, eN.emitterData.values[idx].type);
					}
					((loc_GraphEmitterNode*)eN.node)->Loop = true;
					break;
				}
			case pEffectReader::emitterType::MeshEmitter:
				{
					/*eN.node = new MeshEmitter();
					MeshEmitterData *nodeDat = dynamic_cast<MeshEmitterData*>(Sim::findObject(emitter.datablock.c_str()));
					eN.node->onNewDataBlock(nodeDat, true);
					if(!eN.node->registerObject())
					{
					delete eN.node;
					eN.node = 0;
					}
					eN.node->addToScene();
					eN.node->setPosition(getPosition());*/
					//((MeshEmitter*)eN.node)->setActive(false);
					break;
				}
			case pEffectReader::emitterType::RadiusMeshEmitter:
				{
					/*eN.node = new RadiusMeshEmitter();
					RadiusMeshEmitterData *nodeDat = dynamic_cast<RadiusMeshEmitterData*>(Sim::findObject(emitter.datablock.c_str()));
					eN.node->onNewDataBlock(nodeDat, true);
					if(!eN.node->registerObject())
					{
					delete eN.node;
					eN.node = 0;
					}
					eN.node->addToScene();
					eN.node->setPosition(getPosition());*/
					//((MaskEmitterNode*)eN.node)->setActive(false);
					break;
				}
			case pEffectReader::emitterType::GroundEmitter:
				{
					eN.node = new loc_GroundEmitterNode();
					GroundEmitterNodeData *nodeDat = dynamic_cast<GroundEmitterNodeData*>(Sim::findObject(emitter.datablock.c_str()));
					GroundEmitterData *emitDat = dynamic_cast<GroundEmitterData*>(Sim::findObject(emitter.emitterblock.c_str()));
					eN.node->onNewDataBlock(nodeDat, true);
					((loc_GroundEmitterNode*)eN.node)->setEmitterDataBlock(emitDat);
					if(!eN.node->registerObject())
					{
						delete eN.node;
						eN.node = 0;
					}
					eN.node->addToScene();
					eN.node->setPosition(getPosition());
					((GroundEmitterNode*)eN.node)->setActive(false);
					for(int idx = 0; idx < eN.emitterData.values.size(); idx++)
					{
						eN.emitterData.values[idx].initialValue = getValue(eN, eN.emitterData.values[idx].type);
					}
					((GroundEmitterNode*)eN.node)->standAloneEmitter = true;
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
					for(int idx = 0; idx < eN.emitterData.values.size(); idx++)
					{
						eN.emitterData.values[idx].initialValue = getValue(eN, eN.emitterData.values[idx].type);
					}
					((MaskEmitterNode*)eN.node)->standAloneEmitter = true;
					break;
				}
			case pEffectReader::emitterType::PathEmitter:
				//mEmitterNodes.push_back(emitterNode(ParticleEmitterNode(), emitter));
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
		if(emitter->state == emitterNode::notActivated && timeSpent >= mDataBlock->lifeTimeMS * emitter->emitterData.start){
			emitter->state = emitterNode::Activated; setValue(emitter, pEffectReader::Active, 1);}
		if(emitter->state == emitterNode::Activated && timeSpent >= mDataBlock->lifeTimeMS * emitter->emitterData.end){
			emitter->state = emitterNode::hasActivated; setValue(emitter, pEffectReader::Active, 0); }
		if(emitter->state == emitterNode::Activated)
		{
			for(int idv = 0; idv < emitter->emitterData.values.size(); idv++)
			{
				pEffectReader::value val = emitter->emitterData.values[idv];
				if(val.ease)
				{
					pEffectReader::point theP;
					theP.x = -1;
					for(int idp = 0; idp < val.points.size(); idp++)
					{
						pEffectReader::point p = val.points[idp];
						if(p.x >= (F32)timeSpent / (F32)mDataBlock->lifeTimeMS){
							theP = p; break;}
					}
					if(theP.x >= 0)
					{
						EaseF ease = EaseF(pEffectReader::inOutCompose(theP.easeIn, theP.easeOut), pEffectReader::stringToEase(const_cast<char*>(theP.easing.c_str())));
						// t: current time, b: beginning value, c: change in value, d: duration
						F32 t = timeSpent - (mDataBlock->lifeTimeMS * emitter->emitterData.start);
						F32 b = val.initialValue;
						F32 c = val.DeltaValue;
						F32 d = mDataBlock->lifeTimeMS * (emitter->emitterData.end - emitter->emitterData.start);
						setValue(emitter, val.type, ease.getValue(t,b,c,d));
					}
				}
				else
					if(timeSpent >= mDataBlock->lifeTimeMS * val.setTime)
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

void ParticleEffect::setValue(emitterNode *node, pEffectReader::valueType value, F32 newValue)
{
	PROFILE_SCOPE(pEffectSetValue);
	switch(node->emitterData.type)
	{
	case pEffectReader::emitterType::stockEmitter:
		switch(value)
		{
		case pEffectReader::xPosition:
			node->emitterData.x = F32(newValue);
			return;
		case pEffectReader::yPosition:
			node->emitterData.y = newValue;
			return;
		case pEffectReader::zPosition:
			node->emitterData.y = newValue;
			return;
		case pEffectReader::Active:
			((ParticleEmitterNode*)node->node)->setActive(newValue);
			return;
		case pEffectReader::EjectionPeriod:
			((SphereEmitterNode*)node->node)->sa_ejectionPeriodMS = newValue;
			return;
		case pEffectReader::EjectionOffset:
			((SphereEmitterNode*)node->node)->sa_ejectionOffset = newValue;
			return;
		}
		break;
	case pEffectReader::emitterType::GraphEmitter:
		switch(value)
		{
		case pEffectReader::UpperBoundary:
			((GraphEmitterNode*)node->node)->funcMax = newValue;
			return;
		case pEffectReader::LowerBoundary:
			((GraphEmitterNode*)node->node)->funcMin = newValue;
			return;
		case pEffectReader::TimeScale:
			((GraphEmitterNode*)node->node)->timeScale = newValue;
			return;
		case pEffectReader::xPosition:
			node->emitterData.x = newValue;
			return;
		case pEffectReader::yPosition:
			node->emitterData.y = newValue;
			return;
		case pEffectReader::zPosition:
			node->emitterData.y = newValue;
			return;
		case pEffectReader::Active:
			((GraphEmitterNode*)node->node)->setActive(newValue);
			return;
		case pEffectReader::EjectionPeriod:
			((GraphEmitterNode*)node->node)->sa_ejectionPeriodMS = newValue;
			return;
		case pEffectReader::EjectionOffset:
			((GraphEmitterNode*)node->node)->sa_ejectionOffset = newValue;
			return;
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
			//((GroundEmitterNode*)node->node)->sa_radius = newValue;
			return;
		case pEffectReader::xPosition:
			node->emitterData.x = newValue;
			return;
		case pEffectReader::yPosition:
			node->emitterData.y = newValue;
			return;
		case pEffectReader::zPosition:
			node->emitterData.y = newValue;
			return;
		case pEffectReader::Active:
			((GroundEmitterNode*)node->node)->setActive(newValue);
			return;
		case pEffectReader::EjectionPeriod:
			((GroundEmitterNode*)node->node)->sa_ejectionPeriodMS = newValue;
			return;
		case pEffectReader::EjectionOffset:
			((GroundEmitterNode*)node->node)->sa_ejectionOffset = newValue;
			return;
		}
		break;
	case pEffectReader::emitterType::MaskEmitter:
		switch(value)
		{
		case pEffectReader::Scale:
			((MaskEmitterNode*)node->node)->sa_radius = newValue;
			return;
		case pEffectReader::xPosition:
			node->emitterData.x = newValue;
			return;
		case pEffectReader::yPosition:
			node->emitterData.y = newValue;
			return;
		case pEffectReader::zPosition:
			node->emitterData.y = newValue;
			return;
		case pEffectReader::Active:
			((MaskEmitterNode*)node->node)->setActive(newValue);
			return;
		case pEffectReader::EjectionPeriod:
			((MaskEmitterNode*)node->node)->sa_ejectionPeriodMS = newValue;
			return;
		case pEffectReader::EjectionOffset:
			((MaskEmitterNode*)node->node)->sa_ejectionOffset = newValue;
			return;
		}
		break;
	case pEffectReader::emitterType::PathEmitter:
		/*if(value.compare("Travelspeed"))
		return ((GraphEmitterNode*)node.node)->timeScale;*/
		break;
	}
}

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
	}
	switch(node.emitterData.type)
	{
	case pEffectReader::emitterType::stockEmitter:
		switch(value)
		{
		case pEffectReader::Active:
			return ((SphereEmitterNode*)node.node)->getActive();
		case pEffectReader::EjectionPeriod:
			return ((SphereEmitterNode*)node.node)->sa_ejectionPeriodMS;
		case pEffectReader::EjectionOffset:
			return ((SphereEmitterNode*)node.node)->sa_ejectionOffset;
		}
		break;
	case pEffectReader::emitterType::GraphEmitter:
		switch(value)
		{
		case pEffectReader::UpperBoundary:
			return ((GraphEmitterNode*)node.node)->funcMax;
		case pEffectReader::LowerBoundary:
			return ((GraphEmitterNode*)node.node)->funcMin;
		case pEffectReader::TimeScale:
			return ((GraphEmitterNode*)node.node)->timeScale;
		case pEffectReader::Active:
			return ((MaskEmitterNode*)node.node)->getActive();
		case pEffectReader::EjectionPeriod:
			return ((GraphEmitterNode*)node.node)->sa_ejectionPeriodMS;
		case pEffectReader::EjectionOffset:
			return ((GraphEmitterNode*)node.node)->sa_ejectionOffset;
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
			return ((GroundEmitterNode*)node.node)->sa_radius;
		case pEffectReader::Active:
			return ((MaskEmitterNode*)node.node)->getActive();
		case pEffectReader::EjectionPeriod:
			return ((GroundEmitterNode*)node.node)->sa_ejectionPeriodMS;
		case pEffectReader::EjectionOffset:
			return ((GroundEmitterNode*)node.node)->sa_ejectionOffset;
		}
		break;
	case pEffectReader::emitterType::MaskEmitter:
		switch(value)
		{
		case pEffectReader::Scale:
			return ((MaskEmitterNode*)node.node)->sa_radius;
		case pEffectReader::Active:
			return ((MaskEmitterNode*)node.node)->getActive();
		case pEffectReader::EjectionPeriod:
			return ((MaskEmitterNode*)node.node)->sa_ejectionPeriodMS;
		case pEffectReader::EjectionOffset:
			return ((MaskEmitterNode*)node.node)->sa_ejectionOffset;
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