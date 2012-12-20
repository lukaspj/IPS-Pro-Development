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

	saUpdateBits = 0;
	for(int i = 0; i < ParticleBehaviourCount; i++)
		ParticleBHVs[i] = NULL;
}

ParticleEmitterNode::~ParticleEmitterNode()
{
}

void ParticleEmitterNode::initPersistFields()
{
	addField( "active", TYPEID< bool >(), Offset(mActive,ParticleEmitterNode),
		"Controls whether particles are emitted from this node." );
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

	addGroup( "ParticleBehaviours" );

	addField("ParticleBehaviour", TYPEID<IParticleBehaviour>(), Offset(ParticleBHVs, ParticleEmitterNode), ParticleBehaviourCount,
		"Null");

	endGroup( "ParticleBehaviours" );

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

	standAloneEmitter = mDataBlock->standAloneEmitter;

	sa_ejectionPeriodMS = mDataBlock->sa_ejectionPeriodMS;
	sa_periodVarianceMS = mDataBlock->sa_periodVarianceMS;

	sa_ejectionVelocity = mDataBlock->sa_ejectionVelocity;
	sa_velocityVariance = mDataBlock->sa_velocityVariance;
	sa_ejectionOffset = mDataBlock->sa_ejectionOffset;

	for(int i = 0; i < initialValues.size(); i=i+2)
	{
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
	}
	saUpdateBits = U32_MAX;
	UpdateEmitterValues();

	// Todo: Uncomment if this is a "leaf" class
	scriptOnNewDataBlock();
	return true;
}

void ParticleEmitterNode::inspectPostApply()
{
	Parent::inspectPostApply();
	//setMaskBits(StateMask | EmitterDBMask);
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

	if(mEmitter)
		mEmitter->parentNodePos = getRenderPosition();
}

void ParticleEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	if(!mActive || !mEmitter || !mEmitter->isProperlyAdded() || !mDataBlock)
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
	}

	if( stream->writeFlag( mask & emitterEdited) )
	{
		stream->writeFlag( standAloneEmitter );

		stream->writeInt(sa_ejectionPeriodMS, 10);
		stream->writeInt(sa_periodVarianceMS, 10);
		
		stream->writeInt((S32)(sa_ejectionVelocity * 100), 16);
		stream->writeInt((S32)(sa_velocityVariance * 100), 14);
		stream->writeInt((S32)(sa_ejectionOffset * 100), 16);
	}
	if(stream->writeFlag(saUpdateBits & saBehaviour)){
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
	if(stream->writeFlag(saUpdateBits))
		stream->writeInt(saUpdateBits, 32);
	saUpdateBits = 0;

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
	}
	if ( stream->readFlag() )
	{
		standAloneEmitter = stream->readFlag();

		sa_ejectionPeriodMS = stream->readInt(10);
		sa_periodVarianceMS = stream->readInt(10);

		sa_ejectionVelocity = stream->readInt(16) / 100.0f;
		sa_velocityVariance = stream->readInt(14) / 100.0f;
		sa_ejectionOffset = stream->readInt(16) / 100.0f;
	}
	if(stream->readFlag())
	{
		for(int i = 0; i < ParticleBehaviourCount; i++)
		{
			if ( stream->readFlag() )
			{
				SimDataBlock *dptr = 0;
				SimObjectId id = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );
				Sim::findObject( id, dptr );
				if(dptr)
					ParticleBHVs[i] = (IParticleBehaviour*)dptr;
			}
		}
	}
	if(stream->readFlag()){
		saUpdateBits |= stream->readInt(32);
		if(mEmitter)
			UpdateEmitterValues();
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
			UpdateEmitterValues();
	}

	mEmitterDatablock = data;
}

void ParticleEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	if( strcmp(slotName, "standAloneEmitter") == 0 ){
		saUpdateBits |= saEmitter;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "sa_ejectionPeriodMS") == 0 ){
		saUpdateBits |= saPeriod;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "sa_periodVarianceMS") == 0 ){
		saUpdateBits |= saPeriodVar;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "sa_ejectionVelocity") == 0 ){
		saUpdateBits |= saVel;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "sa_velocityVariance") == 0 ){
		saUpdateBits |= saVelVar;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "sa_ejectionOffset") == 0 ){
		saUpdateBits |= saOffset;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "sa_ejectionOffset") == 0 ){
		saUpdateBits |= saOffset;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "Grounded") == 0 ){
		saUpdateBits |= saGrounded;
		setMaskBits(emitterEdited);
	}
	if( strcmp(slotName, "ParticleBehaviour") == 0 ){
		saUpdateBits |= saBehaviour;
		setMaskBits( emitterEdited );
	}

	if(!isProperlyAdded())
	{
		//Save the values and add them onNewDataBlock
		initialValues.push_back(std::string(slotName));
		initialValues.push_back(std::string(newValue));
	}
}

void ParticleEmitterNode::UpdateEmitterValues()
{
	if(!mEmitter)
		return;

	if(saEmitter & saUpdateBits)
		mEmitter->standAloneEmitter = standAloneEmitter;
	if(saPeriod & saUpdateBits)
		mEmitter->sa_ejectionPeriodMS = sa_ejectionPeriodMS;
	if(saPeriodVar & saUpdateBits)
		mEmitter->sa_periodVarianceMS = sa_periodVarianceMS;
	
	if(saVel & saUpdateBits)
		mEmitter->sa_ejectionVelocity = sa_ejectionVelocity;
	if(saVelVar & saUpdateBits)
		mEmitter->sa_velocityVariance = sa_velocityVariance;
	if(saOffset & saUpdateBits)
		mEmitter->sa_ejectionOffset = sa_ejectionOffset;
	
	if(saGrounded & saUpdateBits)
		mEmitter->grounded = sa_grounded;

	if(saBehaviour & saUpdateBits)
		for(int i = 0; i < ParticleBehaviourCount; i++)
			mEmitter->ParticleBHVs[i] = ParticleBHVs[i];

	saUpdateBits = 0;
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