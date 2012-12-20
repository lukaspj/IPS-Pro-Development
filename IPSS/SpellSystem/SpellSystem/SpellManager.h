//-----------------------------------------------------------------------------
// IPS Pro
// Copyright Lukas J�rgensen 2012 - FuzzyVoidStudio
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

#ifndef SPELL_MANAGER_H
#define SPELL_MANAGER_H

#include "console/consoleTypes.h"
#include "Spell.h"
#include "core\iTickable.h"
#include "console\simDatablock.h"

class SpellManager : SimObject, public virtual ITickable{
	typedef Parent SimObject;
	
	enum SpellState{
		Idle,
		Initializing,
		PreChannel,
		Channel,
		PostChannel,
		PreCast,
		Cast,
		PostCast
	};

	//--------------------------------------------
	// ITickable
	//--------------------------------------------
	virtual void interpolateTick( F32 delta );
    virtual void processTick();
    virtual void advanceTime( F32 timeDelta );

	//--------------------------------------------
	// SimObject
	//--------------------------------------------
	virtual bool onAdd();
	static void initPersistFields();


	
	//--------------------------------------------
	// SpellManager
	//--------------------------------------------
	SpellManager();
	void BeginCast(SimObjectId ID, SpellData *spellDat);
	
	//--------------------------------------------
	// Fields
	//--------------------------------------------
	F32			mTimeSpent;
	SpellData*	mCurSpellData;
	Spell*		mCurSpell;
	SpellState	mSpellState;
	
	DECLARE_CONOBJECT(SpellManager);
};

#endif // SPELL_MANAGER_H