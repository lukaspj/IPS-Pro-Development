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

#ifndef SPELL_H
#define SPELL_H

#include "SpellTarget.h"
#include "PickMethods\PickMethod.h"
#include "console\consoleTypes.h"
#include "console\simDatablock.h"
#include "T3D\shapeBase.h"
#include "SpellDecalManager.h"

class SpellData : public SimDataBlock
{
	typedef SimDataBlock Parent;

public:
	//------- Enums -------
	enum PickType{
		ScreenCenter = 0,
		AOE,
		Self,
		Target,
		ERROR
	};
	enum TargetType{
		Object = 0,
		Point,
		None
	};
public:
	//--------------------------------------------
	// SimDataBlock
	//--------------------------------------------
	bool onAdd();
	void packData(BitStream*);
	void unpackData(BitStream*);
	bool preload(bool server, String &errorStr);
	static void initPersistFields();
	
	//--------------------------------------------
	// SpellData
	//--------------------------------------------
	SpellData();
	~SpellData();

	//------- Fields -------
	PickType	mPickType;
	TargetType	mTargetType;
	U32			mChannelTimes[3];
	U32			mCastTimes[3];
	U32			mTypeMask;
	F32			mRange;
	U32			mCooldown;
	F32			mCost;

	/// Name of the bitmap file.  If this is 'texhandle' the bitmap is not loaded
	/// from a file but rather set explicitly on the control.
	String mLogo;

	SpellDecalManagerData*	mDecalManagerData;

	//------- Callbacks -------
	DECLARE_CALLBACK(void, onInitializeCast, (SimObjectId spellID));

	DECLARE_CALLBACK(void, onChannelBegin, (SimObjectId spellID));
	DECLARE_CALLBACK(void, onChannel, (SimObjectId spellID));
	DECLARE_CALLBACK(void, onChannelEnd, (SimObjectId spellID));

	DECLARE_CALLBACK(void, onPreCast, (SimObjectId spellID));
	DECLARE_CALLBACK(void, onCast, (SimObjectId spellID));
	DECLARE_CALLBACK(void, onPostCast, (SimObjectId spellID));

	DECLARE_CONOBJECT(SpellData);
};

class Spell : public SimObject
{
	typedef SimObject Parent;
	
	//--------------------------------------------
	// SimObject
	//--------------------------------------------
	bool onAdd();
	
	//--------------------------------------------
	// Spell
	//--------------------------------------------
public:
	Spell();
	~Spell();
	static void initPersistFields();
	
	//------- Fields -------
	SpellTarget	mTarget;
	SceneObject* mSource;
	NetConnection* mClient;
	SpellData*	mDataBlock;

	DECLARE_CONOBJECT(Spell);
};

#endif // SPELL_H
