#ifndef _PARTICLEEMITTERDUMMY_H_
#define _PARTICLEEMITTERDUMMY_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif

#include <string>
#include <vector>
#include <sstream>

#ifndef PARTICLE_BEHAVIOUR_H
#include "ImprovedParticle\ParticleBehaviours\particleBehaviour.h"
#endif

#include "scene\sceneObject.h"
#include "T3D\shapeBase.h"

#include "math/mTransform.h"
#include "ts\tsShapeInstance.h"
#include "core/stream/bitStream.h"

#include "sim\netConnection.h"

#include "ImprovedParticle\ParticleBehaviours\attractionBehaviour.h"

class ParticleEmitterData;
class ParticleEmitter;
class IParticleBehaviour;

//*****************************************************************************
// ParticleEmitterNodeData
//*****************************************************************************
class ParticleEmitterNodeData : public GameBaseData
{
	typedef GameBaseData Parent;

protected:
	bool onAdd();

	//-------------------------------------- Console set variables
public:
	F32 timeMultiple;

	ParticleEmitterNodeData();
	~ParticleEmitterNodeData();

	void packData(BitStream*);
	void unpackData(BitStream*);
	bool preload(bool server, String &errorStr);

	DECLARE_CONOBJECT(ParticleEmitterNodeData);
	static void initPersistFields();
};


//*****************************************************************************
// ParticleEmitterNode
//*****************************************************************************
class ParticleEmitterNode : public GameBase
{
	typedef GameBase Parent;

private:
	ParticleEmitterNodeData* mDataBlock;

protected:
	ParticleEmitterData* mEmitterDatablock;
	S32                  mEmitterDatablockId;

	bool             mActive;

	ParticleEmitter* mEmitter;
public:
	bool onAdd();
	void onRemove();
	virtual bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void inspectPostApply();

	virtual ParticleEmitter* getEmitter() { return mEmitter; }
	virtual ParticleEmitterNodeData* getDataBlock() { return mDataBlock; }
	void setmDataBlock(ParticleEmitterNodeData* dat) { mDataBlock = dat; }

	F32 mVelocity;

	void onStaticModified(const char* slotName, const char*newValue);
   
   static bool setEmitterProperty( void *object, const char *index, const char *data );

   void addParticleBehaviour(IParticleBehaviour* bhv, bool overrideLast);

public:
	//------------------------- Stand alone variables
	Box3F	boxTest;

	bool mNodeMounted;

   Vector< NetEvent* > mPendingEvents;
   bool mGhosted;

	ParticleEmitterNode();
	~ParticleEmitterNode();

	// Time/Move Management
	void processTick(const Move* move);
	virtual void advanceTime(F32 dt);

	DECLARE_CONOBJECT(ParticleEmitterNode);

	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

	inline bool getActive( void )        { return mActive;                             };
	inline void setActive( bool active ) { mActive = active; setMaskBits( StateMask ); };
	inline F32 getVelocity( void )			{ return mVelocity;							};
	bool isNodeMounted( void)				{ return mNodeMounted; };
	void mountObjectToNode( ShapeBase *obj, S32 node, const MatrixF &xfm );
	
	virtual void setEmitterDataBlock(ParticleEmitterData* data);

protected:
	enum MaskBits
	{
		StateMask      = Parent::NextFreeMask << 0,
		EmitterDBMask  = Parent::NextFreeMask << 1,
		emitterEdited	 = Parent::NextFreeMask << 2,
		NextFreeMask   = Parent::NextFreeMask << 3,
	};
};			

class ParticleBehaviourNetEvent : public NetEvent
{
   typedef NetEvent Parent;
public:
   IParticleBehaviour* _bhv;
   ParticleEmitterNode* _node;
   bool _overrideLast;

   ParticleBehaviourNetEvent()
   {
   }

   ParticleBehaviourNetEvent(IParticleBehaviour* bhv, ParticleEmitterNode* emitter, bool overrideLast)
   {
      _bhv = bhv;
      _node = emitter;
      _overrideLast = overrideLast;
   }

   void pack(NetConnection* conn, BitStream *stream)  
   {
      S32 ghostID = conn->getGhostIndex(_node);
      if(stream->writeFlag(ghostID != -1))
      {
         stream->writeRangedU32(U32(ghostID), 0, NetConnection::MaxGhostCount);
      }
      else
         _node = NULL;
      if(stream->writeFlag(_bhv->isClientOnly()))
      {
         if(stream->writeFlag(_bhv->getBehaviourType() != behaviourClass::Error))
         {
            stream->writeInt(_bhv->getBehaviourType(), 3);
            _bhv->packUpdate(stream, conn);
         }
      }
      else
      {
         stream->writeRangedU32(_bhv->getId(), DataBlockObjectIdFirst,
            DataBlockObjectIdLast);
      }
   }

   void write(NetConnection* conn, BitStream *stream)  
   {  
      pack(conn, stream);
   }

   void unpack(NetConnection* conn, BitStream *stream)  
   {
      if(stream->readFlag())
      {
         S32 TargetID = stream->readRangedU32(0, NetConnection::MaxGhostCount);
         _node = dynamic_cast<ParticleEmitterNode*>(conn->resolveGhost(TargetID));
      }
      else
         _node = NULL;
      if(stream->readFlag())
      {
         if(stream->readFlag())
         {
            U8 type = stream->readInt(3);
            switch (type)
            {
               case behaviourClass::AttractionBehaviour:
                  _bhv = new AttractionBehaviour();
                  break;
               default:
                  break;
            }
            if(_bhv)
               _bhv->unpackUpdate(stream, conn);
         }
      }
      else
      {
         SimDataBlock *dptr = 0;
         SimObjectId id = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
         if(	Sim::findObject( id, dptr )	)
            _bhv = (IParticleBehaviour*)dptr;
      }
   }

   void process(NetConnection *conn)
   {
      _node->addParticleBehaviour(_bhv, _overrideLast);
   }

   DECLARE_CONOBJECT(ParticleBehaviourNetEvent);  
};

#endif // _H_PARTICLEEMISSIONDUMMY