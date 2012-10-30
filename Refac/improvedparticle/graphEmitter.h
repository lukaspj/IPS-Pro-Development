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

#ifndef GRAPH_EMITTER_H_
#define GRAPH_EMITTER_H_

#include "T3D\fx\particleEmitter.h"

#include "math/muParser/muParser.h"

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif

using namespace mu;

class GraphEmitterData : public ParticleEmitterData
{
	typedef ParticleEmitterData Parent;

	//------- Functions -------
public:
	GraphEmitterData();
	DECLARE_CONOBJECT(GraphEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	virtual ParticleEmitter* createEmitter();

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

class GraphEmitter : public ParticleEmitter
{
	typedef ParticleEmitter Parent;

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
	GraphEmitter();
	DECLARE_CONOBJECT(GraphEmitter);

	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void onStaticModified(const char* slotName, const char*newValue);
	void onDynamicModified(const char* slotName, const char*newValue);
	
	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);
	
	bool GetTerrainHeightAndNormal(const F32 x, const F32 y, F32 &height, Point3F &normal);

protected:
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx);
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node);

	//------- Variables -------
public:
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

private:
	U32	oldTime;
	Point3F parentNodePos;

	//------- Callbacks -------
public:
	void onBoundaryLimit(bool Max, ParticleEmitterNode* node);				///< onBoundaryLimit callback handler

private:
	virtual GraphEmitterData* getDataBlock() { return static_cast<GraphEmitterData*>(Parent::getDataBlock()); }
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

#endif // GRAPH_EMITTER_H_
