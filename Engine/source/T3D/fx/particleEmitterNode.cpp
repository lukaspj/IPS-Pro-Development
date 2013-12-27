#include "particleEmitterNode.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "T3D/fx/particleEmitter.h"
#include "math/mathIO.h"
#include "sim/netConnection.h"
#include "console/engineAPI.h"
#include "console\simPersistID.h"
#include "math/mTransform.h"
#include "ts\tsShapeInstance.h"

#include "ImprovedParticle\ParticleBehaviours\attractionBehaviour.h"

IMPLEMENT_CO_DATABLOCK_V1(ParticleEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(ParticleEmitterNode);

//-----------------------------------------------------------------------------
// ParticleEmitterNodeData
//-----------------------------------------------------------------------------
ParticleEmitterNodeData::ParticleEmitterNodeData()
{
   timeMultiple = 1.0;
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
}

void ParticleEmitterNodeData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   stream->read(&timeMultiple);
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

   mNodeMounted = false;
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
   
   addProtectedField( "emitter",  TYPEID< ParticleEmitterData >(), Offset(mEmitterDatablock, ParticleEmitterNode), 
         &setEmitterProperty, &defaultProtectedGetFn, 
		"Datablock to use when emitting particles." );

   Parent::initPersistFields();
}



bool ParticleEmitterNode::setEmitterProperty(void *obj, const char *index, const char *db)
{
   if( db == NULL || !db || !db[ 0 ] )
   {
      Con::errorf( "GameBase::setDataBlockProperty - Can't unset datablock on GameBase objects" );
      return false;
   }
   
   ParticleEmitterNode* object = static_cast< ParticleEmitterNode* >( obj );
   ParticleEmitterData* data;
   if( Sim::findObject( db, data ) ) {
      object->setEmitterDataBlock( data );
      return true;
   }
   
   Con::errorf( "ParticleEmitterNode::setEmitterProperty - Could not find data block \"%s\"", db );
   return false;
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
   setGlobalBounds();
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
      if(!mNodeMounted)
         mMount.object->getMountTransform( mMount.node, mMount.xfm, &mat );
      else{
         MatrixF nodeTransform = dynamic_cast<ShapeBase*>(mMount.object)->getShapeInstance()->mNodeTransforms[mMount.node];  
         const Point3F& scale = getScale();  

         // The position of the mount point needs to be scaled.  
         Point3F position = nodeTransform.getPosition();  
         position.convolve( scale );  
         nodeTransform.setPosition( position );  

         // Also we would like the object to be scaled to the model.  
         mat.mul(mMount.object->getTransform(), nodeTransform);  
      }
      setTransform( mat );
   }

   if(mEmitter)
      mEmitter->parentNodePos = getRenderPosition();

   if(mEmitter){
      Point3F min = mEmitter->getObjBox().minExtents;
      Point3F max = mEmitter->getObjBox().maxExtents;
      Point3F cmin = mObjBox.minExtents;
      Point3F cmax = mObjBox.maxExtents;
      mObjBox.minExtents.setMin(min);
      mObjBox.maxExtents.setMax(max);
      boxTest = getObjBox();
      resetWorldBox();
      resetRenderWorldBox();/*
                            if( mSceneManager != NULL )
                            mSceneManager->notifyObjectDirty( this );*/
      mEmitter->setScale(getScale());
   }
}

void ParticleEmitterNode::advanceTime(F32 dt)
{
   Parent::advanceTime(dt);

   if(!mActive || !mEmitter || !mEmitter->isProperlyAdded() || !mDataBlock)
      return;

   mEmitter->emitParticles( (U32)(dt * mDataBlock->timeMultiple * 1000.0f), this );
}

U32 ParticleEmitterNode::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate(conn, mask, stream);

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

   stream->writeFlag(mNodeMounted);

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

   mNodeMounted = stream->readFlag();
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
   }

   mEmitterDatablock = data;
}

void ParticleEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
   if( strcmp(slotName, "position") == 0){
      setMaskBits(InitialUpdateMask);
   }
}

DefineEngineMethod( ParticleEmitterNode, mountToNode, bool,
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

DefineEngineMethod(ParticleEmitterNode, setActive, void, (bool active),,
                   "Turns the emitter on or off.\n"
                   "@param active New emitter state\n" )
{
   object->setActive( active );
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

DefineEngineMethod(ParticleEmitterNode, getEmitter, SimObject*,(),,
                   "")
{
   return object->getEmitter();
}