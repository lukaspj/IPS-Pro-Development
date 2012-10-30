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

#ifndef GROUND_EMITTERNODE_H_
#define GROUND_EMITTERNODE_H_

#include "T3D\fx\particleEmitterNode.h"
#include "groundEmitter.h"

class GroundEmitterData;
class GroundEmitter;

class GroundEmitterNodeData : public ParticleEmitterNodeData
{
	typedef ParticleEmitterNodeData Parent;

	//------- Functions -------
public:
	GroundEmitterNodeData();
	DECLARE_CONOBJECT(GroundEmitterNodeData);

	static void initPersistFields();
	bool onAdd();
	void packData(BitStream*);
	void unpackData(BitStream*);

	//------- Variables -------
public:
	F32	 sa_radius;
};

class GroundEmitterNode : public ParticleEmitterNode
{
	typedef ParticleEmitterNode Parent;

	//------- Enums -------
	enum MaskBits
	{
		layerEdited		= Parent::NextFreeMask << 0,
		NextFreeMask	= Parent::NextFreeMask << 1,
	};

	//------- Functions -------
public:
	GroundEmitterNode();
	DECLARE_CONOBJECT(GroundEmitterNode);
	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);
	static void initPersistFields();
	void advanceTime(F32 dt);


protected:
	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void onStaticModified(const char* slotName, const char*newValue);

public:
	virtual GroundEmitterNodeData* getDataBlock() { return static_cast<GroundEmitterNodeData*>(Parent::getDataBlock()); };
	virtual GroundEmitter* getEmitter() { return static_cast<GroundEmitter*>(Parent::getEmitter()); };
	virtual ParticleEmitter* createEmitter() { return new GroundEmitter; };
	void updateMaxMinDistances();

	//------- Variables -------
public:
	F32	 sa_radius;
	StringTableEntry layers[5];
};

#endif // GROUND_EMITTERNODE_H_
