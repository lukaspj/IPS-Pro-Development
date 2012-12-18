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

#include "Spell.h"
#include "CastTypes\ScreenCenterCast.h"
#include "CastTypes\AOECast.h"
#include "console\engineAPI.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(SpellData);
IMPLEMENT_CONOBJECT(Spell);

IMPLEMENT_CALLBACK(SpellData, onChannelBegin, void, (SimObjectId dID, SimObjectId sID),(dID, sID), "")
IMPLEMENT_CALLBACK(SpellData, onChannel, void, (SimObjectId dID, SimObjectId sID),(dID, sID), "");
IMPLEMENT_CALLBACK(SpellData, onChannelEnd, void, (SimObjectId dID, SimObjectId sID),(dID, sID), "");

IMPLEMENT_CALLBACK(SpellData, onPreCast, void, (SimObjectId dID, SimObjectId sID),(dID, sID), "")
IMPLEMENT_CALLBACK(SpellData, onCast, void, (SimObjectId dID, SimObjectId sID),(dID, sID), "");
IMPLEMENT_CALLBACK(SpellData, onPostCast, void, (SimObjectId dID, SimObjectId sID),(dID, sID), "");

typedef SpellData::CastTypes CastType;
DefineEnumType( CastType );

ImplementEnumType( CastType,
	"How the spells should be cast.\n"
	"@ingroup SpellSystem\n\n")
{ CastType::ScreenCenter,		"ScreenCenter",			"Casts the spell in the middle of the screen.\n" },
{ CastType::AOE,				"AOE",					"Spawns a spellindicator and casts the spell on next input (ala AOE's in WoW).\n" },
EndImplementEnumType;


//---------------------------------------------------------------
// Constructor
//---------------------------------------------------------------
SpellData::SpellData()
{
	mChannelTimes = new U32[3];
	mCastTimes = new U32[3];
}

SpellData::~SpellData()
{
	delete mChannelTimes;
	delete mCastTimes;
}

//----------------------- SpellData -------------------------
//---------------------------------------------------------------
// BeginCast
//---------------------------------------------------------------
void SpellData::beginCast(SimObjectId ID)
{
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
	switch (mCastType)
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
	Spell* sp = new Spell(castType, shape, this);
	if(!sp->registerObject())
	{
		delete sp;
		sp = 0;
		Con::errorf("SpellData::BeginCast Failed to register Spell object");
	}
	else{
		sp->Initialize();
	}
}

//----------------------- SimDataBlock -------------------------
//--------------------------------------------------------------
// InitPersistFields
//--------------------------------------------------------------
void SpellData::initPersistFields()
{
	addField("CastType", TYPEID<CastType>(), Offset(mCastType, SpellData), "");

	addField("ChannelTimesMS", TYPEID< U32 >(), Offset(mChannelTimes, SpellData), 3, "");
	
	addField("CastTimesMS", TYPEID< U32 >(), Offset(mCastTimes, SpellData), 3, "");

	Parent::initPersistFields();
}

//---------------------------------------------------------------
// OnAdd
//---------------------------------------------------------------
bool SpellData::onAdd()
{
	if( !Parent::onAdd() )
		return false;
	return true;
}

//---------------------------------------------------------------
// PackData
//--------------------------------------------------------------
void SpellData::packData(BitStream* stream)
{
	for(int i = 0; i < 3; i++)
	{
		stream->writeInt(mChannelTimes[i], 15);
		stream->writeInt(mCastTimes[i], 15);
	}
	stream->writeInt(mCastType, 3);
	Parent::packData(stream);
}

//---------------------------------------------------------------
// UnpackData
//--------------------------------------------------------------
void SpellData::unpackData(BitStream* stream)
{
	for(int i = 0; i < 3; i++)
	{
		mChannelTimes[i] = stream->readInt(15);
		mCastTimes[i] = stream->readInt(15);
	}
	mCastType = (CastType)stream->readInt(3);
	Parent::unpackData(stream);
}

//--------------------------------------------------------------
// Preload
//--------------------------------------------------------------
bool SpellData::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;
	// Verify variables

	return true;
}

//--------------------------------------------------------------
// Console functions
//--------------------------------------------------------------
DefineEngineMethod(SpellData, beginCast, void, (SimObjectId ID), (U32_MAX), "Usage")
{
	if(ID == U32_MAX)
		return;
	object->beginCast(ID);
}


//----------------------- Spell -------------------------
//--------------------------------------------------------------
// onAdd
//--------------------------------------------------------------
bool Spell::onAdd()
{
	if( !Parent::onAdd() )
		return false;
	return true;
}

Spell::Spell(ICastType* _CastType, ShapeBase* _target, SpellData* _Datablock)
{
	mCastType = _CastType;
	mTarget = _target;
	mDataBlock = _Datablock;
}

Spell::~Spell()
{
	delete mCastType;
}

void Spell::Initialize()
{
	mCastType->Cast(mTarget, this);
}

void Spell::Cast(Point3F begin, Point3F end)
{
	mDataBlock->onChannelEnd_callback(mDataBlock->getId(), getId());
	mDataBlock->onPreCast_callback(mDataBlock->getId(), getId());
}

void Spell::onChannel() { mDataBlock->onChannel_callback(mDataBlock->getId(), getId()); }