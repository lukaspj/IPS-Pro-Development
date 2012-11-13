<<<<<<< HEAD
#include "particleEmitterNode.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "T3D/fx/particleEmitter.h"
#include "math/mathIO.h"
#include "sim/netConnection.h"
#include "console/engineAPI.h"

//IMPLEMENT_CO_DATABLOCK_V1(ParticleEmitterNodeData);
//IMPLEMENT_CONOBJECT(ParticleEmitterNode);

typedef ParticleEmitterNode::EnumAttractionMode pAttractionMode;
DefineEnumType( pAttractionMode );

ImplementEnumType( pAttractionMode,
	"The way the particles are interacting with specific objects.\n"
	"@ingroup FX\n\n")
{ ParticleEmitterNode::none,		"Not attracted",        "The particles are not attracted to any object.\n" },
{ ParticleEmitterNode::attract,		"Attract",				"The particles are attracted to the objects.\n" },
{ ParticleEmitterNode::repulse,		"Repulse",				"The particles are repulsed by the object.\n" },
EndImplementEnumType;

//-----------------------------------------------------------------------------
// ParticleEmitterNodeData
//-----------------------------------------------------------------------------
ParticleEmitterNodeData::ParticleEmitterNodeData()
{
	timeMultiple = 1.0;

	attractionrange = 50;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = 0;
		Amount[i] = 1;
		Attraction_offset[i] = "0 0 0";
	}
	sticky = false;
	ParticleCollision = false;

	
	standAloneEmitter = false;

	sa_ejectionPeriodMS = 100;    // 10 Particles Per second
	sa_periodVarianceMS = 0;      // exactly

	sa_ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	sa_velocityVariance = 1.0f;
	sa_ejectionOffset   = 0.0f;   // ejection from the emitter point
}

ParticleEmitterNodeData::~ParticleEmitterNodeData()
{
}

void ParticleEmitterNodeData::initPersistFields()
{
	addField( "timeMultiple", TYPEID< F32 >(), Offset(timeMultiple, ParticleEmitterNodeData),
		"@brief Time multiplier for particle emitter nodes.\n\n"
		"Increasing timeMultiple is like running the emitter at a faster rate - single-shot "
		"emitters will complete in a shorter time, and continuous emitters will generate "
		"particles more quickly.\n\n"
		"Valid range is 0.01 - 100." );

	addGroup( "Independent emitters" );

	addField( "standAloneEmitter", TYPEID< bool >(), Offset(standAloneEmitter, ParticleEmitterNodeData),
		"@brief If true, this datablock is not connected other datablocks of the same type .\n"
		"Useful for animated datablocks." );

	addField("sa_ejectionPeriodMS", TYPEID< S32 >(), Offset(sa_ejectionPeriodMS,   ParticleEmitterNodeData),
		"Time (in milliseconds) between each particle ejection." );

	addField("sa_periodVarianceMS", TYPEID< S32 >(), Offset(sa_periodVarianceMS,   ParticleEmitterNodeData),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );
	
	addField( "sa_ejectionVelocity", TYPEID< F32 >(), Offset(sa_ejectionVelocity, ParticleEmitterNodeData),
		"Particle ejection velocity." );

	addField( "sa_velocityVariance", TYPEID< F32 >(), Offset(sa_velocityVariance, ParticleEmitterNodeData),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "sa_ejectionOffset", TYPEID< F32 >(), Offset(sa_ejectionOffset, ParticleEmitterNodeData),
		"Distance along ejection Z axis from which to eject particles." );

	endGroup( "Independent emitters" );

	addGroup( "Physics" );

	addField( "RayCollision", TYPEID< bool >(), Offset(ParticleCollision, ParticleEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "AttractionMode", TYPEID< ParticleEmitterNode::EnumAttractionMode >(), Offset(AttractionMode, ParticleEmitterNodeData), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Attraction range", TYPEID< F32 >(), Offset(attractionrange, ParticleEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, ParticleEmitterNodeData), attrobjectCount,
		"Reverse the graphEmitter." );

	addField( "Sticky", TYPEID< bool >(), Offset(sticky, ParticleEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "Attraction_offset;", TYPEID< StringTableEntry >(), Offset(Attraction_offset, ParticleEmitterNodeData), attrobjectCount,
		"testReverse the graphEmitter." );

	endGroup( "Physics" );

	Parent::initPersistFields();
}

bool ParticleEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;
	return true;
}

bool ParticleEmitterNodeData::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;
	// Verify variables
	if( timeMultiple < 0.01 || timeMultiple > 100 )
	{
		Con::warnf("ParticleEmitterNodeData::onAdd(%s): timeMultiple must be between 0.01 and 100", getName());
		timeMultiple = timeMultiple < 0.01 ? 0.01 : 100;
	}

	return true;
}

void ParticleEmitterNodeData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->write(timeMultiple);

	stream->writeFlag(standAloneEmitter);

	stream->writeInt(sa_ejectionPeriodMS, 10);
	stream->writeInt(sa_periodVarianceMS, 10);

	stream->writeInt((S32)(sa_ejectionVelocity * 100), 16);
	stream->writeInt((S32)(sa_velocityVariance * 100), 14);
	stream->writeInt((S32)(sa_ejectionOffset * 100), 16);
}

void ParticleEmitterNodeData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	stream->read(&timeMultiple);

	standAloneEmitter = stream->readFlag();

	sa_ejectionPeriodMS = stream->readInt(10);
	sa_periodVarianceMS = stream->readInt(10);
	
	sa_ejectionVelocity = stream->readInt(16) / 100.0f;
	sa_velocityVariance = stream->readInt(14) / 100.0f;
	sa_ejectionOffset = stream->readInt(16) / 100.0f;
}

//-----------------------------------------------------------------------------
// ParticleEmitterNode
//-----------------------------------------------------------------------------
ParticleEmitterNode::ParticleEmitterNode()
{
	mNetFlags.set(ScopeAlways | Ghostable);
	mTypeMask |= EnvironmentObjectType;

	mActive = true;

	mDataBlock          = NULL;
	mEmitterDatablock   = NULL;
	mEmitterDatablockId = 0;
	mEmitter            = NULL;
	mVelocity           = 1.0;

	standAloneEmitter = false;

	sa_ejectionPeriodMS = 100;    // 10 Particles Per second
	sa_periodVarianceMS = 0;      // exactly

	sa_ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	sa_velocityVariance = 1.0f;
	sa_ejectionOffset   = 0.0f;   // ejection from the emitter point

	sticky = false;
	ParticleCollision = false;
	attractionrange = 50;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = 0;
		Amount[i] = 1;
		Attraction_offset[i] = "0 0 0";
		attractedObjectID[i] = "";
	}
}

ParticleEmitterNode::~ParticleEmitterNode()
{
}

void ParticleEmitterNode::initPersistFields()
{
	addField( "active", TYPEID< bool >(), Offset(mActive,ParticleEmitterNode),
		"Controls whether particles are emitted from this node." );
	addField( "emitter",  TYPEID< ParticleEmitterData >(), Offset(mEmitterDatablock, ParticleEmitterNode),
		"Datablock to use when emitting particles." );
	addField( "velocity", TYPEID< F32 >(), Offset(mVelocity, ParticleEmitterNode),
		"Velocity to use when emitting particles (in the direction of the "
		"ParticleEmitterNode object's up (Z) axis)." );

	addGroup( "Independent emitters" );

	addField( "standAloneEmitter", TYPEID< bool >(), Offset(standAloneEmitter, ParticleEmitterNode),
		"@brief If true, this datablock is not connected other datablocks of the same type .\n"
		"Useful for animated datablocks." );

	addField("sa_ejectionPeriodMS", TYPEID< S32 >(), Offset(sa_ejectionPeriodMS,   ParticleEmitterNode),
		"Time (in milliseconds) between each particle ejection." );

	addField("sa_periodVarianceMS", TYPEID< S32 >(), Offset(sa_periodVarianceMS,   ParticleEmitterNode),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "sa_ejectionVelocity", TYPEID< F32 >(), Offset(sa_ejectionVelocity, ParticleEmitterNode),
		"Particle ejection velocity." );

	addField( "sa_velocityVariance", TYPEID< F32 >(), Offset(sa_velocityVariance, ParticleEmitterNode),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "sa_ejectionOffset", TYPEID< F32 >(), Offset(sa_ejectionOffset, ParticleEmitterNode),
		"Distance along ejection Z axis from which to eject particles." );

	endGroup( "Independent emitters" );

	addGroup( "Physics" );

	addField( "RayCollision", TYPEID< bool >(), Offset(ParticleCollision, ParticleEmitterNode),
		"If true, the particles will stick to their original spawn place relative to the emitter." );

	addField( "attractedObjectID", TYPEID< StringTableEntry >(), Offset(attractedObjectID, ParticleEmitterNode), attrobjectCount,
		"The ID or name of the object that the particles should interact with." );

	addField( "Attraction_offset", TYPEID< StringTableEntry >(), Offset(Attraction_offset, ParticleEmitterNode), attrobjectCount,
		"Offset from the objects position the particles should be attracted to or repulsed from." );

	addField( "AttractionMode", TYPEID< pAttractionMode >(), Offset(AttractionMode, ParticleEmitterNode), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, ParticleEmitterNode), attrobjectCount,
		"Amount of influence, combine with attraction range for the desired result." );

	addField( "attractionrange", TYPEID< F32 >(), Offset(attractionrange, ParticleEmitterNode),
		"Range of influence, any objects further away than this length will not attract or repulse the particles." );

	addField( "sticky", TYPEID< bool >(), Offset(sticky, ParticleEmitterNode),
		"If true, the particles will stick to their original spawn place relative to the emitter." );

	endGroup( "Physics" );

	Parent::initPersistFields();
}

bool ParticleEmitterNode::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if( !mEmitterDatablock && mEmitterDatablockId != 0 )
	{
		if( Sim::findObject(mEmitterDatablockId, mEmitterDatablock) == false )
			Con::errorf(ConsoleLogEntry::General, "ParticleEmitterNode::onAdd: Invalid packet, bad datablockId(mEmitterDatablock): %d", mEmitterDatablockId);
	}

	if( isClientObject() )
	{
		setEmitterDataBlock( mEmitterDatablock );
	}
	else
	{
		setMaskBits( StateMask | EmitterDBMask );
	}

	mObjBox.minExtents.set(-0.5, -0.5, -0.5);
	mObjBox.maxExtents.set( 0.5,  0.5,  0.5);
	resetWorldBox();
	addToScene();

	return true;
}

void ParticleEmitterNode::onRemove()
{
	removeFromScene();
	if( isClientObject() )
	{
		if( mEmitter )
		{
			mEmitter->deleteWhenEmpty();
			mEmitter = NULL;
		}
	}

	Parent::onRemove();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool ParticleEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	mDataBlock = dynamic_cast<ParticleEmitterNodeData*>( dptr );
	if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = mDataBlock->AttractionMode[i];
		Amount[i] = mDataBlock->Amount[i];
		Attraction_offset[i] = mDataBlock->Attraction_offset[i];
	}
	attractionrange = mDataBlock->attractionrange;
	sticky = mDataBlock->sticky;
	ParticleCollision = mDataBlock->ParticleCollision;

	standAloneEmitter = mDataBlock->standAloneEmitter;

	sa_ejectionPeriodMS = mDataBlock->sa_ejectionPeriodMS;
	sa_periodVarianceMS = mDataBlock->sa_periodVarianceMS;

	sa_ejectionVelocity = mDataBlock->sa_ejectionVelocity;
	sa_velocityVariance = mDataBlock->sa_velocityVariance;
	sa_ejectionOffset = mDataBlock->sa_ejectionOffset;

	if(mEmitter)
	{
		for(int i = 0; i < attrobjectCount; i++)
		{
			mEmitter->AttractionMode[i] = AttractionMode[i];
			mEmitter->Amount[i] = Amount[i];
			mEmitter->Attraction_offset[i] = Attraction_offset[i];
		}
		mEmitter->sticky = sticky;
		mEmitter->ParticleCollision = ParticleCollision;
		mEmitter->attractionrange = attractionrange;
	}
	for(int i = 0; i < initialValues.size(); i=i+2)
	{
		if(strcmp("attractionrange",initialValues[i].c_str()) == 0)
			attractionrange = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sticky",initialValues[i].c_str()) == 0)
			sticky = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("RayCollision",initialValues[i].c_str()) == 0)
			ParticleCollision = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("standAloneEmitter",initialValues[i].c_str()) == 0)
			standAloneEmitter = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_ejectionPeriodMS",initialValues[i].c_str()) == 0)
			sa_ejectionPeriodMS = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_periodVarianceMS",initialValues[i].c_str()) == 0)
			sa_periodVarianceMS = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_ejectionVelocity",initialValues[i].c_str()) == 0)
			sa_ejectionVelocity = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_velocityVariance",initialValues[i].c_str()) == 0)
			sa_velocityVariance = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_ejectionOffset",initialValues[i].c_str()) == 0)
			sa_ejectionOffset = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Attraction_offset",initialValues[i].c_str()) == 0)
		{
			Attraction_offset[0] = const_cast<char*>(initialValues[i+1].c_str());
			Attraction_offset[1] = const_cast<char*>(initialValues[i+2].c_str());
			i++;
		}
		if(strcmp("attractedObjectID",initialValues[i].c_str()) == 0)
		{
			attractedObjectID[0] = const_cast<char*>(initialValues[i+1].c_str());
			attractedObjectID[1] = const_cast<char*>(initialValues[i+2].c_str());
			i++;
		}
		if(strcmp("AttractionMode",initialValues[i].c_str()) == 0)
		{
			AttractionMode[0] = atoi(const_cast<char*>(initialValues[i+1].c_str()));
			AttractionMode[1] = atoi(const_cast<char*>(initialValues[i+2].c_str()));
			i++;
		}
		if(strcmp("Amount",initialValues[i].c_str()) == 0)
		{
			Amount[0] = atoi(const_cast<char*>(initialValues[i+1].c_str()));
			Amount[1] = atoi(const_cast<char*>(initialValues[i+2].c_str()));
			i++;
		}
	}

	// Todo: Uncomment if this is a "leaf" class
	scriptOnNewDataBlock();
	return true;
}

void ParticleEmitterNode::inspectPostApply()
{
	Parent::inspectPostApply();
	setMaskBits(StateMask | EmitterDBMask);
}

void ParticleEmitterNode::processTick(const Move* move)
{
	Parent::processTick(move);

	if ( isMounted() )
	{
		MatrixF mat;
		mMount.object->getMountTransform( mMount.node, mMount.xfm, &mat );
		setTransform( mat );
	}
}

void ParticleEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	if(!mActive || !mEmitter->isProperlyAdded() || !mDataBlock)
		return;

	//mEmitter->emitParticles( (U32)(dt * mDataBlock->timeMultiple * 1000.0f), this );
}

U32 ParticleEmitterNode::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if ( stream->writeFlag( mask & InitialUpdateMask ) )
	{
		mathWrite(*stream, getTransform());
		mathWrite(*stream, getScale());
	}

	if ( stream->writeFlag( mask & EmitterDBMask ) )
	{
		if( stream->writeFlag(mEmitterDatablock != NULL) )
		{
			stream->writeRangedU32(mEmitterDatablock->getId(), DataBlockObjectIdFirst,
				DataBlockObjectIdLast);
		}
	}

	if ( stream->writeFlag( mask & StateMask ) )
	{
		stream->writeFlag( mActive );
		stream->write( mVelocity );
		stream->write( standAloneEmitter );
	}

	if( stream->writeFlag( mask & emitterEdited) )
	{
		stream->writeInt(sa_ejectionPeriodMS, 10);
		stream->writeInt(sa_periodVarianceMS, 10);
		
		stream->writeInt((S32)(sa_ejectionVelocity * 100), 16);
		stream->writeInt((S32)(sa_velocityVariance * 100), 14);
		stream->writeInt((S32)(sa_ejectionOffset * 100), 16);

		stream->writeFlag(sticky);
		stream->writeFlag(ParticleCollision);
		stream->writeInt(attractionrange*1000, 15);
		for(int i = 0; i < attrobjectCount; i++)
		{
			stream->writeInt(AttractionMode[i], 4);
			stream->writeInt(Amount[i]*1000, 15);
			stream->writeString(attractedObjectID[i]);
			stream->writeString(Attraction_offset[i]);
		}
	}

	return retMask;
}

void ParticleEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
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

	if ( stream->readFlag() )
	{
		mEmitterDatablockId = stream->readFlag() ?
			stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast) : 0;

		ParticleEmitterData *emitterDB = NULL;
		Sim::findObject( mEmitterDatablockId, emitterDB );
		if ( isProperlyAdded() )
			setEmitterDataBlock( emitterDB );
	}

	if ( stream->readFlag() )
	{
		mActive = stream->readFlag();
		stream->read( &mVelocity );

		stream->read( &standAloneEmitter );
	}
	if ( stream->readFlag() )
	{
		sa_ejectionPeriodMS = stream->readInt(10);
		sa_periodVarianceMS = stream->readInt(10);

		sa_ejectionVelocity = stream->readInt(16) / 100.0f;
		sa_velocityVariance = stream->readInt(14) / 100.0f;
		sa_ejectionOffset = stream->readInt(16) / 100.0f;

		sticky = stream->readFlag();
		ParticleCollision = stream->readFlag();
		attractionrange = stream->readInt(15)  / 1000.0f;
		for(int i = 0; i < attrobjectCount; i++)
		{
			AttractionMode[i] = stream->readInt(4);
			Amount[i] = stream->readInt(15) / 1000.0f;
			char buf[256];
			stream->readString(buf);
			attractedObjectID[i] = dStrdup(buf);
			char buf2[256];
			stream->readString(buf2);
			Attraction_offset[i] = dStrdup(buf2);
		}

		if(mEmitter)
		{
			mEmitter->sticky = sticky;
			mEmitter->ParticleCollision = ParticleCollision;
			mEmitter->attractionrange = attractionrange;
			for(int i = 0; i < attrobjectCount; i++)
			{
				mEmitter->AttractionMode[i] = AttractionMode[i];
				mEmitter->Amount[i] = Amount[i];
				mEmitter->attractedObjectID[i] = attractedObjectID[i];
				mEmitter->Attraction_offset[i] = Attraction_offset[i];
			}
		}
	}
}

void ParticleEmitterNode::setEmitterDataBlock(ParticleEmitterData* data)
{
	if ( isServerObject() )
	{
		setMaskBits( EmitterDBMask );
	}
	else
	{
		ParticleEmitter* pEmitter = NULL;
		if ( data )
		{
			// Create emitter with new datablock
			pEmitter = data->createEmitter();
			pEmitter->onNewDataBlock( data, false );
			if( pEmitter->registerObject() == false )
			{
				Con::warnf(ConsoleLogEntry::General, "Could not register base emitter for particle of class: %s", data->getName() ? data->getName() : data->getIdString() );
				delete pEmitter;
				return;
			}
		}

		// Replace emitter
		if ( mEmitter )
			mEmitter->deleteWhenEmpty();

		mEmitter = pEmitter;
		if(mEmitter)
		{
			mEmitter->sticky = sticky;
			mEmitter->ParticleCollision = ParticleCollision;
			mEmitter->attractionrange = attractionrange;
			for(int i = 0; i < attrobjectCount; i++)
			{
				mEmitter->attractedObjectID[i] = attractedObjectID[i];
				mEmitter->AttractionMode[i] = AttractionMode[i];
				mEmitter->Amount[i] = Amount[i];
				mEmitter->Attraction_offset[i] = Attraction_offset[i];
			}
		}
	}

	mEmitterDatablock = data;
}

void ParticleEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	if(strcmp(slotName, "sa_ejectionOffset") == 0 ||
		strcmp(slotName, "sa_ejectionPeriodMS") == 0 ||
		strcmp(slotName, "sa_periodVarianceMS") == 0 ||
		strcmp(slotName, "sa_ejectionOffset") == 0 ||
		strcmp(slotName, "sa_ejectionVelocity") == 0 ||
		strcmp(slotName, "sa_velocityVariance") == 0 ||
		strcmp(slotName, "standAloneEmitter") == 0 ||
		strcmp(slotName, "attracted") == 0 ||
		strcmp(slotName, "attractionrange") == 0 ||
		strcmp(slotName, "Attraction_offset") == 0 ||
		strcmp(slotName, "Amount") == 0 ||
		strcmp(slotName, "sticky") == 0 ||
		strcmp(slotName, "RayCollision") == 0 ||
		strcmp(slotName, "attractedObjectID") == 0 ||
		strcmp(slotName, "AttractionMode") == 0)
		setMaskBits(emitterEdited);

	if(!isProperlyAdded())
	{
		if(strcmp(slotName, "Attraction_offset") == 0)
		{
			initialValues.push_back(std::string("Attraction_offset"));
			initialValues.push_back(std::string(Attraction_offset[0]));
			initialValues.push_back(std::string(Attraction_offset[1]));
		}
		else if(strcmp(slotName, "attractedObjectID") == 0)
		{
			initialValues.push_back(std::string("attractedObjectID"));
			initialValues.push_back(std::string(attractedObjectID[0]));
			initialValues.push_back(std::string(attractedObjectID[1]));
		}
		else if(strcmp(slotName, "AttractionMode") == 0)
		{
			initialValues.push_back(std::string("AttractionMode"));
			std::stringstream ss;
			ss << AttractionMode[0];
			initialValues.push_back(std::string(ss.str()));
			ss.str("");
			ss << AttractionMode[1];
			initialValues.push_back(std::string(ss.str()));
		}
		else if(strcmp(slotName, "Amount") == 0)
		{
			initialValues.push_back(std::string("Amount"));
			std::stringstream ss;
			ss << Amount[0];
			initialValues.push_back(std::string(ss.str()));
			ss.str("");
			ss << Amount[1];
			initialValues.push_back(std::string(ss.str()));
		}
		else{
			//Save the values and add them onNewDataBlock
			initialValues.push_back(std::string(slotName));
			initialValues.push_back(std::string(newValue));
		}
	}
}

void ParticleEmitterNode::onDynamicModified(const char* slotName, const char*newValue)
{

}

DefineEngineMethod(ParticleEmitterNode, setActive, void, (bool active),,
	"Turns the emitter on or off.\n"
	"@param active New emitter state\n" )
{
	object->setActive( active );
}
=======
//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

#include "particleEmitterNode.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "T3D/fx/particleEmitter.h"
#include "math/mathIO.h"
#include "sim/netConnection.h"
#include "console/engineAPI.h"

IMPLEMENT_CO_DATABLOCK_V1(ParticleEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(ParticleEmitterNode);

ConsoleDocClass( ParticleEmitterNodeData,
	"@brief Contains additional data to be associated with a ParticleEmitterNode."
	"@ingroup FX\n"
	);

ConsoleDocClass( ParticleEmitterNode,
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

typedef ParticleEmitterNode::EnumAttractionMode pAttractionMode;
DefineEnumType( pAttractionMode );

ImplementEnumType( pAttractionMode,
	"The way the particles are interacting with specific objects.\n"
	"@ingroup FX\n\n")
{ ParticleEmitterNode::none,		"Not attracted",        "The particles are not attracted to any object.\n" },
{ ParticleEmitterNode::attract,		"Attract",				"The particles are attracted to the objects.\n" },
{ ParticleEmitterNode::repulse,		"Repulse",				"The particles are repulsed by the object.\n" },
EndImplementEnumType;

//-----------------------------------------------------------------------------
// ParticleEmitterNodeData
//-----------------------------------------------------------------------------
ParticleEmitterNodeData::ParticleEmitterNodeData()
{
	timeMultiple = 1.0;

	attractionrange = 50;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = 0;
		Amount[i] = 1;
		Attraction_offset[i] = "0 0 0";
	}
	sticky = false;
	ParticleCollision = false;

	
	standAloneEmitter = false;

	sa_ejectionPeriodMS = 100;    // 10 Particles Per second
	sa_periodVarianceMS = 0;      // exactly

	sa_ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	sa_velocityVariance = 1.0f;
	sa_ejectionOffset   = 0.0f;   // ejection from the emitter point

	sa_thetaMin         = 0.0f;   // All heights
	sa_thetaMax         = 90.0f;

	sa_phiReferenceVel  = 0.0f;   // All directions
	sa_phiVariance      = 180.0f;
}

ParticleEmitterNodeData::~ParticleEmitterNodeData()
{

}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void ParticleEmitterNodeData::initPersistFields()
{
	addField( "timeMultiple", TYPEID< F32 >(), Offset(timeMultiple, ParticleEmitterNodeData),
		"@brief Time multiplier for particle emitter nodes.\n\n"
		"Increasing timeMultiple is like running the emitter at a faster rate - single-shot "
		"emitters will complete in a shorter time, and continuous emitters will generate "
		"particles more quickly.\n\n"
		"Valid range is 0.01 - 100." );

	addGroup( "Independent emitters" );

	addField( "standAloneEmitter", TYPEID< bool >(), Offset(standAloneEmitter, ParticleEmitterNodeData),
		"@brief If true, this datablock is not connected other datablocks of the same type .\n"
		"Useful for animated datablocks." );

	addField("sa_ejectionPeriodMS", TYPEID< S32 >(), Offset(sa_ejectionPeriodMS,   ParticleEmitterNodeData),
		"Time (in milliseconds) between each particle ejection." );

	addField("sa_periodVarianceMS", TYPEID< S32 >(), Offset(sa_periodVarianceMS,   ParticleEmitterNodeData),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "sa_ejectionVelocity", TYPEID< F32 >(), Offset(sa_ejectionVelocity, ParticleEmitterNodeData),
		"Particle ejection velocity." );

	addField( "sa_velocityVariance", TYPEID< F32 >(), Offset(sa_velocityVariance, ParticleEmitterNodeData),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "sa_ejectionOffset", TYPEID< F32 >(), Offset(sa_ejectionOffset, ParticleEmitterNodeData),
		"Distance along ejection Z axis from which to eject particles." );

	addField( "sa_thetaMin", TYPEID< F32 >(), Offset(sa_thetaMin, ParticleEmitterNodeData),
		"Minimum angle, from the horizontal plane, to eject from." );

	addField( "sa_thetaMax", TYPEID< F32 >(), Offset(sa_thetaMax, ParticleEmitterNodeData),
		"Maximum angle, from the horizontal plane, to eject particles from." );

	addField( "sa_phiReferenceVel", TYPEID< F32 >(), Offset(sa_phiReferenceVel, ParticleEmitterNodeData),
		"Reference angle, from the vertical plane, to eject particles from." );

	addField( "sa_phiVariance", TYPEID< F32 >(), Offset(sa_phiVariance, ParticleEmitterNodeData),
		"Variance from the reference angle, from 0 - 360." );

	endGroup( "Independent emitters" );

	addGroup( "Physics" );

	addField( "RayCollision", TYPEID< bool >(), Offset(ParticleCollision, ParticleEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "AttractionMode", TYPEID< ParticleEmitterNode::EnumAttractionMode >(), Offset(AttractionMode, ParticleEmitterNodeData), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Attraction range", TYPEID< F32 >(), Offset(attractionrange, ParticleEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, ParticleEmitterNodeData), attrobjectCount,
		"Reverse the graphEmitter." );

	addField( "Sticky", TYPEID< bool >(), Offset(sticky, ParticleEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "Attraction_offset;", TYPEID< StringTableEntry >(), Offset(Attraction_offset, ParticleEmitterNodeData), attrobjectCount,
		"testReverse the graphEmitter." );

	endGroup( "Physics" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool ParticleEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if( timeMultiple < 0.01 || timeMultiple > 100 )
	{
		Con::warnf("ParticleEmitterNodeData::onAdd(%s): timeMultiple must be between 0.01 and 100", getName());
		timeMultiple = timeMultiple < 0.01 ? 0.01 : 100;
	}


	return true;
}


//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool ParticleEmitterNodeData::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;

	return true;
}


//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void ParticleEmitterNodeData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->write(timeMultiple);

	stream->writeFlag(standAloneEmitter);
	stream->writeInt(sa_ejectionPeriodMS, 10);
	stream->writeInt(sa_periodVarianceMS, 10);
	stream->writeInt((S32)(sa_ejectionVelocity * 100), 16);
	stream->writeInt((S32)(sa_velocityVariance * 100), 14);
	stream->writeInt((S32)(sa_ejectionOffset * 100), 16);
	stream->writeRangedU32((U32)sa_thetaMin, 0, 180);
	stream->writeRangedU32((U32)sa_thetaMax, 0, 180);
	stream->writeRangedU32((U32)sa_phiReferenceVel, 0, 360);
	stream->writeRangedU32((U32)sa_phiVariance, 0, 360);
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void ParticleEmitterNodeData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	stream->read(&timeMultiple);

	standAloneEmitter = stream->readFlag();

	sa_ejectionPeriodMS = stream->readInt(10);
	sa_periodVarianceMS = stream->readInt(10);
	sa_ejectionVelocity = stream->readInt(16) / 100.0f;
	sa_velocityVariance = stream->readInt(14) / 100.0f;
	sa_ejectionOffset = stream->readInt(16) / 100.0f;

	sa_thetaMin = (F32)stream->readRangedU32(0, 180);
	sa_thetaMax = (F32)stream->readRangedU32(0, 180);
	sa_phiReferenceVel = (F32)stream->readRangedU32(0, 360);
	sa_phiVariance = (F32)stream->readRangedU32(0, 360);
}


//-----------------------------------------------------------------------------
// ParticleEmitterNode
//-----------------------------------------------------------------------------
ParticleEmitterNode::ParticleEmitterNode()
{
	// Todo: ScopeAlways?
	mNetFlags.set(ScopeAlways | Ghostable);
	mTypeMask |= EnvironmentObjectType;

	mActive = true;

	mDataBlock          = NULL;
	mEmitterDatablock   = NULL;
	mEmitterDatablockId = 0;
	mEmitter            = NULL;
	mVelocity           = 1.0;

	standAloneEmitter = false;

	sa_ejectionPeriodMS = 100;    // 10 Particles Per second
	sa_periodVarianceMS = 0;      // exactly

	sa_ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	sa_velocityVariance = 1.0f;
	sa_ejectionOffset   = 0.0f;   // ejection from the emitter point

	sa_thetaMin         = 0.0f;   // All heights
	sa_thetaMax         = 90.0f;

	sa_phiReferenceVel  = 0.0f;   // All directions
	sa_phiVariance      = 180.0f;

	sticky = false;
	ParticleCollision = false;
	attractionrange = 50;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = 0;
		Amount[i] = 1;
		Attraction_offset[i] = "0 0 0";
		attractedObjectID[i] = "";
	}
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
ParticleEmitterNode::~ParticleEmitterNode()
{
	//
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void ParticleEmitterNode::initPersistFields()
{
	addField( "active", TYPEID< bool >(), Offset(mActive,ParticleEmitterNode),
		"Controls whether particles are emitted from this node." );
	addField( "emitter",  TYPEID< ParticleEmitterData >(), Offset(mEmitterDatablock, ParticleEmitterNode),
		"Datablock to use when emitting particles." );
	addField( "velocity", TYPEID< F32 >(), Offset(mVelocity, ParticleEmitterNode),
		"Velocity to use when emitting particles (in the direction of the "
		"ParticleEmitterNode object's up (Z) axis)." );

	addGroup( "Independent emitters" );

	addField( "standAloneEmitter", TYPEID< bool >(), Offset(standAloneEmitter, ParticleEmitterNode),
		"@brief If true, this datablock is not connected other datablocks of the same type .\n"
		"Useful for animated datablocks." );

	addField("sa_ejectionPeriodMS", TYPEID< S32 >(), Offset(sa_ejectionPeriodMS,   ParticleEmitterNode),
		"Time (in milliseconds) between each particle ejection." );

	addField("sa_periodVarianceMS", TYPEID< S32 >(), Offset(sa_periodVarianceMS,   ParticleEmitterNode),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "sa_ejectionVelocity", TYPEID< F32 >(), Offset(sa_ejectionVelocity, ParticleEmitterNode),
		"Particle ejection velocity." );

	addField( "sa_velocityVariance", TYPEID< F32 >(), Offset(sa_velocityVariance, ParticleEmitterNode),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "sa_ejectionOffset", TYPEID< F32 >(), Offset(sa_ejectionOffset, ParticleEmitterNode),
		"Distance along ejection Z axis from which to eject particles." );

	addField( "sa_thetaMin", TYPEID< F32 >(), Offset(sa_thetaMin, ParticleEmitterNode),
		"Minimum angle, from the horizontal plane, to eject from." );

	addField( "sa_thetaMax", TYPEID< F32 >(), Offset(sa_thetaMax, ParticleEmitterNode),
		"Maximum angle, from the horizontal plane, to eject particles from." );

	addField( "sa_phiReferenceVel", TYPEID< F32 >(), Offset(sa_phiReferenceVel, ParticleEmitterNode),
		"Reference angle, from the vertical plane, to eject particles from." );

	addField( "sa_phiVariance", TYPEID< F32 >(), Offset(sa_phiVariance, ParticleEmitterNode),
		"Variance from the reference angle, from 0 - 360." );

	endGroup( "Independent emitters" );

	addGroup( "Physics" );

	addField( "RayCollision", TYPEID< bool >(), Offset(ParticleCollision, ParticleEmitterNode),
		"If true, the particles will stick to their original spawn place relative to the emitter." );

	addField( "attractedObjectID", TYPEID< StringTableEntry >(), Offset(attractedObjectID, ParticleEmitterNode), attrobjectCount,
		"The ID or name of the object that the particles should interact with." );

	addField( "Attraction_offset", TYPEID< StringTableEntry >(), Offset(Attraction_offset, ParticleEmitterNode), attrobjectCount,
		"Offset from the objects position the particles should be attracted to or repulsed from." );

	addField( "AttractionMode", TYPEID< pAttractionMode >(), Offset(AttractionMode, ParticleEmitterNode), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, ParticleEmitterNode), attrobjectCount,
		"Amount of influence, combine with attraction range for the desired result." );

	addField( "attractionrange", TYPEID< F32 >(), Offset(attractionrange, ParticleEmitterNode),
		"Range of influence, any objects further away than this length will not attract or repulse the particles." );

	addField( "sticky", TYPEID< bool >(), Offset(sticky, ParticleEmitterNode),
		"If true, the particles will stick to their original spawn place relative to the emitter." );

	endGroup( "Physics" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool ParticleEmitterNode::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if( !mEmitterDatablock && mEmitterDatablockId != 0 )
	{
		if( Sim::findObject(mEmitterDatablockId, mEmitterDatablock) == false )
			Con::errorf(ConsoleLogEntry::General, "ParticleEmitterNode::onAdd: Invalid packet, bad datablockId(mEmitterDatablock): %d", mEmitterDatablockId);
	}

	if( isClientObject() )
	{
		setEmitterDataBlock( mEmitterDatablock );
	}
	else
	{
		setMaskBits( StateMask | EmitterDBMask );
	}

	mObjBox.minExtents.set(-0.5, -0.5, -0.5);
	mObjBox.maxExtents.set( 0.5,  0.5,  0.5);
	resetWorldBox();
	addToScene();

	return true;
}

//-----------------------------------------------------------------------------
// onRemove
//-----------------------------------------------------------------------------
void ParticleEmitterNode::onRemove()
{
	removeFromScene();
	if( isClientObject() )
	{
		if( mEmitter )
		{
			mEmitter->deleteWhenEmpty();
			mEmitter = NULL;
		}
	}

	Parent::onRemove();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool ParticleEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	mDataBlock = dynamic_cast<ParticleEmitterNodeData*>( dptr );
	if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = mDataBlock->AttractionMode[i];
		Amount[i] = mDataBlock->Amount[i];
		Attraction_offset[i] = mDataBlock->Attraction_offset[i];
	}
	attractionrange = mDataBlock->attractionrange;
	sticky = mDataBlock->sticky;
	ParticleCollision = mDataBlock->ParticleCollision;

	standAloneEmitter = mDataBlock->standAloneEmitter;
	sa_ejectionPeriodMS = mDataBlock->sa_ejectionPeriodMS;
	sa_periodVarianceMS = mDataBlock->sa_periodVarianceMS;

	sa_ejectionVelocity = mDataBlock->sa_ejectionVelocity;
	sa_velocityVariance = mDataBlock->sa_velocityVariance;
	sa_ejectionOffset = mDataBlock->sa_ejectionOffset;

	sa_thetaMin = mDataBlock->sa_thetaMin;
	sa_thetaMax = mDataBlock->sa_thetaMax;

	sa_phiReferenceVel = mDataBlock->sa_phiReferenceVel;
	sa_phiVariance = mDataBlock->sa_phiVariance;

	if(mEmitter)
	{
		for(int i = 0; i < attrobjectCount; i++)
		{
			mEmitter->AttractionMode[i] = AttractionMode[i];
			mEmitter->Amount[i] = Amount[i];
			mEmitter->Attraction_offset[i] = Attraction_offset[i];
		}
		mEmitter->sticky = sticky;
		mEmitter->ParticleCollision = ParticleCollision;
		mEmitter->attractionrange = attractionrange;
	}
	for(int i = 0; i < initialValues.size(); i=i+2)
	{
		if(strcmp("attractionrange",initialValues[i].c_str()) == 0)
			attractionrange = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sticky",initialValues[i].c_str()) == 0)
			sticky = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("RayCollision",initialValues[i].c_str()) == 0)
			ParticleCollision = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("standAloneEmitter",initialValues[i].c_str()) == 0)
			standAloneEmitter = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_ejectionPeriodMS",initialValues[i].c_str()) == 0)
			sa_ejectionPeriodMS = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_periodVarianceMS",initialValues[i].c_str()) == 0)
			sa_periodVarianceMS = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_ejectionVelocity",initialValues[i].c_str()) == 0)
			sa_ejectionVelocity = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_velocityVariance",initialValues[i].c_str()) == 0)
			sa_velocityVariance = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_ejectionOffset",initialValues[i].c_str()) == 0)
			sa_ejectionOffset = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_phiReferenceVel",initialValues[i].c_str()) == 0)
			sa_phiReferenceVel = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_phiVariance",initialValues[i].c_str()) == 0)
			sa_phiVariance = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_thetaMax",initialValues[i].c_str()) == 0)
			sa_thetaMax = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_thetaMin",initialValues[i].c_str()) == 0)
			sa_thetaMin = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Attraction_offset",initialValues[i].c_str()) == 0)
		{
			Attraction_offset[0] = const_cast<char*>(initialValues[i+1].c_str());
			Attraction_offset[1] = const_cast<char*>(initialValues[i+2].c_str());
			i++;
		}
		if(strcmp("attractedObjectID",initialValues[i].c_str()) == 0)
		{
			attractedObjectID[0] = const_cast<char*>(initialValues[i+1].c_str());
			attractedObjectID[1] = const_cast<char*>(initialValues[i+2].c_str());
			i++;
		}
		if(strcmp("AttractionMode",initialValues[i].c_str()) == 0)
		{
			AttractionMode[0] = atoi(const_cast<char*>(initialValues[i+1].c_str()));
			AttractionMode[1] = atoi(const_cast<char*>(initialValues[i+2].c_str()));
			i++;
		}
		if(strcmp("Amount",initialValues[i].c_str()) == 0)
		{
			Amount[0] = atoi(const_cast<char*>(initialValues[i+1].c_str()));
			Amount[1] = atoi(const_cast<char*>(initialValues[i+2].c_str()));
			i++;
		}
	}

	// Todo: Uncomment if this is a "leaf" class
	scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
void ParticleEmitterNode::inspectPostApply()
{
	Parent::inspectPostApply();
	setMaskBits(StateMask | EmitterDBMask);
}

//-----------------------------------------------------------------------------
// advanceTime
//-----------------------------------------------------------------------------
void ParticleEmitterNode::processTick(const Move* move)
{
	Parent::processTick(move);

	if ( isMounted() )
	{
		MatrixF mat;
		mMount.object->getMountTransform( mMount.node, mMount.xfm, &mat );
		setTransform( mat );
	}
}

void ParticleEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	if(!mActive || mEmitter.isNull() || !mDataBlock)
		return;

	Point3F emitPoint, emitVelocity;
	Point3F emitAxis(0, 0, 1);
	getTransform().mulV(emitAxis);
	getTransform().getColumn(3, &emitPoint);
	emitVelocity = emitAxis * mVelocity;
	mEmitter->emitParticles(emitPoint, emitPoint,
		emitAxis,
		emitVelocity, (U32)(dt * mDataBlock->timeMultiple * 1000.0f), this);
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 ParticleEmitterNode::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if ( stream->writeFlag( mask & InitialUpdateMask ) )
	{
		mathWrite(*stream, getTransform());
		mathWrite(*stream, getScale());
	}

	if ( stream->writeFlag( mask & EmitterDBMask ) )
	{
		if( stream->writeFlag(mEmitterDatablock != NULL) )
		{
			stream->writeRangedU32(mEmitterDatablock->getId(), DataBlockObjectIdFirst,
				DataBlockObjectIdLast);
		}
	}

	if ( stream->writeFlag( mask & StateMask ) )
	{
		stream->writeFlag( mActive );
		stream->write( mVelocity );
		stream->write( standAloneEmitter );
	}

	if( stream->writeFlag( mask & emitterEdited) )
	{
		stream->writeInt(sa_ejectionPeriodMS, 10);
		stream->writeInt(sa_periodVarianceMS, 10);
		stream->writeInt((S32)(sa_ejectionVelocity * 100), 16);
		stream->writeInt((S32)(sa_velocityVariance * 100), 14);
		stream->writeInt((S32)(sa_ejectionOffset * 100), 16);
		stream->writeRangedU32((U32)sa_thetaMin, 0, 180);
		stream->writeRangedU32((U32)sa_thetaMax, 0, 180);
		stream->writeRangedU32((U32)sa_phiReferenceVel, 0, 360);
		stream->writeRangedU32((U32)sa_phiVariance, 0, 360);

		stream->writeFlag(sticky);
		stream->writeFlag(ParticleCollision);
		stream->writeInt(attractionrange*1000, 15);
		for(int i = 0; i < attrobjectCount; i++)
		{
			stream->writeInt(AttractionMode[i], 4);
			stream->writeInt(Amount[i]*1000, 15);
			stream->writeString(attractedObjectID[i]);
			stream->writeString(Attraction_offset[i]);
		}
	}

	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void ParticleEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
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

	if ( stream->readFlag() )
	{
		mEmitterDatablockId = stream->readFlag() ?
			stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast) : 0;

		ParticleEmitterData *emitterDB = NULL;
		Sim::findObject( mEmitterDatablockId, emitterDB );
		if ( isProperlyAdded() )
			setEmitterDataBlock( emitterDB );
	}

	if ( stream->readFlag() )
	{
		mActive = stream->readFlag();
		stream->read( &mVelocity );

		stream->read( &standAloneEmitter );
	}
	if ( stream->readFlag() )
	{
		sa_ejectionPeriodMS = stream->readInt(10);
		sa_periodVarianceMS = stream->readInt(10);
		sa_ejectionVelocity = stream->readInt(16) / 100.0f;
		sa_velocityVariance = stream->readInt(14) / 100.0f;
		sa_ejectionOffset = stream->readInt(16) / 100.0f;

		sa_thetaMin = (F32)stream->readRangedU32(0, 180);
		sa_thetaMax = (F32)stream->readRangedU32(0, 180);
		sa_phiReferenceVel = (F32)stream->readRangedU32(0, 360);
		sa_phiVariance = (F32)stream->readRangedU32(0, 360);

		sticky = stream->readFlag();
		ParticleCollision = stream->readFlag();
		attractionrange = stream->readInt(15)  / 1000.0f;
		for(int i = 0; i < attrobjectCount; i++)
		{
			AttractionMode[i] = stream->readInt(4);
			Amount[i] = stream->readInt(15) / 1000.0f;
			char buf[256];
			stream->readString(buf);
			attractedObjectID[i] = dStrdup(buf);
			char buf2[256];
			stream->readString(buf2);
			Attraction_offset[i] = dStrdup(buf2);
		}

		if(mEmitter)
		{
			mEmitter->sticky = sticky;
			mEmitter->ParticleCollision = ParticleCollision;
			mEmitter->attractionrange = attractionrange;
			for(int i = 0; i < attrobjectCount; i++)
			{
				mEmitter->AttractionMode[i] = AttractionMode[i];
				mEmitter->Amount[i] = Amount[i];
				mEmitter->attractedObjectID[i] = attractedObjectID[i];
				mEmitter->Attraction_offset[i] = Attraction_offset[i];
			}
		}
	}
}

void ParticleEmitterNode::setEmitterDataBlock(ParticleEmitterData* data)
{
	if ( isServerObject() )
	{
		setMaskBits( EmitterDBMask );
	}
	else
	{
		ParticleEmitter* pEmitter = NULL;
		if ( data )
		{
			// Create emitter with new datablock
			pEmitter = new ParticleEmitter;
			pEmitter->onNewDataBlock( data, false );
			if( pEmitter->registerObject() == false )
			{
				Con::warnf(ConsoleLogEntry::General, "Could not register base emitter for particle of class: %s", data->getName() ? data->getName() : data->getIdString() );
				delete pEmitter;
				return;
			}
		}

		// Replace emitter
		if ( mEmitter )
			mEmitter->deleteWhenEmpty();

		mEmitter = pEmitter;
		if(mEmitter)
		{
			mEmitter->sticky = sticky;
			mEmitter->ParticleCollision = ParticleCollision;
			mEmitter->attractionrange = attractionrange;
			for(int i = 0; i < attrobjectCount; i++)
			{
				mEmitter->attractedObjectID[i] = attractedObjectID[i];
				mEmitter->AttractionMode[i] = AttractionMode[i];
				mEmitter->Amount[i] = Amount[i];
				mEmitter->Attraction_offset[i] = Attraction_offset[i];
			}
		}
	}

	mEmitterDatablock = data;
}

void ParticleEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	if(strcmp(slotName, "sa_ejectionOffset") == 0 ||
		strcmp(slotName, "sa_ejectionPeriodMS") == 0 ||
		strcmp(slotName, "sa_periodVarianceMS") == 0 ||
		strcmp(slotName, "standAloneEmitter") == 0 ||
		strcmp(slotName, "sa_ejectionVelocity") == 0 ||
		strcmp(slotName, "sa_velocityVariance") == 0 ||
		strcmp(slotName, "sa_thetaMin") == 0 ||
		strcmp(slotName, "sa_thetaMax") == 0 ||
		strcmp(slotName, "sa_phiReferenceVel") == 0 ||
		strcmp(slotName, "sa_phiVariance") == 0||
		strcmp(slotName, "attracted") == 0 ||
		strcmp(slotName, "attractionrange") == 0 ||
		strcmp(slotName, "Attraction_offset") == 0 ||
		strcmp(slotName, "Amount") == 0 ||
		strcmp(slotName, "sticky") == 0 ||
		strcmp(slotName, "RayCollision") == 0 ||
		strcmp(slotName, "attractedObjectID") == 0 ||
		strcmp(slotName, "AttractionMode") == 0)
		setMaskBits(emitterEdited);

	if(!isProperlyAdded())
	{
		if(strcmp(slotName, "Attraction_offset") == 0)
		{
			initialValues.push_back(std::string("Attraction_offset"));
			initialValues.push_back(std::string(Attraction_offset[0]));
			initialValues.push_back(std::string(Attraction_offset[1]));
		}
		else if(strcmp(slotName, "attractedObjectID") == 0)
		{
			initialValues.push_back(std::string("attractedObjectID"));
			initialValues.push_back(std::string(attractedObjectID[0]));
			initialValues.push_back(std::string(attractedObjectID[1]));
		}
		else if(strcmp(slotName, "AttractionMode") == 0)
		{
			initialValues.push_back(std::string("AttractionMode"));
			std::stringstream ss;
			ss << AttractionMode[0];
			initialValues.push_back(std::string(ss.str()));
			ss.str("");
			ss << AttractionMode[1];
			initialValues.push_back(std::string(ss.str()));
		}
		else if(strcmp(slotName, "Amount") == 0)
		{
			initialValues.push_back(std::string("Amount"));
			std::stringstream ss;
			ss << Amount[0];
			initialValues.push_back(std::string(ss.str()));
			ss.str("");
			ss << Amount[1];
			initialValues.push_back(std::string(ss.str()));
		}
		else{
			//Save the values and add them onNewDataBlock
			initialValues.push_back(std::string(slotName));
			initialValues.push_back(std::string(newValue));
		}
	}

	if(strcmp(slotName, "attractedObjectID") == 0)
	{
		for(int i = 0; i < attrobjectCount; i++)
		{
			attractedObjectID[i] = StringTableEntry(attractedObjectID[i]);
		}
	}
}

void ParticleEmitterNode::onDynamicModified(const char* slotName, const char*newValue)
{

}

DefineEngineMethod(ParticleEmitterNode, setEmitterDataBlock, void, (ParticleEmitterData* emitterDatablock), (0),
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

DefineEngineMethod(ParticleEmitterNode, setActive, void, (bool active),,
	"Turns the emitter on or off.\n"
	"@param active New emitter state\n" )
{
	object->setActive( active );
}
>>>>>>> 2190e5a3bcdb44a5f496b14f6226b46c12fa418b
