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
   U16 mLifeTimeMS;

	Vector<emitterNode> mEmitterNodes;

	void processTick(const Move* move);
	void advanceTime(F32 dt);

	DECLARE_CONOBJECT(ParticleEffect);
	static void initPersistFields();

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

private:
	void setValue(emitterNode *node, pEffectReader::valueType value, F32 newValue);
	F32 getValue(emitterNode node, pEffectReader::valueType value);
};
#endif // _PARTICLEEFFECT_H_
