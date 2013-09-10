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

	//------- Variables -------
public:

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

	//------- Functions -------
public:
	GraphEmitterNode();
	DECLARE_CONOBJECT(GraphEmitterNode);
	static void initPersistFields();
	void advanceTime(F32 dt);


protected:
	bool onNewDataBlock( GameBaseData *dptr, bool reload );

public:
	virtual GraphEmitterNodeData* getDataBlock() { return static_cast<GraphEmitterNodeData*>(Parent::getDataBlock()); };
	virtual GraphEmitter* getEmitter() { return static_cast<GraphEmitter*>(Parent::getEmitter()); };
	virtual ParticleEmitter* createEmitter() { return new GraphEmitter; };
	void updateMaxMinDistances();

	//------- Variables -------
public:
	bool	cb_Max;
	S32		lastErrorTime;

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
