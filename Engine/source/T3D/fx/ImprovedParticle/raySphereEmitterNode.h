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

#ifndef RAYSPHERE_EMITTERNODE_H_
#define RAYSPHERE_EMITTERNODE_H_

#include "T3D\fx\particleEmitterNode.h"
#include "raySphereEmitter.h"

class RaySphereEmitterNodeData : public ParticleEmitterNodeData
{
	typedef ParticleEmitterNodeData Parent;

public:
	RaySphereEmitterNodeData();
	DECLARE_CONOBJECT(RaySphereEmitterNodeData);

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

class RaySphereEmitterNode : public ParticleEmitterNode
{
	typedef ParticleEmitterNode Parent;
	//------- Enums -------
	enum EmitterUpdateBits
	{
		saThetaMin		= Parent::saNextFreeMask << 0,
		saThetaMax		= Parent::saNextFreeMask << 1,
		saPhiRefVel		= Parent::saNextFreeMask << 2,
		saPhiVar		= Parent::saNextFreeMask << 3,
		saNextFreeMask	= Parent::saNextFreeMask << 4
	};

	//------- Functions -------
public:
	RaySphereEmitterNode();
	DECLARE_CONOBJECT(RaySphereEmitterNode);
	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);
	static void initPersistFields();
	void advanceTime(F32 dt);


protected:
	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void onStaticModified(const char* slotName, const char*newValue);
	void onDynamicModified(const char* slotName, const char*newValue);

public:
	virtual RaySphereEmitterNodeData* getDataBlock() { return static_cast<RaySphereEmitterNodeData*>(Parent::getDataBlock()); }
	virtual RaySphereEmitter* getEmitter() { return static_cast<RaySphereEmitter*>(Parent::getEmitter()); }
	virtual ParticleEmitter* createEmitter() { return new RaySphereEmitter; };

	virtual void UpdateEmitterValues();
   virtual void UpdateNodeValues(ParticleEmitterData* data);

	//------- Variables -------
public:
	F32   sa_thetaMin;                           ///< Minimum angle, from the horizontal plane, to eject from
	F32   sa_thetaMax;                           ///< Maximum angle, from the horizontal plane, to eject from

	F32   sa_phiReferenceVel;                    ///< Reference angle, from the verticle plane, to eject from
	F32   sa_phiVariance;                        ///< Varience from the reference angle, from 0 to n

};

#endif // RAYSPHERE_EMITTERNODE_H_
