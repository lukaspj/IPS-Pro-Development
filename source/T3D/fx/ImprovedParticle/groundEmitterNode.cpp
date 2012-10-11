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
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "GroundEmitter.h"
#include "math/mathIO.h"
#include "sim/netConnection.h"
#include "console/engineAPI.h"

IMPLEMENT_CO_DATABLOCK_V1(GroundEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(GroundEmitterNode);

ConsoleDocClass( GroundEmitterNodeData,
	"@brief Contains additional data to be associated with a GroundEmitterNode."
	"@ingroup FX\n"
	);

ConsoleDocClass( GroundEmitterNode,
	"@brief A particle emitter object that can be positioned in the world and "
	"dynamically enabled or disabled.\n\n"

	"@tsexample\n"
	"datablock GroundEmitterNodeData( SimpleEmitterNodeData )\n"
	"{\n"
	"   timeMultiple = 1.0;\n"
	"};\n\n"

	"%emitter = new GroundEmitterNode()\n"
	"{\n"
	"   datablock = SimpleEmitterNodeData;\n"
	"   active = true;\n"
	"   emitter = FireEmitterData;\n"
	"   velocity = 3.5;\n"
	"};\n\n"

	"// Dynamically change emitter datablock\n"
	"%emitter.setEmitterDataBlock( DustEmitterData );\n"
	"@endtsexample\n"

	"@note To change the emitter field dynamically (after the GroundEmitterNode "
	"object has been created) you must use the setEmitterDataBlock() method or the "
	"change will not be replicated to other clients in the game.\n"
	"Similarly, use the setActive() method instead of changing the active field "
	"directly. When changing velocity, you need to toggle setActive() on and off "
	"to force the state change to be transmitted to other clients.\n\n"

	"@ingroup FX\n"
	"@see GroundEmitterNodeData\n"
	"@see GroundEmitterData\n"
	);

//-----------------------------------------------------------------------------
// GroundEmitterNodeData
//-----------------------------------------------------------------------------
GroundEmitterNodeData::GroundEmitterNodeData()
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

	standAloneEmitter = false;

	sa_ejectionPeriodMS = 100;    // 10 Grounds Per second
	sa_periodVarianceMS = 0;      // exactly

	sa_ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	sa_velocityVariance = 1.0f;
	sa_ejectionOffset   = 0.0f;   // ejection from the emitter point
	sa_radius = 5;

	sa_thetaMin         = 0.0f;   // All heights
	sa_thetaMax         = 90.0f;
}

GroundEmitterNodeData::~GroundEmitterNodeData()
{

}

typedef GroundEmitterNode::EnumAttractionMode grAttractionMode;
DefineEnumType( grAttractionMode );

ImplementEnumType( grAttractionMode,
	"The way the particles are interacting with specific objects.\n"
	"@ingroup FX\n\n")
{ GroundEmitterNode::none,			"Not attracted",        "The particles are not attracted to any object.\n" },
{ GroundEmitterNode::attract,		"Attract",				"The particles are attracted to the objects.\n" },
{ GroundEmitterNode::repulse,		"Repulse",				"The particles are repulsed by the object.\n" },
EndImplementEnumType;

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GroundEmitterNodeData::initPersistFields()
{
	addField( "timeMultiple", TYPEID< F32 >(), Offset(timeMultiple, GroundEmitterNodeData),
		"@brief Time multiplier for particle emitter nodes.\n\n"
		"Increasing timeMultiple is like running the emitter at a faster rate - single-shot "
		"emitters will complete in a shorter time, and continuous emitters will generate "
		"particles more quickly.\n\n"
		"Valid range is 0.01 - 100." );

	addGroup( "Independent emitters");

	addField( "standAloneEmitter", TYPEID< bool >(), Offset(standAloneEmitter, GroundEmitterNodeData),
		"@brief If true, this datablock is not connected other datablocks of the same type .\n"
		"Useful for animated datablocks." );

	addField("sa_ejectionPeriodMS", TYPEID< S32 >(), Offset(sa_ejectionPeriodMS,   GroundEmitterNodeData),
		"Time (in milliseconds) between each particle ejection." );

	addField("sa_periodVarianceMS", TYPEID< S32 >(), Offset(sa_periodVarianceMS,   GroundEmitterNodeData),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "sa_ejectionVelocity", TYPEID< F32 >(), Offset(sa_ejectionVelocity, GroundEmitterNodeData),
		"Particle ejection velocity." );

	addField( "sa_velocityVariance", TYPEID< F32 >(), Offset(sa_velocityVariance, GroundEmitterNodeData),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "sa_ejectionOffset", TYPEID< F32 >(), Offset(sa_ejectionOffset, GroundEmitterNodeData),
		"The amount to scale the expression with." );

	addField( "sa_radius", TYPEID< F32 >(), Offset(sa_radius, GroundEmitterNodeData),
		"The amount to scale the expression with." );

	addField( "sa_thetaMin", TYPEID< F32 >(), Offset(sa_thetaMin, GroundEmitterNodeData),
		"Minimum angle, from the horizontal plane, to eject from." );

	addField( "sa_thetaMax", TYPEID< F32 >(), Offset(sa_thetaMax, GroundEmitterNodeData),
		"Maximum angle, from the horizontal plane, to eject particles from." );

	endGroup( "Independent emitters" );

	addGroup( "Physics" );

	addField( "AttractionMode", TYPEID< GroundEmitterNode::EnumAttractionMode >(), Offset(AttractionMode, GroundEmitterNodeData), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Attraction range", TYPEID< F32 >(), Offset(attractionrange, GroundEmitterNodeData),
		"Reverse the GroundEmitter." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, GroundEmitterNodeData), attrobjectCount,
		"Reverse the GroundEmitter." );

	addField( "Sticky", TYPEID< bool >(), Offset(sticky, GroundEmitterNodeData),
		"Reverse the GroundEmitter." );

	addField( "Attraction_offset;", TYPEID< StringTableEntry >(), Offset(Attraction_offset, GroundEmitterNodeData), attrobjectCount,
		"testReverse the GroundEmitter." );

	endGroup( "Physics" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool GroundEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if( timeMultiple < 0.01 || timeMultiple > 100 )
	{
		Con::warnf("GroundEmitterNodeData::onAdd(%s): timeMultiple must be between 0.01 and 100", getName());
		timeMultiple = timeMultiple < 0.01 ? 0.01 : 100;
	}


	return true;
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool GroundEmitterNodeData::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void GroundEmitterNodeData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->write(timeMultiple);
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void GroundEmitterNodeData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	stream->read(&timeMultiple);
}

//-----------------------------------------------------------------------------
// GroundEmitterNode
//-----------------------------------------------------------------------------
GroundEmitterNode::GroundEmitterNode()
{
	mNetFlags.set(Ghostable);
	mTypeMask |= EnvironmentObjectType;

	mActive = true;
	shuttingDown = false;

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
	sa_ejectionOffset  = 0.0f;   // ejection from the emitter point
	sa_radius   = 5;   // ejection from the emitter point

	sa_thetaMin         = 0.0f;   // All heights
	sa_thetaMax         = 90.0f;

	sticky = false;
	attractionrange = 50;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = 0;
		Amount[i] = 1;
		Attraction_offset[i] = "0 0 0";
		attractedObjectID[i] = "";
	}
	for(int i = 0; i < 5; i++)
	{
		layers[i] = StringTable->EmptyString();
	}

	cb_Max = false;
	initialValues = *new std::vector<std::string>();
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GroundEmitterNode::~GroundEmitterNode()
{
	//
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GroundEmitterNode::initPersistFields()
{
	addField( "active", TYPEID< bool >(), Offset(mActive,GroundEmitterNode),
		"Controls whether particles are emitted from this node." );
	addField( "emitter",  TYPEID< GroundEmitterData >(), Offset(mEmitterDatablock, GroundEmitterNode),
		"Datablock to use when emitting particles." );
	addField( "velocity", TYPEID< F32 >(), Offset(mVelocity, GroundEmitterNode),
		"Velocity to use when emitting particles (in the direction of the "
		"GroundEmitterNode object's up (Z) axis)." );

	// Add our variables to the worldeditor
	addGroup("GroundEmitter");

	addField( "layers", TypeTerrainMaterialName, Offset(layers, GroundEmitterNode), 5, 
		"Terrain material name to limit coverage to, or blank to not limit." );

	endGroup( "GroundEmitter");

	addField( "standAloneEmitter", TYPEID< bool >(), Offset(standAloneEmitter, GroundEmitterNode),
		"@brief If true, this datablock is not connected other datablocks of the same type .\n"
		"Useful for animated datablocks." );

	addField("sa_ejectionPeriodMS", TYPEID< S32 >(), Offset(sa_ejectionPeriodMS,   GroundEmitterNode),
		"Time (in milliseconds) between each particle ejection." );

	addField("sa_periodVarianceMS", TYPEID< S32 >(), Offset(sa_periodVarianceMS,   GroundEmitterNode),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "sa_ejectionVelocity", TYPEID< F32 >(), Offset(sa_ejectionVelocity, GroundEmitterNode),
		"Particle ejection velocity." );

	addField( "sa_velocityVariance", TYPEID< F32 >(), Offset(sa_velocityVariance, GroundEmitterNode),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "sa_ejectionOffset", TYPEID< F32 >(), Offset(sa_ejectionOffset, GroundEmitterNode),
		"The amount to scale the expression with." );

	addField( "sa_radius", TYPEID< F32 >(), Offset(sa_radius, GroundEmitterNode),
		"The amount to scale the expression with." );

	addField( "sa_thetaMin", TYPEID< F32 >(), Offset(sa_thetaMin, GroundEmitterNode),
		"Minimum angle, from the horizontal plane, to eject from." );

	addField( "sa_thetaMax", TYPEID< F32 >(), Offset(sa_thetaMax, GroundEmitterNode),
		"Maximum angle, from the horizontal plane, to eject particles from." );

	endGroup( "Independent emitters" );

	addGroup( "Physics" );

	addField( "attractedObjectID", TYPEID< StringTableEntry >(), Offset(attractedObjectID, GroundEmitterNode), attrobjectCount,
		"The ID or name of the object that the particles should interact with." );

	addField( "Attraction_offset", TYPEID< StringTableEntry >(), Offset(Attraction_offset, GroundEmitterNode), attrobjectCount,
		"Offset from the objects position the particles should be attracted to or repulsed from." );

	addField( "AttractionMode", TYPEID< grAttractionMode >(), Offset(AttractionMode, GroundEmitterNode), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, GroundEmitterNode), attrobjectCount,
		"Amount of influence, combine with attraction range for the desired result." );

	addField( "attractionrange", TYPEID< F32 >(), Offset(attractionrange, GroundEmitterNode),
		"Range of influence, any objects further away than this length will not attract or repulse the particles." );

	addField( "sticky", TYPEID< bool >(), Offset(sticky, GroundEmitterNode),
		"If true, the particles will stick to their original spawn place relative to the emitter." );

	endGroup( "Physics" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool GroundEmitterNode::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if( !mEmitterDatablock && mEmitterDatablockId != 0 )
	{
		if( Sim::findObject(mEmitterDatablockId, mEmitterDatablock) == false )
			Con::errorf(ConsoleLogEntry::General, "GroundEmitterNode::onAdd: Invalid packet, bad datablockId(mEmitterDatablock): %d", mEmitterDatablockId);
	}

	if( isClientObject() )
	{
		setEmitterDataBlock( mEmitterDatablock );
	}
	else
	{
		/*
		sa_thetaMax = mEmitterDatablock->thetaMax;
		sa_thetaMin = mEmitterDatablock->thetaMin;

		sa_phiReferenceVel = mEmitterDatablock->phiReferenceVel;
		sa_phiVariance = mEmitterDatablock->phiVariance;

		sa_ejectionVelocity = mEmitterDatablock->ejectionVelocity;
		sa_velocityVariance = mEmitterDatablock->velocityVariance;
		sa_ejectionOffset = mEmitterDatablock->ejectionOffset;

		sa_ejectionPeriodMS = mEmitterDatablock->ejectionPeriodMS;
		sa_periodVarianceMS = mEmitterDatablock->periodVarianceMS;
		| emitterEdited */
		setMaskBits( StateMask | EmitterDBMask );
	}

	mObjBox.minExtents.set(-0.5, -0.5, -0.5);
	mObjBox.maxExtents.set( 0.5,  0.5,  0.5);
	resetWorldBox();
	addToScene();

	return true;
}

//-----------------------------------------------------------------------------
// safeDelete
//-----------------------------------------------------------------------------
//Not used
void GroundEmitterNode::safeDelete()
{
	if(!shuttingDown)
	{
		shuttingDown = true;
		if(isServerObject()){
			Con::printf("ServerSafeDelete");
			//setMaskBits( StateMask );
		}
		else
			Con::printf("ClientSafeDelete");
		//this->safeDeleteObject();
		this->deleteObject();
	}
}

//-----------------------------------------------------------------------------
// onRemove
//-----------------------------------------------------------------------------
void GroundEmitterNode::onRemove()
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
bool GroundEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	mDataBlock = dynamic_cast<GroundEmitterNodeData*>( dptr );
	if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	/* sa_thetaMax = mDataBlock->sa_thetaMax;
	sa_thetaMin = mDataBlock->sa_thetaMin;

	sa_phiReferenceVel = mDataBlock->sa_phiReferenceVel;
	sa_phiVariance = mDataBlock->sa_phiVariance;

	sa_ejectionVelocity = mDataBlock->sa_ejectionVelocity;
	sa_velocityVariance = mDataBlock->sa_velocityVariance;
	sa_ejectionOffset = mDataBlock->sa_ejectionOffset;

	sa_ejectionPeriodMS = mDataBlock->sa_ejectionPeriodMS;
	sa_periodVarianceMS = mDataBlock->sa_periodVarianceMS;*/

	// Synchronize the expression data.
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = mDataBlock->AttractionMode[i];
		Amount[i] = mDataBlock->Amount[i];
		Attraction_offset[i] = mDataBlock->Attraction_offset[i];
	}
	attractionrange = mDataBlock->attractionrange;
	sticky = mDataBlock->sticky;

	if(mEmitter)
	{
		for(int i = 0; i < attrobjectCount; i++)
		{
			mEmitter->AttractionMode[i] = AttractionMode[i];
			mEmitter->Amount[i] = Amount[i];
			mEmitter->Attraction_offset[i] = Attraction_offset[i];
		}
		mEmitter->sticky = sticky;
		mEmitter->attractionrange = attractionrange;
	}

	for(int i = 0; i < initialValues.size(); i=i+2)
	{
		if(strcmp("attractionrange",initialValues[i].c_str()) == 0)
			attractionrange = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sticky",initialValues[i].c_str()) == 0)
			sticky = atoi(const_cast<char*>(initialValues[i+1].c_str()));
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
void GroundEmitterNode::inspectPostApply()
{
	Parent::inspectPostApply();
	setMaskBits(StateMask | EmitterDBMask);
}

//-----------------------------------------------------------------------------
// advanceTime
//-----------------------------------------------------------------------------
void GroundEmitterNode::processTick(const Move* move)
{
	Parent::processTick(move);
	bool server = false;
	if(isServerObject())
		server = true;
	else
		server = false;

	mObjBox.minExtents = Point3F(-sa_radius, -sa_radius, -sa_radius);
	mObjBox.maxExtents = Point3F(sa_radius, sa_radius, sa_radius);
	resetWorldBox();

	if ( isMounted() )
	{
		MatrixF mat;
		mMount.object->getMountTransform( mMount.node, mMount.xfm, &mat );
		setTransform( mat );
	}
}

void GroundEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	if(!mActive || mEmitter.isNull() || !mDataBlock)
		return;

	Point3F emitPoint, emitVelocity;
	Point3F emitAxis(0, 0, 1);
	getTransform().mulV(emitAxis);
	getTransform().getColumn(3, &emitPoint);
	emitVelocity = emitAxis * mVelocity;
	// Here we also send 'this' so the emitter can use the nodes data.
	mEmitter->emitParticles( (U32)(dt * mDataBlock->timeMultiple * 1000.0f), this);
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 GroundEmitterNode::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
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
		stream->write( shuttingDown );
	}

	if( stream->writeFlag( mask & emitterEdited) )
	{
		stream->writeInt(sa_ejectionPeriodMS, 10);
		stream->writeInt(sa_periodVarianceMS, 10);
		stream->write(sa_ejectionVelocity);
		stream->write(sa_velocityVariance);
		stream->write(sa_radius);
		stream->write(sa_ejectionOffset);
		stream->writeRangedU32((U32)sa_thetaMin, 0, 180);
		stream->writeRangedU32((U32)sa_thetaMax, 0, 180);

		stream->write(sticky);
		stream->write(attractionrange);
		for(int i = 0; i < attrobjectCount; i++)
		{
			stream->write(AttractionMode[i]);
			stream->write(Amount[i]);
			stream->writeString(attractedObjectID[i]);
			stream->writeString(Attraction_offset[i]);
		}
	}

	if( stream->writeFlag( mask & exprEdited) )
	{
		for(int i = 0; i < 5; i++)
		{
			stream->writeString(layers[i]);
		}
	}
	if( stream->writeFlag(mask & dynamicMod) )
	{

	}
	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void GroundEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
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

		GroundEmitterData *emitterDB = NULL;
		Sim::findObject( mEmitterDatablockId, emitterDB );
		if ( isProperlyAdded() )
			setEmitterDataBlock( emitterDB );
	}

	if ( stream->readFlag() )
	{
		mActive = stream->readFlag();
		stream->read( &mVelocity );

		stream->read( &standAloneEmitter );
		bool startShutdown = false;
		stream->read( &startShutdown );
		if(startShutdown)
			safeDelete();
	}
	//EmitterEdited
	if ( stream->readFlag() )
	{
		sa_ejectionPeriodMS = stream->readInt(10);
		sa_periodVarianceMS = stream->readInt(10);
		stream->read((F32 *)&sa_ejectionVelocity);
		stream->read((F32 *)&sa_velocityVariance);
		stream->read((F32 *)&sa_radius);
		stream->read((F32 *)&sa_ejectionOffset);

		sa_thetaMin = (F32)stream->readRangedU32(0, 180);
		sa_thetaMax = (F32)stream->readRangedU32(0, 180);

		stream->read(&sticky);
		stream->read((F32 *)&attractionrange);
		for(int i = 0; i < attrobjectCount; i++)
		{
			stream->read(&AttractionMode[i]);
			stream->read((F32 *)&Amount[i]);
			char buf[256];
			stream->readString(buf);
			attractedObjectID[i] = dStrdup(buf);
			char buf2[256];
			stream->readString(buf2);
			Attraction_offset[i] = dStrdup(buf2);
		}

		//attractedObjectID = stream->readInt(6);


		if(mEmitter)
		{
			mEmitter->sticky = sticky;
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
	//ExprEditted
	if ( stream->readFlag() )
	{
		for(int i = 0; i < 5; i++)
		{
			layers[i] = stream->readSTString();
		}
	}
	if( stream->readFlag() )
	{

	}
}

//-----------------------------------------------------------------------------
// This function converts uppercase characters to lowercase
//-----------------------------------------------------------------------------
char* GroundEmitterNode::UpToLow(char* c)
{
	for (int i=0; i<strlen(c); i++)
		if(c[i] >= 0x41 && c[i] <= 0x5A)
			c[i] = c[i] + 0x20;
	//tolower(c[i]);
	return c;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void GroundEmitterNode::setEmitterDataBlock(GroundEmitterData* data)
{
	if ( isServerObject() )
	{
		// Update the node's sa values to the new emitter datablocks values.
		/*sa_thetaMax = data->thetaMax;
		sa_thetaMin = data->thetaMin;

		sa_phiReferenceVel = data->phiReferenceVel;
		sa_phiVariance = data->phiVariance;

		sa_ejectionVelocity = data->ejectionVelocity;
		sa_velocityVariance = data->velocityVariance;
		sa_ejectionOffset = data->ejectionOffset;

		sa_ejectionPeriodMS = data->ejectionPeriodMS;
		sa_periodVarianceMS = data->periodVarianceMS;
		| emitterEdited */

		setMaskBits( EmitterDBMask );
	}
	else
	{
		if ( mEmitter && data == mEmitter->getDataBlock() )
			return;
		GroundEmitter* pEmitter = NULL;
		if ( data )
		{
			// Create emitter with new datablock
			pEmitter = new GroundEmitter;
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

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void GroundEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	// Was it an sa_value that was edited? Then set the emitterEdited bit.
	if(strcmp(slotName, "sa_ejectionOffset") == 0 ||
		strcmp(slotName, "sa_ejectionPeriodMS") == 0 ||
		strcmp(slotName, "sa_periodVarianceMS") == 0 ||
		strcmp(slotName, "standAloneEmitter") == 0 ||
		strcmp(slotName, "sa_ejectionVelocity") == 0 ||
		strcmp(slotName, "sa_velocityVariance") == 0 ||
		strcmp(slotName, "sa_thetaMin") == 0 ||
		strcmp(slotName, "sa_thetaMax") == 0 ||
		strcmp(slotName, "sa_phiReferenceVel") == 0 ||
		strcmp(slotName, "sa_phiVariance") == 0 ||
		strcmp(slotName, "attracted") == 0 ||
		strcmp(slotName, "attractionrange") == 0 ||
		strcmp(slotName, "Attraction_offset") == 0 ||
		strcmp(slotName, "Amount") == 0 ||
		strcmp(slotName, "sticky") == 0 ||
		strcmp(slotName, "attractedObjectID") == 0 ||
		strcmp(slotName, "AttractionMode") == 0)
		setMaskBits(emitterEdited);

	// Was it an expression related value? Then set the exprEdited bit.
	if(strcmp(slotName, "layers") == 0 )
	{
		setMaskBits(exprEdited);
	}

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

	// If it was the emitter value which was updated, then update the associated emitter
	if(strcmp(slotName, "emitter") == 0)
	{
		if(isServerObject())
		{
			sa_thetaMax = mEmitterDatablock->thetaMax;
			sa_thetaMin = mEmitterDatablock->thetaMin;

			sa_ejectionVelocity = mEmitterDatablock->ejectionVelocity;
			sa_velocityVariance = mEmitterDatablock->velocityVariance;
			sa_radius = mEmitterDatablock->radius;
			sa_ejectionOffset = mEmitterDatablock->ejectionOffset;

			sa_ejectionPeriodMS = mEmitterDatablock->ejectionPeriodMS;
			sa_periodVarianceMS = mEmitterDatablock->periodVarianceMS;

			setMaskBits(EmitterDBMask);
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void GroundEmitterNode::onDynamicModified(const char* slotName, const char*newValue)
{

}

DefineEngineMethod(GroundEmitterNode, setEmitterDataBlock, void, (GroundEmitterData* emitterDatablock), (0),
	"Assigns the datablock for this emitter node.\n"
	"@param emitterDatablock GroundEmitterData datablock to assign\n"
	"@tsexample\n"
	"// Assign a new emitter datablock\n"
	"%emitter.setEmitterDatablock( %emitterDatablock );\n"
	"@endtsexample\n" )
{
	if ( !emitterDatablock )
	{
		Con::errorf("GroundEmitterData datablock could not be found when calling setEmitterDataBlock in GroundEmitterNode.");
		return;
	}

	object->setEmitterDataBlock(emitterDatablock);
}

DefineEngineMethod(GroundEmitterNode, setActive, void, (bool active),,
	"Turns the emitter on or off.\n"
	"@param active New emitter state\n" )
{
	object->setActive( active );
}

DefineEngineMethod(GroundEmitterNode, safeDelete, void, (void),,
	"Delete the emitter.\n")
{
	object->safeDelete();
}

DefineEngineMethod(GroundEmitterNode, setDataBlock, void, (void),,
	"Delete the emitter.\n")
{
	object->safeDelete();
}