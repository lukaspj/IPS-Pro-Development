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

	enum EmitterUpdateBits
	{
		saXFunc			= Parent::saNextFreeMask << 0,
		saYFunc			= Parent::saNextFreeMask << 1,
		saZFunc			= Parent::saNextFreeMask << 2,
		saReverse		= Parent::saNextFreeMask << 3,
		saLoop			= Parent::saNextFreeMask << 4,
		saGrounded		= Parent::saNextFreeMask << 5,
		saTimeScale		= Parent::saNextFreeMask << 6,
		safuncMax		= Parent::saNextFreeMask << 7,
		safuncMin		= Parent::saNextFreeMask << 8,
		saProgMode		= Parent::saNextFreeMask << 9,
		saNextFreeMask	= Parent::saNextFreeMask << 10
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
	virtual void UpdateEmitterValues();

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
