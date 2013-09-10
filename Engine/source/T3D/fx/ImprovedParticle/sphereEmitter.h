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

	bool onNewDataBlock( GameBaseData *dptr, bool reload ) { return Parent::onNewDataBlock(dptr, reload); };
	void onStaticModified(const char* slotName, const char*newValue) { Parent::onStaticModified(slotName, newValue); };
	
	static void initPersistFields() { Parent::initPersistFields(); };

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream) { return Parent::packUpdate(conn, mask, stream); };
	void unpackUpdate(NetConnection *conn,           BitStream* stream) {Parent::unpackUpdate(conn, stream); };


protected:
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, const MatrixF &trans);

	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node);

private:
	virtual SphereEmitterData* getDataBlock() { return static_cast<SphereEmitterData*>(Parent::getDataBlock()); }

public:
	// Variables

	F32   sa_thetaMin;                           ///< Minimum angle, from the horizontal plane, to eject from
	F32   sa_thetaMax;                           ///< Maximum angle, from the horizontal plane, to eject from

	F32   sa_phiReferenceVel;                    ///< Reference angle, from the verticle plane, to eject from
	F32   sa_phiVariance;                        ///< Varience from the reference angle, from 0 to n
};


#endif // SPHERE_EMITTER_H_
