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

#include "SpellTarget.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(SpellTargetData);

//---------------------------------------------------------------
// Constructor
//---------------------------------------------------------------
SpellTargetData::SpellTargetData()
{
	mTarget = NULL;
}

//----------------------- SimDataBlock -------------------------
//---------------------------------------------------------------
// InitPersistFields
//--------------------------------------------------------------
void SpellTargetData::initPersistFields()
{

	Parent::initPersistFields();
}

//---------------------------------------------------------------
// OnAdd
//---------------------------------------------------------------
bool SpellTargetData::onAdd()
{
	if( !Parent::onAdd() )
		return false;
	return true;
}

//---------------------------------------------------------------
// PackData
//--------------------------------------------------------------
void SpellTargetData::packData(BitStream* stream)
{
	Parent::packData(stream);
}

//---------------------------------------------------------------
// UnpackData
//--------------------------------------------------------------
void SpellTargetData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
}

//---------------------------------------------------------------
// Preload
//--------------------------------------------------------------
bool SpellTargetData::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;
	// Verify variables

	return true;
}