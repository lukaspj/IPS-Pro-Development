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

#ifndef _Ground_EMITTERNODE_H_
#define _Ground_EMITTERNODE_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif

#include "math/muParser/muParser.h"

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif

#ifndef _BITSTREAM_H_
#include "core/stream/bitStream.h"
#endif

using namespace mu;
#ifndef attrobjectCount
#define attrobjectCount (U8)2
#endif

class GroundEmitterData;
class GroundEmitter;

//*****************************************************************************
// GroundEmitterNodeData
//*****************************************************************************
class GroundEmitterNodeData : public GameBaseData
{
	typedef GameBaseData Parent;

protected:
	bool onAdd();

	//-------------------------------------- Console set variables
public:
	F32 timeMultiple;

	bool standAloneEmitter;
	S32   sa_ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32   sa_periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n

	F32   sa_ejectionVelocity;                   ///< Ejection velocity
	F32   sa_velocityVariance;                   ///< Variance for velocity between 0 and n

	F32   sa_thetaMin;                           ///< Minimum angle, from the horizontal plane, to eject from
	F32   sa_thetaMax;                           ///< Maximum angle, from the horizontal plane, to eject from

	F32   sa_ejectionOffset;						///< Z offset from emitter point to eject from

	F32	 sa_radius;

	S32		AttractionMode[attrobjectCount];							///< How the objects should interact with the associated objects.
	bool		sticky;
	F32		attractionrange;
	F32		Amount[attrobjectCount];
	StringTableEntry	Attraction_offset[attrobjectCount];

	//-------------------------------------- load set variables
public:

public:
	GroundEmitterNodeData();
	~GroundEmitterNodeData();

	void packData(BitStream*);
	void unpackData(BitStream*);
	bool preload(bool server, String &errorStr);

	DECLARE_CONOBJECT(GroundEmitterNodeData);
	static void initPersistFields();
};

//*****************************************************************************
// GroundEmitterNode
//*****************************************************************************
class GroundEmitterNode : public GameBase
{
	typedef GameBase Parent;

	enum MaskBits
	{
		StateMask      = Parent::NextFreeMask << 0,
		EmitterDBMask  = Parent::NextFreeMask << 1,
		NextFreeMask   = Parent::NextFreeMask << 2,
		emitterEdited	 = Parent::NextFreeMask << 3,
		exprEdited	 = Parent::NextFreeMask << 4,
		dynamicMod	 = Parent::NextFreeMask << 5,
	};

	char* UpToLow(char* c);

	bool cb_Max;

private:
	GroundEmitterNodeData* mDataBlock;

	bool shuttingDown;


protected:
	bool onAdd();
	void onRemove();
	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void inspectPostApply();

	void GroundEmitterNode::onStaticModified(const char* slotName, const char*newValue);
	void GroundEmitterNode::onDynamicModified(const char* slotName, const char*newValue);

	GroundEmitterData* mEmitterDatablock;
	S32                  mEmitterDatablockId;

	bool             mActive;

	SimObjectPtr<GroundEmitter> mEmitter;
	F32              mVelocity;

public:

	//------------------------- Stand alone variables
	bool standAloneEmitter;
	S32   sa_ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32   sa_periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n

	F32   sa_ejectionVelocity;                   ///< Ejection velocity
	F32   sa_velocityVariance;                   ///< Variance for velocity between 0 and n

	F32   sa_thetaMin;                           ///< Minimum angle, from the horizontal plane, to eject from
	F32   sa_thetaMax;                           ///< Maximum angle, from the horizontal plane, to eject from
	F32   sa_ejectionOffset;						///< Z offset from emitter point to eject from

	F32	 sa_radius;
	StringTableEntry layers[5];

	S32	AttractionMode[attrobjectCount];		///< How the objects should interact with the associated objects.

	bool		sticky;
	F32		attractionrange;
	StringTableEntry attractedObjectID[attrobjectCount];
	F32		Amount[attrobjectCount];
	StringTableEntry	Attraction_offset[attrobjectCount];

	std::vector<std::string> initialValues;

	void safeDelete(); //Not used
	bool currentlyShuttingDown() { return shuttingDown; }; ///< For shutting down the viual effects immediately


	GroundEmitterNode();
	~GroundEmitterNode();

	GroundEmitter *getGroundEmitter() {return mEmitter;}

	// Time/Move Management
public:
	void processTick(const Move* move);
	void advanceTime(F32 dt);

	enum EnumAttractionMode{
		none = 0,
		attract = 1,
		repulse = 2,
	};

	DECLARE_CONOBJECT(GroundEmitterNode);
	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

	inline bool getActive( void )        { return mActive;                             };
	inline void setActive( bool active ) { mActive = active; setMaskBits( StateMask ); };

	void setEmitterDataBlock(GroundEmitterData* data);
};

#endif // _Ground_EMITTERNODE_H_
