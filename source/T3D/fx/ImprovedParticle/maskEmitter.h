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

#ifndef MASK_EMITTER_H_
#define MASK_EMITTER_H_

#include "T3D\fx\particleEmitter.h"

#include "math/muParser/muParser.h"

#include "PixelMask.h"

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif

using namespace mu;

class MaskEmitterData : public ParticleEmitterData
{
	typedef ParticleEmitterData Parent;

	//------- Functions -------
public:
	MaskEmitterData();
	DECLARE_CONOBJECT(MaskEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	virtual ParticleEmitter* createEmitter();

	//------- Variables -------
public:
	F32			radius;
	PixelMask*	pMask;
};

class MaskEmitter : public ParticleEmitter
{
	typedef ParticleEmitter Parent;

	//------- Enums -------
	enum MaskBits
	{
		NextFreeMask	= Parent::NextFreeMask << 0,
	};

	//------- Functions -------
public:
	MaskEmitter();
	DECLARE_CONOBJECT(MaskEmitter);

	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void onStaticModified(const char* slotName, const char*newValue);
	
	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);
	
	bool GetTerrainHeightAndNormal(const F32 x, const F32 y, F32 &height, Point3F &normal);

protected:
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx);
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node);

	//------- Variables -------
public:

private:
	virtual MaskEmitterData* getDataBlock() { return static_cast<MaskEmitterData*>(Parent::getDataBlock()); }
};

#endif // GRAPH_EMITTER_H_
