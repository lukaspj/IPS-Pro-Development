// Copyright (C) 2013 Winterleaf Entertainment L,L,C.
// 
// THE SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND,
// INCLUDING WITHOUT LIMITATION THE WARRANTIES OF MERCHANT ABILITY, FITNESS
// FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT. THE ENTIRE RISK AS TO THE
// QUALITY AND PERFORMANCE OF THE SOFTWARE IS THE RESPONSIBILITY OF LICENSEE.
// SHOULD THE SOFTWARE PROVE DEFECTIVE IN ANY RESPECT, LICENSEE AND NOT LICEN-
// SOR OR ITS SUPPLIERS OR RESELLERS ASSUMES THE ENTIRE COST OF ANY SERVICE AND
// REPAIR. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS
// AGREEMENT. NO USE OF THE SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
// 
// The use of the WinterLeaf Entertainment LLC Improved Particle System Bundle (IPS Bundle)
// is governed by this license agreement.
// 
// RESTRICTIONS
// 
// (a) Licensee may not: (i) create any derivative works of IPS Bundle, including but not
// limited to translations, localizations, technology add-ons, or game making software
// other than Games without express permission from Winterleaf Entertainment; (ii) redistribute, 
// encumber , sell, rent, lease, sublicense, or otherwise
// transfer rights to IPS Bundle; or (iii) remove or alter any trademark, logo, copyright
// or other proprietary notices, legends, symbols or labels in IPS Bundle; or (iv) use
// the Software to develop or distribute any software that competes with the Software
// without WinterLeaf Entertainment's prior written consent; or (v) use the Software for
// any illegal purpose.
// (b) Licensee may not distribute the IPS Bundle in any manner.
// 
// LICENSEGRANT.
// This license allows companies of any size, government entities or individuals to cre-
// ate, sell, rent, lease, or otherwise profit commercially from, games using executables
// created from the source code of IPS Bundle
// 
// Please visit http://www.winterleafentertainment.com for more information about the project and latest updates.

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

	enum EnumProgressMode {
		byParticleCount = 0,
		byTime,
	};

	GraphEmitterData();
	DECLARE_CONOBJECT(GraphEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	virtual ParticleEmitter* createEmitter();

	//------- Variables -------
public:
	char*	xFunc;					///< The expression that calculates the x-coordinate of new particles
	char*	yFunc;					///< The expression that calculates the y-coordinate of new particles
	char*	zFunc;					///< The expression that calculates the z-coordinate of new particles

	S32		funcMax;				///< The upper boundary for the t-value
	S32		funcMin;				///< The lower boundary for the t-value
	S32		particleProg;			///< The t-value

   EnumProgressMode ProgressMode;			///< Enum that defines how the t-value rises

	bool	Reverse;				///< If true, the t-value is falling
	bool	Loop;					///< If true, the t-value will iterate between the upper and the lower boundary
	bool	mGrounded;				///< If true, particles will be emitted along the terrain

	F32	mTimeScale;				///< A coefficient for the t-value

	//------- Callbacks -------
	// onBoundaryLimit is called when the t-value reaches funcMax or funcMin
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

	//------- Functions -------
public:
	GraphEmitter();
	DECLARE_CONOBJECT(GraphEmitter);

	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void onStaticModified(const char* slotName, const char*newValue);
	virtual void onDynamicModified(const char* slotName, const char*newValue);
	
	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

protected:
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, const MatrixF& trans);
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node);

	//------- Variables -------
public:
	char*	xFunc;								///< The expression that calculates the x-coordinate of new particles
	char*	yFunc;								///< The expression that calculates the y-coordinate of new particles
	char*	zFunc;								///< The expression that calculates the z-coordinate of new particles

	S32		funcMax;							///< The upper boundary for the t-value
	S32		funcMin;							///< The lower boundary for the t-value
	F32		particleProg;						///< The t-value

	S32		ProgressMode;						///< Enum that defines how the t-value rises

	bool	Reverse;							///< If true, the t-value is falling
	bool	Loop;								///< If true, the t-value will iterate between the upper and the lower boundary
   bool  mGrounded;                 ///> If true, the particles will be emitted along the terrain

	F32		timeScale;							///< A coefficient for the t-value

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

	F32 xMxDist;								///< The maximal x-distance the xFunc can hit
	F32 xMnDist;								///< The maximal y-distance the xFunc can hit
	F32 yMxDist;								///< The maximal z-distance the xFunc can hit
	F32 yMnDist;								///< The minimum x-distance the xFunc can hit
	F32 zMxDist;								///< The minimum y-distance the xFunc can hit
	F32 zMnDist;								///< The minimum z-distance the xFunc can hit

private:
	bool cb_Max;								///< Internal boolean to tell wether the boundary was hit max or low
	U32 lastErrorTime;
	
public:
	//------- MuParser custom functions -------
	static F32 mu_RandomInteger() { return gRandGen.randI(); };
	static F32 mu_SeededRandomInteger(F32 seed) { gRandGen.setSeed(seed); return gRandGen.randI(); };
	static F32 mu_RandomFloat() { return gRandGen.randF(); };
	static F32 mu_SeededRandomFloat(F32 seed) { gRandGen.setSeed(seed); return gRandGen.randF(); };
	static F32 mu_ModulusOprt(F32 val, F32 mod) { return F32( U32(val) % U32(mod) ); };

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
   virtual void write  (NetConnection *conn, BitStream *bstream) { pack(conn, bstream); };
	virtual void unpack (NetConnection *conn, BitStream *bstream);
	virtual void process(NetConnection *conn);

	DECLARE_CONOBJECT(CallbackEvent);
};

#endif // GRAPH_EMITTER_H_
