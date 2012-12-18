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

#ifndef MASK_EMITTERNODE_H_
#define MASK_EMITTERNODE_H_

#include "T3D\fx\particleEmitterNode.h"
#include "maskEmitter.h"

class MaskEmitterData;
class MaskEmitter;

class MaskEmitterNodeData : public ParticleEmitterNodeData
{
	typedef ParticleEmitterNodeData Parent;

	//------- Functions -------
public:
	MaskEmitterNodeData();
	DECLARE_CONOBJECT(MaskEmitterNodeData);

	static void initPersistFields();
	bool onAdd();
	void packData(BitStream*);
	void unpackData(BitStream*);

	//------- Variables -------
public:
	F32		sa_radius;
	bool	grounded;
};

class MaskEmitterNode : public ParticleEmitterNode
{
	typedef ParticleEmitterNode Parent;

	//------- Enums -------
	enum MaskBits
	{
		NextFreeMask	= Parent::NextFreeMask << 0,
	};

	enum EmitterUpdateBits
	{
		saTresMax		= Parent::saNextFreeMask << 0,
		saTresMin		= Parent::saNextFreeMask << 1,
		saGrounded		= Parent::saNextFreeMask << 2,
		saRadius		= Parent::saNextFreeMask << 3,
		saNextFreeMask	= Parent::saNextFreeMask << 4
	};

	//------- Functions -------
public:
	MaskEmitterNode();
	DECLARE_CONOBJECT(MaskEmitterNode);
	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);
	static void initPersistFields();
	void advanceTime(F32 dt);

protected:
	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void onStaticModified(const char* slotName, const char*newValue);

public:
	virtual MaskEmitterNodeData* getDataBlock() { return static_cast<MaskEmitterNodeData*>(Parent::getDataBlock()); };
	virtual MaskEmitter* getEmitter() { return static_cast<MaskEmitter*>(Parent::getEmitter()); };
	virtual ParticleEmitter* createEmitter() { return new MaskEmitter; };
	void setEmitterDataBlock(ParticleEmitterData* data);
	void UpdateEmitterValues();

	//------- Variables -------
public:
	U8			sa_Treshold_min;
	U8			sa_Treshold_max;
	F32			sa_radius;
	bool		grounded;
};

#endif // MASK_EMITTERNODE_H_
