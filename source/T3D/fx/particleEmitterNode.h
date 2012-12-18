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

#ifndef ParticleBehaviourCount
#define ParticleBehaviourCount (U8)8
#endif

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

	//------------------------- Stand alone variables
	bool	standAloneEmitter;
	S32		sa_ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32		sa_periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n

	F32		sa_ejectionVelocity;                   ///< Ejection velocity
	F32		sa_velocityVariance;                   ///< Variance for velocity between 0 and n
	F32		sa_ejectionOffset;                     ///< Z offset from emitter point to eject from

	bool	sa_grounded;

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
public:

	F32 mVelocity;

	void ParticleEmitterNode::onStaticModified(const char* slotName, const char*newValue);
	void ParticleEmitterNode::onDynamicModified(const char* slotName, const char*newValue);

public:
	//------------------------- Stand alone variables
	bool	standAloneEmitter;
	S32		sa_ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32		sa_periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n
	
	F32		sa_ejectionVelocity;                   ///< Ejection velocity
	F32		sa_velocityVariance;                   ///< Variance for velocity between 0 and n
	F32		sa_ejectionOffset;                     ///< Z offset from emitter point to eject from
	bool	sa_grounded;
	SimDataBlock* ParticleBHVs[ParticleBehaviourCount];

	std::vector<std::string> initialValues;

	ParticleEmitterNode();
	~ParticleEmitterNode();

	enum EnumAttractionMode{
		none = 0,
		attract = 1,
		repulse = 2,
	};

	// Time/Move Management
	void processTick(const Move* move);
	virtual void UpdateEmitterValues();
	virtual void advanceTime(F32 dt);

	//DECLARE_CONOBJECT(ParticleEmitterNode);

	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

	inline bool getActive( void )        { return mActive;                             };
	inline void setActive( bool active ) { mActive = active; setMaskBits( StateMask ); };
	inline F32 getVelocity( void )			{ return mVelocity;							};

	virtual void setEmitterDataBlock(ParticleEmitterData* data);

protected:
	enum MaskBits
	{
		StateMask      = Parent::NextFreeMask << 0,
		EmitterDBMask  = Parent::NextFreeMask << 1,
		emitterEdited	 = Parent::NextFreeMask << 2,
		NextFreeMask   = Parent::NextFreeMask << 3,
	};

	enum EmitterUpdateBits
	{
		saEmitter		= BIT(0),
		saPeriod		= BIT(1),
		saPeriodVar		= BIT(2),
		saVel			= BIT(3),
		saVelVar		= BIT(4),
		saOffset		= BIT(5),
		saGrounded		= BIT(6),
		saBehaviour		= BIT(7),
		saNextFreeMask	= BIT(8)
	};
	U32 saUpdateBits;
};

#endif // _H_PARTICLEEMISSIONDUMMY