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

#ifndef RADIUS_MESH_EMITTER_H_
#define RADIUS_MESH_EMITTER_H_

#include "meshEmitter.h"

#include "IPSCore.h"
#include "IPSEmitterHelper.h"

#include <vector>

class RadiusMeshEmitterData : public MeshEmitterData
{
	typedef MeshEmitterData Parent;
	DECLARE_CONOBJECT(RadiusMeshEmitterData);
};

class RadiusMeshEmitter : public MeshEmitter
{
	typedef MeshEmitter Parent;

protected:
	enum MaskBits
	{
		NextFreeMask	= Parent::NextFreeMask << 0
	};

public:

	RadiusMeshEmitter();

	virtual bool getPointOnFace(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew);
	virtual bool getPointOnVertex(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew);
	virtual void loadFaces();
	virtual void debugRenderDelegate(ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance* overrideMat);
	virtual void bindDelegate(ObjectRenderInst *ori) { ori->renderDelegate.bind( this,  &RadiusMeshEmitter::debugRenderDelegate ); };
	
	static void initPersistFields();
	virtual U32 packUpdate(NetConnection *conn, U32 mask, BitStream *stream);
	virtual void unpackUpdate(NetConnection *conn, BitStream* stream);
	virtual void onStaticModified(const char *slotName, const char *newValue);

	virtual void prepRenderImage(SceneRenderState* state) { Parent::prepRenderImage(state); };

	// RadiusMesh variables ----------------------------------------------------------------
	Point3F center;
	F32 radius;
	bool active;
	void tickToDeath(bool boolean) { mDeleteWhenEmpty = boolean; 
	if(isServerObject())
      setMaskBits(MeshEmitterMask);};

	Vector<IPSCore::face> emitfaces;

	std::vector<std::string> initialValues;
	std::vector<std::string> anotherValues;
	//--- Debug
	bool renderIntersections;
	bool renderEmittingFaces;
	bool shadeByCoverage;
	bool renderSphere;
	//--- Debug
	// RadiusMesh variables ----------------------------------------------------------------

	DECLARE_CONOBJECT(RadiusMeshEmitter);
};

#endif // RADIUS_MESH_EMITTER_H_
