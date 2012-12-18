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

#ifndef SPELL_H
#define SPELL_H

#include "CastTypes\CastType.h"
#include "console\consoleTypes.h"
#include "console\simDatablock.h"
#include "T3D\shapeBase.h"

class SpellData : public SimDataBlock
{
	typedef SimDataBlock Parent;

public:
	//------- Enums -------
	enum CastTypes{
		ScreenCenter = 0,
		AOE
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
	void beginCast(SimObjectId ID);

	//------- Fields -------
	CastTypes	mCastType;
	U32*		mChannelTimes;
	U32*		mCastTimes;

	//------- Callbacks -------
	DECLARE_CALLBACK(void, onChannelBegin, (SimObjectId dID, SimObjectId sID));
	DECLARE_CALLBACK(void, onChannel, (SimObjectId dID, SimObjectId sID));
	DECLARE_CALLBACK(void, onChannelEnd, (SimObjectId dID, SimObjectId sID));

	DECLARE_CALLBACK(void, onPreCast, (SimObjectId dID, SimObjectId sID));
	DECLARE_CALLBACK(void, onCast, (SimObjectId dID, SimObjectId sID));
	DECLARE_CALLBACK(void, onPostCast, (SimObjectId dID, SimObjectId sID));

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
	Spell(ICastType* castType, ShapeBase* target, SpellData* datablock);
	~Spell();
	void Initialize();
	void Cast(Point3F begin, Point3F end);
	void onChannel();
	
	//------- Fields -------
	ICastType *mCastType;
	ShapeBase *mTarget;
	SpellData *mDataBlock;


	DECLARE_CONOBJECT(Spell);
};

#endif // SPELL_H
