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

#include "sphereEmitterNode.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(SphereEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(SphereEmitterNode);

ConsoleDocClass( SphereEmitterNodeData,
	"@brief Contains additional data to be associated with a ParticleEmitterNode."
	"@ingroup FX\n"
	);

ConsoleDocClass( SphereEmitterNode,
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

SphereEmitterNodeData::SphereEmitterNodeData()
{
	sa_thetaMin         = 0.0f;   // All heights
	sa_thetaMax         = 90.0f;

	sa_phiReferenceVel  = 0.0f;   // All directions
	sa_phiVariance      = 180.0f;
}

void SphereEmitterNodeData::initPersistFields()
{
	addGroup( "Independent emitters" );

	addField( "sa_thetaMin", TYPEID< F32 >(), Offset(sa_thetaMin, SphereEmitterNodeData),
		"Minimum angle, from the horizontal plane, to eject from." );

	addField( "sa_thetaMax", TYPEID< F32 >(), Offset(sa_thetaMax, SphereEmitterNodeData),
		"Maximum angle, from the horizontal plane, to eject particles from." );

	addField( "sa_phiReferenceVel", TYPEID< F32 >(), Offset(sa_phiReferenceVel, SphereEmitterNodeData),
		"Reference angle, from the vertical plane, to eject particles from." );

	addField( "sa_phiVariance", TYPEID< F32 >(), Offset(sa_phiVariance, SphereEmitterNodeData),
		"Variance from the reference angle, from 0 - 360." );

	endGroup( "Independent emitters" );
	
	Parent::initPersistFields();
}

bool SphereEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	//Validate variables here

	return true;
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void SphereEmitterNodeData::packData(BitStream* stream)
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
void SphereEmitterNodeData::unpackData(BitStream* stream)
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
SphereEmitterNode::SphereEmitterNode()
{
	sa_thetaMin         = 0.0f;   // All heights
	sa_thetaMax         = 90.0f;

	sa_phiReferenceVel  = 0.0f;   // All directions
	sa_phiVariance      = 180.0f;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void SphereEmitterNode::initPersistFields()
{
	addGroup( "Independent emitters" );

	addField( "sa_thetaMin", TYPEID< F32 >(), Offset(sa_thetaMin, SphereEmitterNode),
		"Minimum angle, from the horizontal plane, to eject from." );

	addField( "sa_thetaMax", TYPEID< F32 >(), Offset(sa_thetaMax, SphereEmitterNode),
		"Maximum angle, from the horizontal plane, to eject particles from." );

	addField( "sa_phiReferenceVel", TYPEID< F32 >(), Offset(sa_phiReferenceVel, SphereEmitterNode),
		"Reference angle, from the vertical plane, to eject particles from." );

	addField( "sa_phiVariance", TYPEID< F32 >(), Offset(sa_phiVariance, SphereEmitterNode),
		"Variance from the reference angle, from 0 - 360." );

	endGroup( "Independent emitters" );
	
	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool SphereEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	setmDataBlock(dynamic_cast<SphereEmitterNodeData*>( dptr ));
	SphereEmitterNodeData* DataBlock = getDataBlock();
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
U32 SphereEmitterNode::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
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
void SphereEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);
	if ( stream->readFlag() )
	{
		sa_thetaMin = (F32)stream->readRangedU32(0, 180);
		sa_thetaMax = (F32)stream->readRangedU32(0, 180);
		sa_phiReferenceVel = (F32)stream->readRangedU32(0, 360);
		sa_phiVariance = (F32)stream->readRangedU32(0, 360);
	}
}

void SphereEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	if(strcmp(slotName, "sa_thetaMin") == 0 ||
		strcmp(slotName, "sa_thetaMax") == 0 ||
		strcmp(slotName, "sa_phiReferenceVel") == 0 ||
		strcmp(slotName, "sa_phiVariance") == 0)
		setMaskBits(emitterEdited);

	Parent::onStaticModified(slotName, newValue);
}

void SphereEmitterNode::onDynamicModified(const char* slotName, const char*newValue)
{

}

void SphereEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	SphereEmitterNodeData* DataBlock = getDataBlock();
	if(!mActive || !mEmitter->isProperlyAdded() || !DataBlock)
		return;

	mEmitter->emitParticles( (U32)(dt * DataBlock->timeMultiple * 1000.0f), this );
}