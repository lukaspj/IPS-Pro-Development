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

#ifndef SPELL_TARGET_H
#define SPELL_TARGET_H

#include "console\consoleTypes.h"
#include "console\simDatablock.h"
#include "T3D\shapeBase.h"

class SpellTargetData : public SimDataBlock
{
	typedef SimDataBlock Parent;
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
	// SpellTargetData
	//--------------------------------------------
	SpellTargetData();

	//Fields
	ShapeBase* mTarget;

	DECLARE_CONOBJECT(SpellTargetData);
};

#endif // SPELL_TARGET_H
