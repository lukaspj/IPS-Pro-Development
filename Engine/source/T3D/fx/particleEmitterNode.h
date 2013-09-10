#ifndef _PARTICLEEMITTERDUMMY_H_
#define _PARTICLEEMITTERDUMMY_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif

#include <string>
#include <vector>
#include <sstream>

#include "scene\sceneObject.h"
#include "T3D\shapeBase.h"

#include "math/mTransform.h"
#include "ts\tsShapeInstance.h"

class ParticleEmitterData;
class ParticleEmitter;

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

	//DECLARE_CONOBJECT(ParticleEmitterNodeData);
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
	bool onAdd();
	void onRemove();
	virtual bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void inspectPostApply();

	ParticleEmitterData* mEmitterDatablock;
	S32                  mEmitterDatablockId;

	bool             mActive;

	ParticleEmitter* mEmitter;
public:
	virtual ParticleEmitter* getEmitter() { return mEmitter; }
	virtual ParticleEmitterNodeData* getDataBlock() { return mDataBlock; }
	void setmDataBlock(ParticleEmitterNodeData* dat) { mDataBlock = dat; }
	virtual ParticleEmitter* createEmitter() = 0;

	F32 mVelocity;

	void onStaticModified(const char* slotName, const char*newValue);

public:
	//------------------------- Stand alone variables
	Box3F	boxTest;

	bool mNodeMounted;

	ParticleEmitterNode();
	~ParticleEmitterNode();

	// Time/Move Management
	void processTick(const Move* move);
	virtual void advanceTime(F32 dt);

	//DECLARE_CONOBJECT(ParticleEmitterNode);

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

#endif // _H_PARTICLEEMISSIONDUMMY