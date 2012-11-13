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

#ifndef _MASKEMITTERNODE_H_
#define _MASKEMITTERNODE_H_

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

class MaskEmitterData;
class MaskEmitter;

//*****************************************************************************
// MaskEmitterNodeData
//*****************************************************************************
class MaskEmitterNodeData : public GameBaseData
{
	typedef GameBaseData Parent;

protected:
	bool onAdd();

	//-------------------------------------- Console set variables
public:
	F32					timeMultiple;

	bool				standAloneEmitter;
	S32					sa_ejectionPeriodMS;					///< Time, in Milliseconds, between particle ejection
	S32					sa_periodVarianceMS;					///< Varience in ejection peroid between 0 and n

	F32					sa_ejectionVelocity;					///< Ejection velocity
	F32					sa_velocityVariance;					///< Variance for velocity between 0 and n

	F32					sa_ejectionOffset;						///< Z offset from emitter point to eject from

	F32					sa_radius;

	S32					AttractionMode[attrobjectCount];		///< How the objects should interact with the associated objects.
	bool				sticky;
	bool				grounded;
	F32					attractionrange;
	F32					Amount[attrobjectCount];
	StringTableEntry	Attraction_offset[attrobjectCount];
	bool				ParticleCollision;

	//-------------------------------------- load set variables
public:

public:
	MaskEmitterNodeData();
	~MaskEmitterNodeData();

	void packData(BitStream*);
	void unpackData(BitStream*);
	bool preload(bool server, String &errorStr);

	DECLARE_CONOBJECT(MaskEmitterNodeData);
	static void initPersistFields();
};

//*****************************************************************************
// MaskEmitterNode
//*****************************************************************************
class MaskEmitterNode : public GameBase
{
	typedef GameBase Parent;

	enum MaskBits
	{
		StateMask		= Parent::NextFreeMask << 0,
		EmitterDBMask	= Parent::NextFreeMask << 1,
		NextFreeMask	= Parent::NextFreeMask << 2,
		emitterEdited	= Parent::NextFreeMask << 3,
		exprEdited		= Parent::NextFreeMask << 4,
		dynamicMod		= Parent::NextFreeMask << 5,
	};

	char* UpToLow(char* c);

	bool cb_Max;

private:
	MaskEmitterNodeData* mDataBlock;

	bool shuttingDown;


protected:
	bool onAdd();
	void onRemove();
	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void inspectPostApply();

	void MaskEmitterNode::onStaticModified(const char* slotName, const char*newValue);
	void MaskEmitterNode::onDynamicModified(const char* slotName, const char*newValue);

	MaskEmitterData* mEmitterDatablock;
	S32                  mEmitterDatablockId;

	bool             mActive;

	SimObjectPtr<MaskEmitter> mEmitter;
	F32              mVelocity;

public:

	//------------------------- Stand alone variables
	bool	standAloneEmitter;
	S32		sa_ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32		sa_periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n

	F32		sa_ejectionVelocity;                   ///< Ejection velocity
	F32		sa_velocityVariance;                   ///< Variance for velocity between 0 and n
	F32		sa_ejectionOffset;						///< Z offset from emitter point to eject from

	F32		sa_radius;

	S32	AttractionMode[attrobjectCount];		///< How the objects should interact with the associated objects.

	bool	grounded;
	bool	sticky;
	bool	ParticleCollision;
	F32		attractionrange;
	StringTableEntry attractedObjectID[attrobjectCount];
	F32		Amount[attrobjectCount];
	StringTableEntry	Attraction_offset[attrobjectCount];

	std::vector<std::string> initialValues;

	void safeDelete(); //Not used
	bool currentlyShuttingDown() { return shuttingDown; }; ///< For shutting down the viual effects immediately


	MaskEmitterNode();
	~MaskEmitterNode();

	MaskEmitter *getMaskEmitter() {return mEmitter;}

	// Time/Move Management
public:
	void processTick(const Move* move);
	void advanceTime(F32 dt);

	enum EnumAttractionMode{
		none = 0,
		attract = 1,
		repulse = 2,
	};

	DECLARE_CONOBJECT(MaskEmitterNode);
	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

	inline bool getActive( void )        { return mActive;                             };
	inline void setActive( bool active ) { mActive = active; setMaskBits( StateMask ); };

	void setEmitterDataBlock(MaskEmitterData* data);
};

#endif // _MASKEMITTERNODE_H_
