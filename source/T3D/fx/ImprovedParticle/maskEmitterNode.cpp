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

#include "MaskEmitterNode.h"
#include "IPSCore.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "sim/netConnection.h"
#include "console/engineAPI.h"

IMPLEMENT_CO_DATABLOCK_V1(MaskEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(MaskEmitterNode);

ConsoleDocClass( MaskEmitterNodeData,
	"@brief Contains additional data to be associated with a ParticleEmitterNode."
	"@ingroup FX\n"
	);

ConsoleDocClass( MaskEmitterNode,
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

MaskEmitterNodeData::MaskEmitterNodeData()
{
	grounded = false;
	sa_radius = 1;
}

void MaskEmitterNodeData::initPersistFields()
{
	Parent::initPersistFields();

	addGroup( "MaskEmitter");

	addField( "sa_radius", TYPEID< F32 >(), Offset(sa_radius, MaskEmitterNodeData),
		"The amount to scale the expression with." );
	
	addField( "Grounded", TYPEID< bool >(), Offset(grounded, MaskEmitterNodeData),
		"Reverse the graphEmitter." );

	endGroup( "MaskEmitter" );
}

bool MaskEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	//Validate variables here

	return true;
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void MaskEmitterNodeData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->writeInt(sa_radius * 1000, 15);
	stream->writeFlag(grounded);
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void MaskEmitterNodeData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
	sa_radius = stream->readInt(15) / 1000;
	grounded = stream->readFlag();
}

//-----------------------------------------------------------------------------
// SphereEmitterNode
//-----------------------------------------------------------------------------
MaskEmitterNode::MaskEmitterNode()
{
	grounded = false;
	sa_radius = 1;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void MaskEmitterNode::initPersistFields()
{
	addGroup( "MaskEmitter");

	addField( "sa_radius", TYPEID< F32 >(), Offset(sa_radius, MaskEmitterNode),
		"The amount to scale the expression with." );
	
	addField( "Grounded", TYPEID< bool >(), Offset(grounded, MaskEmitterNode),
		"Reverse the graphEmitter." );

	endGroup( "MaskEmitter" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool MaskEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	setmDataBlock(dynamic_cast<MaskEmitterNodeData*>( dptr ));
	MaskEmitterNodeData* DataBlock = getDataBlock();
	if ( !DataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;
	grounded = DataBlock->grounded;
	sa_radius = DataBlock->sa_radius;

	for(int i = 0; i < initialValues.size(); i=i+2)
	{
		if(strcmp("Grounded",initialValues[i].c_str()) == 0)
			grounded = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_radius",initialValues[i].c_str()) == 0)
			sa_radius = atof(const_cast<char*>(initialValues[i+1].c_str()));
	}

	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 MaskEmitterNode::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{	
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if( stream->writeFlag( mask & emitterEdited) )
	{
		stream->writeInt(sa_radius * 1000, 15);
		stream->writeFlag(grounded);
	}

	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void MaskEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);

	if(stream->readFlag())
	{
		sa_radius = stream->readInt(15) / 1000;
		grounded = stream->readFlag();
	}
}

void MaskEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	if(strcmp(slotName, "Grounded") == 0 ||
		strcmp(slotName, "sa_radius") == 0)
		setMaskBits(emitterEdited);
	Parent::onStaticModified(slotName, newValue);
}

void MaskEmitterNode::setEmitterDataBlock(ParticleEmitterData* data)
{
	Parent::setEmitterDataBlock(data);
}

void MaskEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	MaskEmitterNodeData* DataBlock = getDataBlock();
	if(!mActive || !mEmitter->isProperlyAdded() || !DataBlock)
		return;

	mEmitter->emitParticles( (U32)(dt * DataBlock->timeMultiple * 1000.0f), this );
}