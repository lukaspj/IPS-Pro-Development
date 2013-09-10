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

#ifndef NODE_MESH_EMITTER_H_
#define NODE_MESH_EMITTER_H_

#include "meshEmitter.h"

#include "IPSCore.h"
#include "IPSEmitterHelper.h"

#include <vector>

class NodeMeshEmitterData : public MeshEmitterData
{
	typedef MeshEmitterData Parent;

public:
	//--------------------------------------------
	// SimObject
	//--------------------------------------------
	static void initPersistFields() { Parent::initPersistFields(); };
	virtual bool preload(bool server, String &errorStr) { return Parent::preload(server, errorStr); };
	virtual bool onAdd() { return Parent::onAdd(); };

	//--------------------------------------------
	// SimDatablock
	//--------------------------------------------
	virtual void packData(BitStream* stream) { Parent::packData(stream); };
	virtual void unpackData(BitStream* stream) { Parent::unpackData(stream); };

	DECLARE_CONOBJECT(NodeMeshEmitterData);
};

class NodeMeshEmitter : public MeshEmitter
{
	typedef MeshEmitter Parent;

	struct NodeVertex
	{
		//S32 MeshIndex;
		S32 VertIndex;
      S32 ObjIndex;
	};

protected:
	enum MaskBits
	{
		NodeNameMask	= Parent::NextFreeMask << 0,
		NextFreeMask	= Parent::NextFreeMask << 1
	};

public:

	NodeMeshEmitter();

	virtual bool getPointOnFace(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew);
	virtual bool getPointOnVertex(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew);
	virtual void loadFaces(SimObject *SB, ShapeBase *SS, TSStatic* TS);
	
	static void initPersistFields();
	virtual U32 packUpdate(NetConnection *conn, U32 mask, BitStream *stream);
	virtual void unpackUpdate(NetConnection *conn, BitStream* stream);
	virtual void onStaticModified(const char *slotName, const char *newValue);

	Vector<NodeVertex> NodeVertices;
	StringTableEntry NodeName;

	DECLARE_CONOBJECT(NodeMeshEmitter);
};

#endif // NODE_MESH_EMITTER_H_
