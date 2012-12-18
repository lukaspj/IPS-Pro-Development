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

#include "SpellManager.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "console\engineAPI.h"
#include "CastTypes\ScreenCenterCast.h"
#include "CastTypes\AOECast.h"

SpellManager::SpellManager(){
	mTimeSpent = 0;
	mSpellState = SpellState::Idle;
	mCurSpellData = NULL;
	mCurSpell = NULL;
}

void SpellManager::BeginCast(SimObjectId ID, SpellData *SpellDat){
	mSpellState = SpellState::Initializing;
	mTimeSpent = 0;
	mCurSpellData = SpellDat;

	SimObject* obj;
	if(!Sim::findObject(ID,obj))
	{
		Con::errorf("SpellData::BeginCast: %u was not a proper SimObjectId!", ID);
		return;
	}
	ShapeBase* shape = dynamic_cast<ShapeBase*>(obj);
	if(!shape)
	{
		Con::errorf("SpellData::BeginCast: %u was not a proper ShapeBase object!", ID);
		return;
	}
	ICastType* castType;
	switch (SpellDat->mCastType)
	{
	case SpellData::ScreenCenter:
		castType = new ScreenCenterCast();
		break;
	case SpellData::AOE:
		//AOECast::Cast(shape, new Spell());
		break;
	default:
		break;
	}
	mCurSpell = new Spell(castType, shape, SpellDat);
	if(!mCurSpell->registerObject())
	{
		delete mCurSpell;
		mCurSpell = 0;
		Con::errorf("SpellData::BeginCast Failed to register Spell object");
		mSpellState = SpellState::Idle;
	}
}

void SpellManager::advanceTime( F32 TimeDelta )
{
	if(mSpellState != SpellState::Idle)
		mTimeSpent += TimeDelta;
	switch (mSpellState)
	{
	case SpellManager::Initializing:
		mCurSpellData->onChannelBegin_callback(mCurSpellData->getId(), mCurSpell->getId());
		mSpellState = SpellState::PreChannel;
		mTimeSpent = 0;
		break;
	case SpellManager::PreChannel:
		if(mTimeSpent >= mCurSpellData->mChannelTimes[0])
		{
			mCurSpell->Initialize();
			mSpellState = SpellState::Channel;
			mTimeSpent = 0;
		}
		break;
	case SpellManager::Channel:
		break;
	case SpellManager::PostChannel:
		break;
	case SpellManager::PreCast:
		break;
	case SpellManager::Cast:
		break;
	case SpellManager::PostCast:
		break;
	default:
		break;
	}
}