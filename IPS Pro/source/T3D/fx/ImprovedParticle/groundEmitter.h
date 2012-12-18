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

#ifndef GROUND_EMITTER_H_
#define GROUND_EMITTER_H_

#include "T3D\fx\particleEmitter.h"

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif

class GroundEmitterData : public ParticleEmitterData
{
	typedef ParticleEmitterData Parent;

	//------- Functions -------
public:
	GroundEmitterData();
	DECLARE_CONOBJECT(GroundEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	virtual ParticleEmitter* createEmitter();

	//------- Variables -------
public:
	F32   radius;
};

class GroundEmitter : public ParticleEmitter
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
	GroundEmitter();
	DECLARE_CONOBJECT(GroundEmitter);

	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	
	static void initPersistFields();
	
	bool	GetTerrainHeightAndMat(const F32 &x, const F32 &y, F32 &height, StringTableEntry &mat, Point3F &normal);

protected:
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, const MatrixF &trans);
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node);

	//------- Variables -------
public:
	F32		sa_Radius;
private:
	Point3F parentNodePos;

	//------- Callbacks -------
public:

private:
	virtual GroundEmitterData* getDataBlock() { return static_cast<GroundEmitterData*>(Parent::getDataBlock()); }
};

#endif // GROUND_EMITTER_H_
