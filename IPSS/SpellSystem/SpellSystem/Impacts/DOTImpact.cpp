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

#include "DOTImpact.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CONOBJECT(DOTImpact);

//---------------------------------------------------------------
// Constructor
//---------------------------------------------------------------
DOTImpact::DOTImpact()
{
	TimeSpent = 0;
	TickMS = 0;
	TickCount = 0;
	TicksSpent = 0;
}

//----------------------CastType-----------------------



//----------------------- ITickable -------------------------
void DOTImpact::interpolateTick( F32 delta ) 
{
}
void DOTImpact::processTick() 
{
}
void DOTImpact::advanceTime( F32 timeDelta ) 
{
	TimeSpent += timeDelta;
	U32 TimeSinceTick = TimeSpent - (TicksSpent * TickMS);
	while( TimeSinceTick >= TickMS )
	{
		doImpact_callback(getId());
		TicksSpent++;
		
		if(TicksSpent >= TickCount)
			return;

		TimeSinceTick = TimeSpent - (TicksSpent * TickMS);
	}
}


//----------------------- SimObject -------------------------
//---------------------------------------------------------------
// InitPersistFields
//--------------------------------------------------------------
void DOTImpact::initPersistFields()
{
	addField("TickMS", TypeS32, Offset(TickMS, DOTImpact));
	addField("TickCount", TypeS32, Offset(TickCount, DOTImpact));

	Parent::initPersistFields();
}

//---------------------------------------------------------------
// OnAdd
//---------------------------------------------------------------
bool DOTImpact::onAdd()
{
	if( !Parent::onAdd() )
		return false;
	return true;
}