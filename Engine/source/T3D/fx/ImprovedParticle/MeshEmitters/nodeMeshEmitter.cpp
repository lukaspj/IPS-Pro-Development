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

#include "nodeMeshEmitter.h"
#include "core/stream/bitStream.h"

#include "ts\tsShapeInstance.h"
#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "T3D\staticShape.h"
#include "ts\TSMeshIntrinsics.h"

IMPLEMENT_CO_DATABLOCK_V1(NodeMeshEmitterData);
IMPLEMENT_CO_NETOBJECT_V1(NodeMeshEmitter);

NodeMeshEmitter::NodeMeshEmitter()
{
	NodeName = "";
	Parent();
}

void NodeMeshEmitter::initPersistFields()
{
	addGroup("NodeMeshEmitter");

	addField("NodeName", TYPEID<StringTableEntry>(), Offset(NodeName, NodeMeshEmitter), "");

	endGroup("NodeMeshEmitter");
	Parent::initPersistFields();
}


bool NodeMeshEmitter::getPointOnVertex(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew)
{
	PROFILE_SCOPE(nodeMeshEmitVert);

	F32 initialVel = ejectionVelocity;
	initialVel    += (velocityVariance * 2.0f * gRandGen.randF()) - velocityVariance;
	// Seed the random generator - this should maybe be swapped out in favor for the gRandGen.
	//  - Although they both work fine.
	srand(mainTime);
	// Throw out some trash results. Apparently the first 1-2 results fails to give proper random numbers.
	int trash = rand();
	trash = rand();
	if(mainTime < 0)
		mainTime = 0;
	NodeVertex NV = NodeVertices[mainTime % NodeVertices.size()];
	// If evenEmission is on, set the co to a random number for the per vertex emission.
	if(evenEmission && NodeVertices.size() > 0)
		NV = NodeVertices[gRandGen.randI() % NodeVertices.size()];
	mainTime++;

	const TSShapeInstance* model;
	TSShape* shape;
	if(SS)
		model = SS->getShapeInstance();
	else{
		model = TS->getShapeInstance();
	}
	shape = model->getShape();
	const TSShape::Detail& det = shape->details[model->getCurrentDetail()];
	S32 od = det.objectDetailNum;
   const TSShape::Object &obj = shape->objects[NV.ObjIndex];
	TSMesh* mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
	if(!mesh)
		return false;;

	TSSkinMesh* sMesh = dynamic_cast<TSSkinMesh*>(mesh);
	//TSMesh::TSMeshVertexArray vertexList = shape->meshes[meshIndex]->mVertexData;
	S32 numVerts;
	numVerts = mesh->mVertexData.size();
	if (sMesh)
      numVerts = sMesh->mVertexData.size();//[NV.ObjIndex]

	if(!numVerts)
		return false;

	vertexCount = numVerts;

	// Apparently, creating a TSMesh::TSMeshVertexArray vertexList instead of 
	//  - accessing the vertexdata directly can cause the program to crash on shutdown and startup.
	//  - It calls it's deconstructor when it goes out of scope. Seems like a bug.
	const Point3F *vertPos;
	const Point3F *vertNorm;
	if (sMesh)
	{
		vertPos = new const Point3F(sMesh->mVertexData[NV.VertIndex].vert());
		vertNorm = new const Point3F(sMesh->mVertexData[NV.VertIndex].normal());
	}
	else
	{
		vertPos = new const Point3F(mesh->mVertexData[NV.VertIndex].vert());
		vertNorm = new const Point3F(mesh->mVertexData[NV.VertIndex].normal());
	}

	// Get the transform of the object to get the transform matrix.
	//  - If it is a TSStatic we need to access the rootnode aswell
	//  - Which contains the rotation information.
	MatrixF trans;
	MatrixF nodetrans;
	MatrixF mat;
	if(SS)
	{
		trans = SS->getTransform();
	}
	else
	{
		trans = (dynamic_cast<TSStatic*>(SB))->getTransform();
		nodetrans = dynamic_cast<TSStatic*>(SB)->getShapeInstance()->mNodeTransforms[0];
		mat.mul(trans, nodetrans);
	}

	Point3F* p = new Point3F();
	if(SS)
	{
		trans.mulV(*vertPos,p);
		pNew->pos = SS->getPosition() + *p + (*vertNorm * ejectionOffset);
	}
	else{
		mat.mulV((*vertPos * dynamic_cast<TSStatic*>(SB)->getScale()),p);
		pNew->pos = dynamic_cast<TSStatic*>(SB)->getPosition() + *p + (*vertNorm * ejectionOffset);
	}
	// Set the relative position for later use.
	pNew->relPos = *p +(*vertNorm * ejectionOffset);
	// Velocity is based on the normal of the vertex
	pNew->vel = *vertNorm * initialVel;
	pNew->orientDir = *vertNorm;

	// Clean up
	delete(*p);
	delete(*vertPos);
	// Exit the loop
	return true;
}

bool NodeMeshEmitter::getPointOnFace(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew)
{
   return Parent::getPointOnFace(SB, SS, TS, pNew);
}

int compareNodeVertex (const void * a, const void * b)
{
   return ( (*(NodeMeshEmitter::NodeVertex*)a).VertIndex - (*(NodeMeshEmitter::NodeVertex*)b).VertIndex );
}

inline bool containsNodeVertex(Vector<NodeMeshEmitter::NodeVertex> &src, U32 &idx, S32 &size)
{
   S32 first = 0;
   S32 last = size - 1;
   S32 middle = (first+last) / 2;
   S32 search = idx;
   // Binary search for vert1
   while( first <= last )
   {
      if ( src[middle].VertIndex < search )
         first = middle + 1;    
      else if (src[middle].VertIndex == search ) 
      {
         return true;
      }
      else
         last = middle - 1;
 
      middle = (first + last)/2;
   }
   return false;
}

void NodeMeshEmitter::loadFaces(SimObject *SB, ShapeBase *SS, TSStatic* TS)
{
	PROFILE_SCOPE(NodeMeshEmitLoadFaces);
	emitfaces.clear();
	vertexCount = 0;
	NodeVertices.clear();
	if(SB && (SS || TS))
	{
		const TSShapeInstance* model;
		TSShape* shape;
		if(SS)
			model = SS->getShapeInstance();
		else{
			model = TS->getShapeInstance();
		}
		shape = model->getShape();
		const TSShape::Detail& det = shape->details[model->getCurrentDetail()];
		S32 od = det.objectDetailNum;
		S32 start = shape->subShapeFirstObject[det.subShapeNum];
		S32 end   = start + shape->subShapeNumObjects[det.subShapeNum];

		S32 nodeStart = shape->subShapeFirstNode[det.subShapeNum];
		S32 nodeEnd = nodeStart + shape->subShapeNumNodes[det.subShapeNum];
      S32 node = shape->findNode(NodeName);

		if(node < 0)
			return;

		for (U32 meshIndex = start; meshIndex < end; meshIndex++)
		{
			const TSShape::Object &obj = shape->objects[meshIndex];
			TSMesh* mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
			if(!mesh)
				continue;

			TSSkinMesh* sMesh = dynamic_cast<TSSkinMesh*>(mesh);
         
         S32 rnode = -1;
         for(int nii = 0; nii < sMesh->batchData.nodeIndex.size(); nii++)
            if(sMesh->batchData.nodeIndex[nii] == node)
               rnode = nii;
         if(rnode < 0)
            continue;
			S32 * curVtx = sMesh->vertexIndex.begin();
			S32 * curBone = sMesh->boneIndex.begin();

			// Build the batch operations
         for(U32 i = 0; i < sMesh->vertexIndex.size(); i++)
			{
				const S32 vidx = *curVtx;
				++curVtx;

				const S32 midx = *curBone;
				++curBone;

            if(midx == rnode)
				{
					NodeVertex NV;
               NV.ObjIndex = meshIndex;
					NV.VertIndex = vidx;
					NodeVertices.push_back(NV);
				}
			}
		}
      std::qsort(NodeVertices.address(),NodeVertices.size(),sizeof(NodeVertex), compareNodeVertex);
      std::vector<IPSCore::face> triangles;
		bool skinmesh = false;
		for(S32 meshIndex = 0; meshIndex < model->getShape()->meshes.size(); meshIndex++)
		{
			TSSkinMesh* sMesh = dynamic_cast<TSSkinMesh*>(model->getShape()->meshes[meshIndex]);
			if(sMesh)
			{
				if(sMesh->mVertexData.size()){
					skinmesh = true;
					break;
				}
			}
		}
      for (S32 meshIndex = start; meshIndex < end; meshIndex++)  
		{
			TSSkinMesh* sMesh;

			const TSShape::Object &obj = shape->objects[meshIndex];
			TSMesh* Mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;

			sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
			if(!Mesh)
				continue;

			S32 numVerts = Mesh->mVertexData.size();
			if(!numVerts)
				continue;
			vertexCount += numVerts;

			S32 numPrims = Mesh->primitives.size();
			if(!numPrims)
				continue;

			S32 numIndices = Mesh->indices.size();
			if(!numIndices)
				continue;
			for( U32 primIndex = 0; primIndex < numPrims; primIndex++ )
			{
				U32 start = Mesh->primitives[primIndex].start;

				U32 numElements = Mesh->primitives[primIndex].numElements;

				if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles) 
				{

					TSMesh::__TSMeshVertexBase v1, v2, v3;
					Point3F p1, p2, p3;

               S32 NVSize = NodeVertices.size();

					for (U32 triIndex = 0; triIndex < numElements; triIndex+=3)
					{
						U32 triStart = start + triIndex;
                  bool vert1 = false, vert2 = false, vert3 = false;
                  vert1 = containsNodeVertex(NodeVertices, Mesh->indices[triStart], NVSize);
                  vert2 = containsNodeVertex(NodeVertices, Mesh->indices[triStart+1], NVSize);
                  vert3 = containsNodeVertex(NodeVertices, Mesh->indices[triStart+2], NVSize);

                  if(!(vert1 && vert2 && vert3))
                     continue;
						v1 = Mesh->mVertexData[Mesh->indices[triStart]];
						v2 = Mesh->mVertexData[Mesh->indices[triStart + 1]];
						v3 = Mesh->mVertexData[Mesh->indices[triStart + 2]];

						Point3F scale;
						if(SS)
							scale = SS->getScale();
						else
							scale = (dynamic_cast<TSStatic*>(SB))->getScale();
						p1 = v1.vert() * scale;
						p2 = v2.vert() * scale;
						p3 = v3.vert() * scale;

						Point3F veca = p1 - p2;
						Point3F vecb = p2 - p3;
						Point3F vecc = p3 - p1;

						IPSCore::face tris;
						tris.meshIndex = meshIndex;
						tris.skinMesh = skinmesh;
						tris.triStart = triStart;
						tris.area = IPSCore::HeronsF(veca, vecb, vecc);
						triangles.push_back(tris);
					}
				}
				else
				{
					if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Fan) 
						Con::warnf("Was a fan DrawPrimitive not TrisDrawPrimitive");
					else if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Strip) 
						Con::warnf("Was a Strip DrawPrimitive not TrisDrawPrimitive");
					else
						Con::warnf("Couldn't determine primitive type");
				}
			}
		}

		F32 averageArea = 0;
		for(S32 index = 0; index < triangles.size(); index++)
		{
			averageArea += triangles[index].area;
		}
		averageArea = averageArea / triangles.size();
      // Reserve some space to prevent numerous
      //  reallocs, which takes a lot of time.
      // Note: Each face is 224 bits, memory
      //  usage can quickly raise to the skies.
      emitfaces.reserve(triangles.size() * 2);
		for(S32 index = 0; index < triangles.size(); index++)
		{
			float n = triangles[index].area / averageArea;
			float t;
			t = n - floor(n);
			if (t >= 0.5)    
				n = ceil(n);
			else 
				n = floor(n);
			for(int i = -1; i < n; i++)
			{
				emitfaces.push_back(triangles[index]);
			}
		}
	}
}

U32 NodeMeshEmitter::packUpdate(NetConnection* conn, U32 mask, BitStream *stream)
{
	U32 retMask = Parent::packUpdate(conn, mask, stream);

	if(stream->writeFlag(mask & NodeNameMask))
		stream->writeString(NodeName);

	return retMask;
}

void NodeMeshEmitter::unpackUpdate(NetConnection* conn, BitStream *stream)
{
	Parent::unpackUpdate(conn, stream);

	if(stream->readFlag())
	{
		char buf[255];
		stream->readString(buf);
		NodeName = dStrdup(buf);
		Parent::loadFaces();
	}

}

//-----------------------------------------------------------------------------
// onStaticModified
// Added
//-----------------------------------------------------------------------------
void NodeMeshEmitter::onStaticModified(const char* slotName, const char*newValue)
{
	if(strcmp("NodeName", slotName) == 0){
		setMaskBits(NodeNameMask);
		Parent::loadFaces();
	}

	Parent::onStaticModified(slotName, newValue);
}