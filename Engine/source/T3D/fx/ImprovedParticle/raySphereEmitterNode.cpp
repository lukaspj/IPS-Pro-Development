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

#include "raySphereEmitterNode.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

#include "console/engineAPI.h"
#include "console\simPersistID.h"
#include "math/mTransform.h"

IMPLEMENT_CO_DATABLOCK_V1(RaySphereEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(RaySphereEmitterNode);

ConsoleDocClass( RaySphereEmitterNodeData,
	"@brief Contains additional data to be associated with a ParticleEmitterNode."
	"@ingroup FX\n"
	);

ConsoleDocClass( RaySphereEmitterNode,
	"@brief A particle emitter object that can be positioned in the world and "
	"dynamically enabled or disabled.\n\n"

	"@tsexample\n"
	"datablock ParticleEmitterNodeData( SimpleEmitterNodeData )\n"
	"{\n"
	"   timeMultiple = 1.0;\n"
	"};\n\n"

	"%emitter = new ParticleEmitterNode()\n"
	"{\n"
	"   datablock = SimpleEmitterNodeData;\n"
	"   active = true;\n"
	"   emitter = FireEmitterData;\n"
	"   velocity = 3.5;\n"
	"};\n\n"

	"// Dynamically change emitter datablock\n"
	"%emitter.setEmitterDataBlock( DustEmitterData );\n"
	"@endtsexample\n"

	"@note To change the emitter field dynamically (after the ParticleEmitterNode "
	"object has been created) you must use the setEmitterDataBlock() method or the "
	"change will not be replicated to other clients in the game.\n"
	"Similarly, use the setActive() method instead of changing the active field "
	"directly. When changing velocity, you need to toggle setActive() on and off "
	"to force the state change to be transmitted to other clients.\n\n"

	"@ingroup FX\n"
	"@see ParticleEmitterNodeData\n"
	"@see ParticleEmitterData\n"
	);

RaySphereEmitterNodeData::RaySphereEmitterNodeData()
{
	sa_thetaMin         = 0.0f;   // All heights
	sa_thetaMax         = 90.0f;

	sa_phiReferenceVel  = 0.0f;   // All directions
	sa_phiVariance      = 180.0f;
}

void RaySphereEmitterNodeData::initPersistFields()
{
	addGroup( "Independent emitters" );

	addField( "sa_thetaMin", TYPEID< F32 >(), Offset(sa_thetaMin, RaySphereEmitterNodeData),
		"Minimum angle, from the horizontal plane, to eject from." );

	addField( "sa_thetaMax", TYPEID< F32 >(), Offset(sa_thetaMax, RaySphereEmitterNodeData),
		"Maximum angle, from the horizontal plane, to eject particles from." );

	addField( "sa_phiReferenceVel", TYPEID< F32 >(), Offset(sa_phiReferenceVel, RaySphereEmitterNodeData),
		"Reference angle, from the vertical plane, to eject particles from." );

	addField( "sa_phiVariance", TYPEID< F32 >(), Offset(sa_phiVariance, RaySphereEmitterNodeData),
		"Variance from the reference angle, from 0 - 360." );

	endGroup( "Independent emitters" );
	
	Parent::initPersistFields();
}

bool RaySphereEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	//Validate variables here

	return true;
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void RaySphereEmitterNodeData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->writeRangedU32((U32)sa_thetaMin, 0, 180);
	stream->writeRangedU32((U32)sa_thetaMax, 0, 180);
	stream->writeRangedU32((U32)sa_phiReferenceVel, 0, 360);
	stream->writeRangedU32((U32)sa_phiVariance, 0, 360);
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void RaySphereEmitterNodeData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	sa_thetaMin = (F32)stream->readRangedU32(0, 180);
	sa_thetaMax = (F32)stream->readRangedU32(0, 180);
	sa_phiReferenceVel = (F32)stream->readRangedU32(0, 360);
	sa_phiVariance = (F32)stream->readRangedU32(0, 360);
}

//-----------------------------------------------------------------------------
// SphereEmitterNode
//-----------------------------------------------------------------------------
RaySphereEmitterNode::RaySphereEmitterNode()
{
	sa_thetaMin         = 0.0f;   // All heights
	sa_thetaMax         = 90.0f;

	sa_phiReferenceVel  = 0.0f;   // All directions
	sa_phiVariance      = 180.0f;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void RaySphereEmitterNode::initPersistFields()
{
	addField( "emitter",  TYPEID< RaySphereEmitterData >(), Offset(mEmitterDatablock, RaySphereEmitterNode),
		"Datablock to use when emitting particles." );

	addGroup( "Independent emitters" );

	addField( "sa_thetaMin", TYPEID< F32 >(), Offset(sa_thetaMin, RaySphereEmitterNode),
		"Minimum angle, from the horizontal plane, to eject from." );

	addField( "sa_thetaMax", TYPEID< F32 >(), Offset(sa_thetaMax, RaySphereEmitterNode),
		"Maximum angle, from the horizontal plane, to eject particles from." );

	addField( "sa_phiReferenceVel", TYPEID< F32 >(), Offset(sa_phiReferenceVel, RaySphereEmitterNode),
		"Reference angle, from the vertical plane, to eject particles from." );

	addField( "sa_phiVariance", TYPEID< F32 >(), Offset(sa_phiVariance, RaySphereEmitterNode),
		"Variance from the reference angle, from 0 - 360." );

	endGroup( "Independent emitters" );
	
	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool RaySphereEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	setmDataBlock(dynamic_cast<RaySphereEmitterNodeData*>( dptr ));
	RaySphereEmitterNodeData* DataBlock = getDataBlock();
	if ( !DataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	sa_thetaMin = DataBlock->sa_thetaMin;
	sa_thetaMax = DataBlock->sa_thetaMax;

	sa_phiReferenceVel = DataBlock->sa_phiReferenceVel;
	sa_phiVariance = DataBlock->sa_phiVariance;

	for(int i = 0; i < initialValues.size(); i=i+2)
	{
		if(strcmp("sa_phiReferenceVel",initialValues[i].c_str()) == 0)
			sa_phiReferenceVel = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_phiVariance",initialValues[i].c_str()) == 0)
			sa_phiVariance = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_thetaMax",initialValues[i].c_str()) == 0)
			sa_thetaMax = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_thetaMin",initialValues[i].c_str()) == 0)
			sa_thetaMin = atof(const_cast<char*>(initialValues[i+1].c_str()));
	}

	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 RaySphereEmitterNode::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if( stream->writeFlag( mask & emitterEdited) )
	{
		stream->writeRangedU32((U32)sa_thetaMin, 0, 180);
		stream->writeRangedU32((U32)sa_thetaMax, 0, 180);
		stream->writeRangedU32((U32)sa_phiReferenceVel, 0, 360);
		stream->writeRangedU32((U32)sa_phiVariance, 0, 360);
	}

	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void RaySphereEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);
	if ( stream->readFlag() )
	{
		sa_thetaMin = (F32)stream->readRangedU32(0, 180);
		sa_thetaMax = (F32)stream->readRangedU32(0, 180);
		sa_phiReferenceVel = (F32)stream->readRangedU32(0, 360);
		sa_phiVariance = (F32)stream->readRangedU32(0, 360);
	}
	if(mEmitter && saUpdateBits != 0)
		UpdateEmitterValues();
}

void RaySphereEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	if( strcmp(slotName, "sa_thetaMin") == 0 ){
		saUpdateBits |= saThetaMin;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "sa_thetaMax") == 0 ){
		saUpdateBits |= saThetaMax;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "sa_phiReferenceVel") == 0 ){
		saUpdateBits |= saPhiRefVel;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "sa_phiVariance") == 0 ){
		saUpdateBits |= saPhiVar;
		setMaskBits(emitterEdited);
	}

	Parent::onStaticModified(slotName, newValue);
}

void RaySphereEmitterNode::onDynamicModified(const char* slotName, const char*newValue)
{

}

void RaySphereEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	RaySphereEmitterNodeData* DataBlock = getDataBlock();
	if(!mActive || !mEmitter || !mEmitter->isProperlyAdded() || !DataBlock)
		return;

	mEmitter->emitParticles( (U32)(dt * DataBlock->timeMultiple * 1000.0f), this );
}

void RaySphereEmitterNode::UpdateEmitterValues()
{
	if(!mEmitter)
		return;
	RaySphereEmitter* emitter = (RaySphereEmitter*)mEmitter;
	if(saThetaMin & saUpdateBits)
		emitter->sa_thetaMin = sa_thetaMin;
	if(saThetaMax & saUpdateBits)
		emitter->sa_thetaMax = sa_thetaMax;
	if(saPhiRefVel & saUpdateBits)
		emitter->sa_phiReferenceVel = sa_phiReferenceVel;
	if(saPhiVar & saUpdateBits)
		emitter->sa_phiVariance = sa_phiVariance;

	Parent::UpdateEmitterValues();
}

void RaySphereEmitterNode::UpdateNodeValues(ParticleEmitterData* data)
{
	RaySphereEmitterData* sdata = (RaySphereEmitterData*)data;
   if(!sdata)
      return;
	if(!(saThetaMin & saUpdateBits))
	   sa_thetaMin = sdata->thetaMin;
	if(!(saThetaMax & saUpdateBits))
	   sa_thetaMax = sdata->thetaMax;
	if(!(saPhiRefVel & saUpdateBits))
	   sa_phiReferenceVel = sdata->phiReferenceVel;
	if(!(saPhiVar & saUpdateBits))
	   sa_phiVariance = sdata->phiVariance;

	Parent::UpdateEmitterValues();
}
DefineEngineMethod( RaySphereEmitterNode, mountToNode, bool,
				   ( SceneObject* objB, StringTableEntry node, TransformF txfm ), ( MatrixF::Identity ),
				   "@brief Mount objB to this object at the desired slot with optional transform.\n\n"

				   "@param objB  Object to mount onto us\n"
				   "@param slot  Mount slot ID\n"
				   "@param txfm (optional) mount offset transform\n"
				   "@return true if successful, false if failed (objB is not valid)" )
{
	ShapeBase* host = dynamic_cast<ShapeBase*>(objB);
	if ( host )
	{
		S32 slot = host->getShape()->findNode(node);
		host->mountObject( object, slot, txfm.getMatrix() );
		object->mNodeMounted = true;
		return true;
	}
	return false;
}

DefineEngineMethod(RaySphereEmitterNode, setActive, void, (bool active),,
				   "Turns the emitter on or off.\n"
				   "@param active New emitter state\n" )
{
	object->setActive( active );
}

DefineEngineMethod(RaySphereEmitterNode, setEmitterDataBlock, void, (ParticleEmitterData* emitterDatablock), (0),
   "Assigns the datablock for this emitter node.\n"
   "@param emitterDatablock ParticleEmitterData datablock to assign\n"
   "@tsexample\n"
   "// Assign a new emitter datablock\n"
   "%emitter.setEmitterDatablock( %emitterDatablock );\n"
   "@endtsexample\n" )
{
   if ( !emitterDatablock )
   {
      Con::errorf("ParticleEmitterData datablock could not be found when calling setEmitterDataBlock in particleEmitterNode.");
      return;
   }

   object->setEmitterDataBlock(emitterDatablock);
}