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

#ifndef SPHERE_EMITTERNODE_H_
#define SPHERE_EMITTERNODE_H_

#include "T3D\fx\particleEmitterNode.h"
#include "sphereEmitter.h"

class SphereEmitterData;
class SphereEmitter;

class SphereEmitterNodeData : public ParticleEmitterNodeData
{
	typedef ParticleEmitterNodeData Parent;

public:
	SphereEmitterNodeData();
	DECLARE_CONOBJECT(SphereEmitterNodeData);

	static void initPersistFields();
	bool onAdd();
	void packData(BitStream*);
	void unpackData(BitStream*);

public:
	F32   sa_thetaMin;                           ///< Minimum angle, from the horizontal plane, to eject from
	F32   sa_thetaMax;                           ///< Maximum angle, from the horizontal plane, to eject from

	F32   sa_phiReferenceVel;                    ///< Reference angle, from the verticle plane, to eject from
	F32   sa_phiVariance;                        ///< Varience from the reference angle, from 0 to n
};

class SphereEmitterNode : public ParticleEmitterNode
{
	typedef ParticleEmitterNode Parent;

public:
	SphereEmitterNode();
	DECLARE_CONOBJECT(SphereEmitterNode);
	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);
	static void initPersistFields();
	void advanceTime(F32 dt);


protected:
	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void onStaticModified(const char* slotName, const char*newValue);
	void onDynamicModified(const char* slotName, const char*newValue);

public:
	F32   sa_thetaMin;                           ///< Minimum angle, from the horizontal plane, to eject from
	F32   sa_thetaMax;                           ///< Maximum angle, from the horizontal plane, to eject from

	F32   sa_phiReferenceVel;                    ///< Reference angle, from the verticle plane, to eject from
	F32   sa_phiVariance;                        ///< Varience from the reference angle, from 0 to n

private:
	//SphereEmitterNodeData* mDataBlock;
	virtual SphereEmitterNodeData* getDataBlock() { return static_cast<SphereEmitterNodeData*>(Parent::getDataBlock()); }
	virtual SphereEmitter* getEmitter() { return static_cast<SphereEmitter*>(Parent::getEmitter()); }
	virtual ParticleEmitter* createEmitter() { return new SphereEmitter; };
	//SimObjectPtr<SphereEmitter> mEmitter;
};

#endif // SPHERE_EMITTERNODE_H_
