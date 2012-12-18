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

#include "GroundEmitterNode.h"
#include "IPSCore.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "sim/netConnection.h"
#include "console/engineAPI.h"

IMPLEMENT_CO_DATABLOCK_V1(GroundEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(GroundEmitterNode);

ConsoleDocClass( GroundEmitterNodeData,
	"@brief Contains additional data to be associated with a ParticleEmitterNode."
	"@ingroup FX\n"
	);

ConsoleDocClass( GroundEmitterNode,
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

GroundEmitterNodeData::GroundEmitterNodeData()
{
	sa_radius = 5;
}

void GroundEmitterNodeData::initPersistFields()
{
	addGroup( "Independent emitters");

	addField( "sa_radius", TYPEID< F32 >(), Offset(sa_radius, GroundEmitterNodeData),
		"The amount to scale the expression with." );

	endGroup( "Independent emitters");
}

bool GroundEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	//Validate variables here

	return true;
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void GroundEmitterNodeData::packData(BitStream* stream)
{
	Parent::packData(stream);
	stream->writeInt((S32)(sa_radius * 100), 16);
	stream->write(sa_radius);
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void GroundEmitterNodeData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
	sa_radius = stream->readInt(16) / 100.0f;
}

//-----------------------------------------------------------------------------
// SphereEmitterNode
//-----------------------------------------------------------------------------
GroundEmitterNode::GroundEmitterNode()
{
	sa_radius   = 5;   // ejection from the emitter point
	
	for(int i = 0; i < 5; i++)
	{
		layers[i] = StringTable->EmptyString();
	}
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GroundEmitterNode::initPersistFields()
{
	addField( "emitter",  TYPEID< GroundEmitterData >(), Offset(mEmitterDatablock, GroundEmitterNode),
		"Datablock to use when emitting particles." );

	// Add our variables to the worldeditor
	addGroup("GroundEmitter");

	addField( "layers", TypeTerrainMaterialName, Offset(layers, GroundEmitterNode), 5, 
		"Terrain material name to limit coverage to, or blank to not limit." );

	endGroup( "GroundEmitter");

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool GroundEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	setmDataBlock(dynamic_cast<GroundEmitterNodeData*>( dptr ));
	GroundEmitterNodeData* DataBlock = getDataBlock();
	if ( !DataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;


	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 GroundEmitterNode::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{	
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if( stream->writeFlag( mask & emitterEdited) )
	{
		stream->write(sa_radius);
	}

	if( stream->writeFlag( mask & layerEdited) )
	{
		for(int i = 0; i < 5; i++)
		{
			stream->writeString(layers[i]);
		}
	}

	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void GroundEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);

	//EmitterEdited
	if ( stream->readFlag() )
	{
		stream->read((F32 *)&sa_radius);
	}

	if ( stream->readFlag() )
	{
		for(int i = 0; i < 5; i++)
		{
			layers[i] = stream->readSTString();
		}
	}
}

void GroundEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	if( strcmp(slotName, "Radius") == 0 ){
		saUpdateBits |= saRadius;
		setMaskBits(emitterEdited);
	}

	if(strcmp(slotName, "layers") == 0)
		setMaskBits(layerEdited);

	Parent::onStaticModified(slotName, newValue);
}

void GroundEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	GroundEmitterNodeData* DataBlock = getDataBlock();
	if(!mActive || !mEmitter->isProperlyAdded() || !DataBlock)
		return;

	mEmitter->emitParticles( (U32)(dt * DataBlock->timeMultiple * 1000.0f), this );
}

void GroundEmitterNode::setEmitterDataBlock(ParticleEmitterData* data)
{
	Parent::setEmitterDataBlock(data);
}

void GroundEmitterNode::UpdateEmitterValues()
{
	if(!mEmitter)
		return;
	GroundEmitter* emitter = getEmitter();
	if(saRadius & saUpdateBits)
		emitter->sa_Radius = sa_radius;

	Parent::UpdateEmitterValues();
}