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

#ifndef _PARTICLEEFFECT_H_
#define _PARTICLEEFFECT_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif
#ifndef _PEFFECTREADER_H_
#include "core\util\xml\pEffectReader.h"
#endif
#include "core\util\tVector.h"
class ParticleEffectData;
class ParticleEffect;
class EmitterTemplate;

//*****************************************************************************
// ParticleEffectData
//*****************************************************************************
class ParticleEffectData : public GameBaseData
{
	typedef GameBaseData Parent;

protected:
	bool onAdd();

public:
	ParticleEffectData();
	~ParticleEffectData();

	void onStaticModified(const char* slotName, const char*newValue);

	StringTableEntry pEffectPath;
	Vector<std::string> initialValues;
	Vector<pEffectReader::emitter> emitters;
	U16 lifeTimeMS;

	void packData(BitStream*);
	void unpackData(BitStream*);
	bool preload(bool server, String &errorStr);

	DECLARE_CONOBJECT(ParticleEffectData);
	static void initPersistFields();
};

//*****************************************************************************
// ParticleEffect
//*****************************************************************************
class ParticleEffect : public GameBase
{
	typedef GameBase Parent;

private:
	ParticleEffectData* mDataBlock;
	bool hasSynced;

protected:
	bool onAdd();
	void onRemove();
	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	void inspectPostApply();

public:
	enum MaskBits
	{
		StateMask		= Parent::NextFreeMask << 0,
		EmittersMask	= Parent::NextFreeMask << 1,
		resetMask		= Parent::NextFreeMask << 2,
	};

	ParticleEffect();
	~ParticleEffect();

	struct emitterNode{
		GameBase* node;
		GameBaseData* emitter;
		pEffectReader::emitter emitterData;
		enum states{ notActivated = 1, Activated = 2, hasActivated = 3 };
		states state;
	};
	U32 startTime;

	Vector<emitterNode> mEmitterNodes;

	void processTick(const Move* move);
	void advanceTime(F32 dt);

	DECLARE_CONOBJECT(ParticleEffect);
	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

private:
	void setValue(emitterNode node, std::string value, F32 newValue);
	F32 getValue(emitterNode node, std::string value);
};
#endif // _PARTICLEEFFECT_H_
