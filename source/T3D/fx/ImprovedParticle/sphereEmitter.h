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

#ifndef SPHERE_EMITTER_H_
#define SPHERE_EMITTER_H_

#include "T3D\fx\particleEmitter.h"

class SphereEmitterData : public ParticleEmitterData
{
	typedef ParticleEmitterData Parent;

public:
	SphereEmitterData();
	DECLARE_CONOBJECT(SphereEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	virtual ParticleEmitter* createEmitter();

	// Variables

	F32   thetaMin;                           ///< Minimum angle, from the horizontal plane, to eject from
	F32   thetaMax;                           ///< Maximum angle, from the horizontal plane, to eject from

	F32   phiReferenceVel;                    ///< Reference angle, from the verticle plane, to eject from
	F32   phiVariance;                        ///< Varience from the reference angle, from 0 to n
};

class SphereEmitter : public ParticleEmitter
{
	typedef ParticleEmitter Parent;

public:
	SphereEmitter();

	DECLARE_CONOBJECT(SphereEmitter);


protected:
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx);

	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node);

private:

private:
	virtual SphereEmitterData* getDataBlock() { return static_cast<SphereEmitterData*>(Parent::getDataBlock()); }
};


#endif // SPHERE_EMITTER_H_
