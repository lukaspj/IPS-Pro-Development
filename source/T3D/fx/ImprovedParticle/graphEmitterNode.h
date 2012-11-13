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

#ifndef GRAPH_EMITTERNODE_H_
#define GRAPH_EMITTERNODE_H_

#include "T3D\fx\particleEmitterNode.h"
#include "graphEmitter.h"

class GraphEmitterData;
class GraphEmitter;

class GraphEmitterNodeData : public ParticleEmitterNodeData
{
	typedef ParticleEmitterNodeData Parent;

	//------- Functions -------
public:
	GraphEmitterNodeData();
	DECLARE_CONOBJECT(GraphEmitterNodeData);

	static void initPersistFields();
	bool onAdd();
	void packData(BitStream*);
	void unpackData(BitStream*);

	//------- Variables -------
public:
	char*	xFunc;
	char*	yFunc;
	char*	zFunc;

	S32		funcMax;
	S32		funcMin;
	S32		particleProg;

	U8		ProgressMode;

	bool	Reverse;
	bool	Loop;
	bool	Grounded;

	F32		timeScale;

	//------- Callbacks -------
	DECLARE_CALLBACK( void, onBoundaryLimit, ( GameBase* obj, bool Max) );
};

class GraphEmitterNode : public ParticleEmitterNode
{
	typedef ParticleEmitterNode Parent;

	//------- Enums -------
	enum MaskBits
	{
		exprEdited		= Parent::NextFreeMask << 0,
		dynamicMod		= Parent::NextFreeMask << 1,
		NextFreeMask	= Parent::NextFreeMask << 2,
	};

public:
	enum EnumProgressMode {
		byParticleCount = 0,
		byTime,
	};

	//------- Functions -------
public:
	GraphEmitterNode();
	DECLARE_CONOBJECT(GraphEmitterNode);
	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);
	static void initPersistFields();
	void advanceTime(F32 dt);


protected:
	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void onStaticModified(const char* slotName, const char*newValue);
	void onDynamicModified(const char* slotName, const char*newValue);

public:
	virtual GraphEmitterNodeData* getDataBlock() { return static_cast<GraphEmitterNodeData*>(Parent::getDataBlock()); };
	virtual GraphEmitter* getEmitter() { return static_cast<GraphEmitter*>(Parent::getEmitter()); };
	virtual ParticleEmitter* createEmitter() { return new GraphEmitter; };
	void updateMaxMinDistances();
	void setEmitterDataBlock(ParticleEmitterData* data);

	//------- Variables -------
public:
	bool shuttingDown;

	char*	xFunc;									///< The expression that calculates the x-coordinate of new particles
	char*	yFunc;									///< The expression that calculates the y-coordinate of new particles
	char*	zFunc;									///< The expression that calculates the z-coordinate of new particles

	S32		funcMax;								///< The maximum value of t
	S32		funcMin;								///< The minimum value of t
	S32		ProgressMode;							///< How to increment the t value

	F32		particleProg;							///< The t value

	bool	Reverse;								///< If true, decrements the t value instead
	bool	Loop;									///< Keep inside the boundary limits or break them?
	bool	Grounded;
	bool cb_Max;

	F32	timeScale;								///< Amount to speed up the emitter

	Parser xfuncParser;							///< The parser for xFunc
	Parser yfuncParser;							///< The parser for yFunc
	Parser zfuncParser;							///< The parser for zFunc

	struct muVar{								///< A muParser variable struct
		F32 value;
		char token;
	};

	muVar xVariables[100];						///< All the variables for the xfuncParser
	muVar yVariables[100];						///< All the variables for the yfuncParser
	muVar zVariables[100];						///< All the variables for the zfuncParser

	F32 xMxDist;
	F32 xMnDist;
	F32 yMxDist;
	F32 yMnDist;
	F32 zMxDist;
	F32 zMnDist;
	
	//------- Callbacks -------
public:
	void onBoundaryLimit(bool Max);				///< onBoundaryLimit callback handler
};

//*****************************************************************************
// GraphEmitterNetEvemt
//*****************************************************************************
// A simple NetEvent to transmit a string over the network.
// This is based on the code in netTest.cc

class nodeCallbackEvent : public NetEvent
{
	typedef NetEvent Parent;
	bool Max;
	S32 mNode;
public:
	nodeCallbackEvent(S32 node = -1, bool max = false);
	virtual ~nodeCallbackEvent();

	virtual void pack   (NetConnection *conn, BitStream *bstream);
	virtual void write  (NetConnection *conn, BitStream *bstream);
	virtual void unpack (NetConnection *conn, BitStream *bstream);
	virtual void process(NetConnection *conn);

	DECLARE_CONOBJECT(nodeCallbackEvent);
};

#endif // GRAPH_EMITTERNODE_H_
=======
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

#ifndef _GRAPH_EMITTERNODE_H_
#define _GRAPH_EMITTERNODE_H_

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

class GraphEmitterData;
class GraphEmitter;

//*****************************************************************************
// GraphEmitterNodeData
//*****************************************************************************
class GraphEmitterNodeData : public GameBaseData
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
	F32   sa_ejectionOffset;                     ///< Z offset from emitter point to eject from

	char* xFunc;
	char* yFunc;
	char* zFunc;
	S32   funcMax;
	S32   funcMin;
	S32	 particleProg;

	S32		AttractionMode[attrobjectCount];							///< How the objects should interact with the associated objects.
	bool		sticky;
	bool		ParticleCollision;
	F32		attractionrange;
	F32		Amount[attrobjectCount];
	StringTableEntry	Attraction_offset[attrobjectCount];

	U8	ProgressMode;
	bool Reverse;

	bool Loop;

	bool	Grounded;

	F32	timeScale;

	DECLARE_CALLBACK( void, onBoundaryLimit, ( GameBase* obj, bool Max) );

	//-------------------------------------- load set variables
public:

public:
	GraphEmitterNodeData();
	~GraphEmitterNodeData();

	void packData(BitStream*);
	void unpackData(BitStream*);
	bool preload(bool server, String &errorStr);

	DECLARE_CONOBJECT(GraphEmitterNodeData);
	static void initPersistFields();
};

//*****************************************************************************
// GraphEmitterNode
//*****************************************************************************
class GraphEmitterNode : public GameBase
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
	GraphEmitterNodeData* mDataBlock;

	bool shuttingDown;


protected:
	bool onAdd();
	void onRemove();
	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void inspectPostApply();

	void GraphEmitterNode::onStaticModified(const char* slotName, const char*newValue);
	void GraphEmitterNode::onDynamicModified(const char* slotName, const char*newValue);

	GraphEmitterData* mEmitterDatablock;
	S32                  mEmitterDatablockId;

	bool             mActive;

	SimObjectPtr<GraphEmitter> mEmitter;
	F32              mVelocity;

public:

	//------------------------- Stand alone variables
	bool standAloneEmitter;
	S32   sa_ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32   sa_periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n

	F32   sa_ejectionVelocity;                   ///< Ejection velocity
	F32   sa_velocityVariance;                   ///< Variance for velocity between 0 and n
	F32   sa_ejectionOffset;                     ///< Z offset from emitter point to eject from

	char* xFunc;									///< The expression that calculates the x-coordinate of new particles
	char* yFunc;									///< The expression that calculates the y-coordinate of new particles
	char* zFunc;									///< The expression that calculates the z-coordinate of new particles
	S32   funcMax;								///< The maximum value of t
	S32   funcMin;								///< The minimum value of t
	F32	 particleProg;							///< The t value

	S32	ProgressMode;							///< How to increment the t value
	S32	AttractionMode[attrobjectCount];		///< How the objects should interact with the associated objects.
	bool Reverse;								///< If true, decrements the t value instead
	bool Loop;									///< Keep inside the boundary limits or break them?

	bool		sticky;
	bool		ParticleCollision;
	bool		Grounded;
	F32		attractionrange;
	StringTableEntry attractedObjectID[attrobjectCount];
	F32		Amount[attrobjectCount];
	StringTableEntry	Attraction_offset[attrobjectCount];

	F32	timeScale;								///< Amount to speed up the emitter

	Parser xfuncParser;							///< The parser for xFunc
	Parser yfuncParser;							///< The parser for yFunc
	Parser zfuncParser;							///< The parser for zFunc

	struct muVar{								///< A muParser variable struct
		F32 value;
		char token;
	};

	muVar xVariables[100];						///< All the variables for the xfuncParser
	muVar yVariables[100];						///< All the variables for the yfuncParser
	muVar zVariables[100];						///< All the variables for the zfuncParser

	F32 xMxDist;
	F32 xMnDist;
	F32 yMxDist;
	F32 yMnDist;
	F32 zMxDist;
	F32 zMnDist;

	void updateMaxMinDistances();

	std::vector<std::string> initialValues;

	void onBoundaryLimit(bool Max);				///< onBoundaryLimit callback handler

	void safeDelete(); //Not used
	bool currentlyShuttingDown() { return shuttingDown; }; ///< For shutting down the viual effects immediately


	GraphEmitterNode();
	~GraphEmitterNode();

	GraphEmitter *getGraphEmitter() {return mEmitter;}

	// Time/Move Management
public:
	void processTick(const Move* move);
	void advanceTime(F32 dt);

	enum EnumProgressMode {
		byParticleCount = 0,
		byTime,
	};

	enum EnumAttractionMode{
		none = 0,
		attract = 1,
		repulse = 2,
	};

	DECLARE_CONOBJECT(GraphEmitterNode);
	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

	inline bool getActive( void )        { return mActive;                             };
	inline void setActive( bool active ) { mActive = active; setMaskBits( StateMask ); };

	void setEmitterDataBlock(GraphEmitterData* data);
};


//*****************************************************************************
// GraphEmitterNetEvemt
//*****************************************************************************
// A simple NetEvent to transmit a string over the network.
// This is based on the code in netTest.cc

class CallbackEvent : public NetEvent
{
	typedef NetEvent Parent;
	bool Max;
	S32 mNode;
public:
	CallbackEvent(S32 node = -1, bool max = false);
	virtual ~CallbackEvent();

	virtual void pack   (NetConnection *conn, BitStream *bstream);
	virtual void write  (NetConnection *conn, BitStream *bstream);
	virtual void unpack (NetConnection *conn, BitStream *bstream);
	virtual void process(NetConnection *conn);

	DECLARE_CONOBJECT(CallbackEvent);
};

//*****************************************************************************
// GraphEmitterNetEvemt
//*****************************************************************************
// A simple NetEvent to transmit a string over the network.
// This is based on the code in netTest.cc

class DeleteEvent : public NetEvent
{
	typedef NetEvent Parent;
	S32 mNode;
public:
	DeleteEvent(S32 node = -1);
	virtual ~DeleteEvent();

	virtual void pack   (NetConnection *conn, BitStream *bstream);
	virtual void write  (NetConnection *conn, BitStream *bstream);
	virtual void unpack (NetConnection *conn, BitStream *bstream);
	virtual void process(NetConnection *conn);

	DECLARE_CONOBJECT(DeleteEvent);
};

#endif // _GRAPH_EMITTERNODE_H_

