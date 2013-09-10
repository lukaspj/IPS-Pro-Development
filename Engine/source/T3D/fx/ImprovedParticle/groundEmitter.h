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

#ifndef GROUND_EMITTER_H_
#define GROUND_EMITTER_H_

#include "T3D\fx\particleEmitter.h"

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif

#define GroundEmitterLayers (U8)5

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
	F32	 radius;					///< Radius around the node that there will be emitted particles
	StringTableEntry layers[GroundEmitterLayers];		///< Terrain layers that is ignored by the GroundEmitter
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
	
	

protected:
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, const MatrixF &trans);
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node);

	//------- Variables -------
public:
	F32	 sa_radius;					///< Radius around the node that there will be emitted particles
	StringTableEntry sa_layers[5];		///< Terrain layers that is ignored by the GroundEmitter

	//------- Callbacks -------
public:

private:
	virtual GroundEmitterData* getDataBlock() { return static_cast<GroundEmitterData*>(Parent::getDataBlock()); }
};

#endif // GROUND_EMITTER_H_
