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

#ifndef MASK_EMITTER_H_
#define MASK_EMITTER_H_

#include "T3D\fx\particleEmitter.h"

#include "../PixelMask.h"

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif

//-----------------------------------------------------------
// MaskEmitterData
//-----------------------------------------------------------
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
	U8			Threshold_min;			///< Minimum alpha value
	U8			Threshold_max;			///< Maximum alpha value
	F32			radius;					///< Size of the emission field
	PixelMask*	pMask;					///< Particle mask
};

//-----------------------------------------------------------
// MaskEmitter
//-----------------------------------------------------------
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

protected:
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, const MatrixF &trans);
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node);

	//------- Variables -------
public:
	U8					sa_Treshold_min;	///< Minimum alpha value
	U8					sa_Treshold_max;	///< Maximum alpha value
	U8					sa_Radius;			///< Size of the emission field
	bool				sa_Grounded;		///< If true, emits particles along the terrain
	PixelMask::Cache	maskCache;			///< Cache for optimization
private:
	virtual MaskEmitterData* getDataBlock() { return static_cast<MaskEmitterData*>(Parent::getDataBlock()); }
};

#endif // GRAPH_EMITTER_H_
