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
	PROFILE_SCOPE(nodeMeshEmitFace);

	F32 initialVel = ejectionVelocity;
	initialVel    += (velocityVariance * 2.0f * gRandGen.randF()) - velocityVariance;
	// Seed the random generator - this should maybe be swapped out in favor for the gRandGen.
	//  - Although they both work fine.
	srand(mainTime);
	// Throw out some trash results. Apparently the first 1-2 results fails to give proper random numbers.
	int trash = rand();
	trash = rand();
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
	S32 start = shape->subShapeFirstObject[det.subShapeNum];
	S32 end   = start + shape->subShapeNumObjects[det.subShapeNum];

	S32 nodeStart = shape->subShapeFirstNode[det.subShapeNum];
	S32 nodeEnd = nodeStart + shape->subShapeNumNodes[det.subShapeNum];
	S32 node = shape->findNode(NodeName);
	if(node < 0)
		return false;

#pragma region perTriangle
	S32 meshIndex;

	TSSkinMesh* sMesh = NULL;
	TSMesh* Mesh  = NULL;
	bool accepted = false;
	bool skinmesh = false;
	for(meshIndex = start; meshIndex < end; meshIndex++)
	{
		const TSShape::Object &obj = shape->objects[meshIndex];
		TSMesh* mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
		sMesh = dynamic_cast<TSSkinMesh*>(mesh);
		if(sMesh)
		{
			if(sMesh->mVertexData.size()){
				skinmesh = true;
				break;
			}
		}
	}
	// We don't want to run with partly skinmesh, partly static mesh.
	//  - So here we filter out skinmeshes from static meshes.
	while(!accepted)
	{
		accepted = false;
		// Pick a random mesh and test it.
		//  - This prevents the uneven emission from 
		//  - being as linear as it is with per vertex.
		meshIndex = (rand() % (end - start)) + start;
		const TSShape::Object &obj = shape->objects[meshIndex];
		TSMesh* Mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
		if(Mesh)
			accepted = true;
		if(skinmesh)
		{
			sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
			if(sMesh)
			{
				if(sMesh->mVertexData.size()){
					accepted = true;
					skinmesh = true;
				}
				else
					accepted = false;
			}
			else
				accepted = false;
		}
		if(!skinmesh && Mesh)
		{
			if(Mesh->verts.size() > 0)
				accepted = true;
			if(Mesh->verts.size() <= 0)
				accepted = false;
			if(Mesh->mVertexData.size() > 0)
				accepted = true;
			if(Mesh->mVertexData.size() <= 0)
				accepted = false;
		}
	}
#pragma region Test

	if(true)
	{
		NodeVertex NV = NodeVertices[gRandGen.randI() % NodeVertices.size()];
      const TSShape::Object &obj = shape->objects[NV.ObjIndex];
		TSMesh* mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
		if(!mesh)
			return false;;

		TSSkinMesh* sMesh = dynamic_cast<TSSkinMesh*>(mesh);
		S32 * curVtx = sMesh->vertexIndex.begin();
		S32 * curBone = sMesh->boneIndex.begin();

		if(NodeVertices.size() <= 0)
			return false;

		//TSMesh::TSMeshVertexArray vertexList = shape->meshes[meshIndex]->mVertexData;
		S32 numVerts;
		numVerts = mesh->mVertexData.size();
		if (sMesh)
			numVerts = sMesh[NV.ObjIndex].mVertexData.size();

		if(!numVerts)
			return false;

		vertexCount = numVerts;

		const Point3F *vertPos;
		const Point3F *vertNorm;
		vertPos = new const Point3F(sMesh->mVertexData[NV.VertIndex].vert());
		vertNorm = new const Point3F(sMesh->mVertexData[NV.VertIndex].normal());

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
		return true;
	}
#pragma endregion

	if(!evenEmission)
	{
		PROFILE_SCOPE(meshEmitOdd);
#pragma region skinMesh
		if(skinmesh)
		{
			Vector<Point3F> normList = shape->meshes[meshIndex]->norms;
			if (sMesh)
			{
				S32 numVerts = sMesh->mVertexData.size();
				if(numVerts)
				{
					S32 numPrims = sMesh->primitives.size();
					if(numPrims)
					{
						S32 numIndices = sMesh->indices.size();
						if(numIndices)
						{
							// Get a random primitive
							S32 primIndex = rand() % numPrims;
							S32 start = sMesh->primitives[primIndex].start;
							S16 numElements = sMesh->primitives[primIndex].numElements;

							// Define some variables we will use later
							TSMesh::__TSMeshVertexBase v1, v2, v3;
							Point3F vp1;

							// Test if the primitive is a triangle. Which it should be.
							//  - Theres no handler for other primitives than triangles,
							//  - if such is needed email me at LukasPJ@FuzzyVoidStudio.com
							if ( (shape->meshes[meshIndex]->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
							{
								// Get a random triangle
								U32 triStart = (rand() % (numElements/3));
								// This is not really necessary due to the way we handle the
								//  - triangles, but it is an useful snippet for modifications!
								//  - due to some rendering thing, every other triangle is 
								//  - counter clock wise. Read about it in the official DX9 docs.
								U8 indiceBool = (triStart * 3) % 2;
								if(indiceBool == 0)
								{
									v1 = sMesh->mVertexData[sMesh->indices[start + (triStart*3)]];
									v2 = sMesh->mVertexData[sMesh->indices[start + (triStart*3) + 1]];
									v3 = sMesh->mVertexData[sMesh->indices[start + (triStart*3) + 2]];
								}
								else
								{
									v3 = sMesh->mVertexData[sMesh->indices[start + (triStart*3)]];
									v2 = sMesh->mVertexData[sMesh->indices[start + (triStart*3) + 1]];
									v1 = sMesh->mVertexData[sMesh->indices[start + (triStart*3) + 2]];
									/*
									v1
									v3
									v2
									*/
								}
								// Create 2 vectors from the 3 points that make up the triangle
								Point3F p1 = v1.vert();
								Point3F p2 = v2.vert();
								Point3F p3 = v3.vert();
								Point3F vec1;
								Point3F vec2;
								vec1 = p2-p1;
								vec2 = p3-p2;
								// Get 2 random coefficients
								F32 K1 = rand() % 1000 + 1;
								F32 K2 = rand() % 1000 + 1;
								Point3F planeVec;
								// If the point is outside of the triangle, mirror it in so it fits
								//  - into the triangle. This is for a perfectly even result on a 
								//  - per face basis.
								if(K2 <= K1)
									planeVec = p1 + (vec1 * (K1 / 1000)) + (vec2 * (K2 / 1000));
								else
									planeVec = p1 + (vec1 * (1-(K1 / 1000))) + (vec2 * (1-(K2 / 1000)));

								// Add up the normals of the three vertices and normalize them to get
								//  - the correct normal of the plane.
								Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
								normalV->normalize();

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
									trans = TS->getTransform();
									nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
									mat.mul(trans, nodetrans);
								}

								Point3F* p = new Point3F();

								if(SS)
								{
									trans.mulV(planeVec,p);
									pNew->pos = SS->getPosition() + *p + (*normalV * ejectionOffset);
								}
								else{
									mat.mulV((*planeVec * TS->getScale()),p);
									pNew->pos = TS->getPosition() + *p + (*normalV * ejectionOffset);
								}
								delete(*p);
								delete(*normalV);
								return true;
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
				}
			}
		}
#pragma endregion
#pragma region staticmesh
		// Same procedure as above
		if(!skinmesh)
		{
			if(Mesh)
			{
				S32 numVerts = Mesh->mVertexData.size();
				if(numVerts)
				{
					S32 numPrims = Mesh->primitives.size();
					if(numPrims)
					{
						S32 numIndices = Mesh->indices.size();
						if(numIndices)
						{
							S32 primIndex = rand() % numPrims;
							S32 start = Mesh->primitives[primIndex].start;
							S16 numElements = Mesh->primitives[primIndex].numElements;

							TSMesh::__TSMeshVertexBase v1, v2, v3;
							Point3F vp1;

							if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
							{
								U32 triStart = (rand() % (numElements/3));
								U8 indiceBool = (triStart * 3) % 2;
								if(indiceBool == 0)
								{
									v1 = Mesh->mVertexData[Mesh->indices[start + (triStart*3)]];
									v2 = Mesh->mVertexData[Mesh->indices[start + (triStart*3) + 1]];
									v3 = Mesh->mVertexData[Mesh->indices[start + (triStart*3) + 2]];
								}
								else
								{
									v3 = Mesh->mVertexData[Mesh->indices[start + (triStart*3)]];
									v2 = Mesh->mVertexData[Mesh->indices[start + (triStart*3) + 1]];
									v1 = Mesh->mVertexData[Mesh->indices[start + (triStart*3) + 2]];
									/*
									v1
									v3
									v2
									*/
								}
								Point3F p1 = v1.vert();
								Point3F p2 = v2.vert();
								Point3F p3 = v3.vert();
								Point3F vec1;
								Point3F vec2;
								vec1 = p2-p1;
								vec2 = p3-p2;
								F32 K1 = rand() % 1000 + 1;
								F32 K2 = rand() % 1000 + 1;
								Point3F planeVec;
								if(K2 <= K1)
									planeVec = p1 + (vec1 * (K1 / 1000)) + (vec2 * (K2 / 1000));
								else
									planeVec = p1 + (vec1 * (1-(K1 / 1000))) + (vec2 * (1-(K2 / 1000)));

								// Construct a vector from the 3 results
								const Point3F *vertPos = new const Point3F(planeVec);

								Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
								normalV->normalize();

								MatrixF trans;
								MatrixF nodetrans;
								MatrixF mat;
								if(SS)
								{
									trans = SS->getTransform();
								}
								else
								{
									trans = TS->getTransform();
									nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
									mat.mul(trans, nodetrans);
								}
								// Rotate our point by the rotation matrix
								Point3F* p = new Point3F();

								if(SS)
								{
									trans.mulV(*vertPos,p);
									pNew->pos = SS->getPosition() + *p + (*normalV * ejectionOffset);
								}
								else{
									mat.mulV((*vertPos * TS->getScale()),p);
									pNew->pos = TS->getPosition() + *p + (*normalV * ejectionOffset);
								}
								return true;
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
				}
			}
		}
#pragma endregion
	}
	if(evenEmission)
	{
		if(emitfaces.size())
		{
			PROFILE_SCOPE(meshEmitEven);
			// Get a random face from our emitfaces vector.
			//  - then follow basically the same procedure as above.
			//  - Just slightly simplified
			S32 faceIndex = rand() % emitfaces.size();
			IPSCore::face tris = emitfaces[faceIndex];
			const TSShape::Object &obj = shape->objects[tris.meshIndex];
			TSMesh* Mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
			sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
			TSMesh::__TSMeshVertexBase v1, v2, v3;
			Point3F p1, p2, p3;
			if(tris.skinMesh)
			{
				sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
				U8 indiceBool = tris.triStart % 2;
				if(indiceBool == 0)
				{
					v1 = sMesh->mVertexData[sMesh->indices[tris.triStart]];
					v2 = sMesh->mVertexData[sMesh->indices[tris.triStart + 1]];
					v3 = sMesh->mVertexData[sMesh->indices[tris.triStart + 2]];
				}
				else
				{
					v3 = sMesh->mVertexData[sMesh->indices[tris.triStart]];
					v2 = sMesh->mVertexData[sMesh->indices[tris.triStart + 1]];
					v1 = sMesh->mVertexData[sMesh->indices[tris.triStart + 2]];
				}
				p1 = v1.vert();
				p2 = v2.vert();
				p3 = v3.vert();
			}
			else{
				U8 indiceBool = tris.triStart % 2;
				if(indiceBool == 0)
				{
					v1 = Mesh->mVertexData[Mesh->indices[tris.triStart]];
					v2 = Mesh->mVertexData[Mesh->indices[tris.triStart + 1]];
					v3 = Mesh->mVertexData[Mesh->indices[tris.triStart + 2]];
				}
				else
				{
					v3 = Mesh->mVertexData[Mesh->indices[tris.triStart]];
					v2 = Mesh->mVertexData[Mesh->indices[tris.triStart + 1]];
					v1 = Mesh->mVertexData[Mesh->indices[tris.triStart + 2]];
				}
				p1 = v1.vert();
				p2 = v2.vert();
				p3 = v3.vert();
			}
			Point3F vec1;
			Point3F vec2;
			vec1 = p2-p1;
			vec2 = p3-p2;
			F32 K1 = rand() % 1000 + 1;
			F32 K2 = rand() % 1000 + 1;
			Point3F planeVec;
			if(K2 <= K1)
				planeVec = p1 + (vec1 * (K1 / 1000)) + (vec2 * (K2 / 1000));
			else
				planeVec = p1 + (vec1 * (1-(K1 / 1000))) + (vec2 * (1-(K2 / 1000)));
			// Construct a vector from the 3 results
			const Point3F *vertPos = new const Point3F(planeVec);

			Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
			normalV->normalize();

			MatrixF trans;
			MatrixF nodetrans;
			MatrixF mat;
			if(SS)
			{
				trans = SS->getTransform();
			}
			else
			{
				trans = TS->getTransform();
				nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
				mat.mul(trans, nodetrans);
			}
			// Rotate our point by the rotation matrix
			Point3F* p = new Point3F();

			if(SS)
			{
				trans.mulV(*vertPos,p);
				pNew->pos = SS->getPosition() + *p + (*normalV * ejectionOffset);
			}
			else{
				mat.mulV((*vertPos * TS->getScale()),p);
				mat.mulV(*normalV);
				pNew->pos = TS->getPosition() + *p + (*normalV * ejectionOffset);
			}
			pNew->relPos = *p +(*normalV * ejectionOffset);
			pNew->vel = *normalV * initialVel;
			pNew->orientDir = *normalV;
			delete(*p);
			delete(*vertPos);
			delete(*normalV);
			return true;
		}
	}
	return false;
#pragma endregion
}

void NodeMeshEmitter::loadFaces(SimObject *SB, ShapeBase *SS, TSStatic* TS)
{
	Parent::loadFaces(SB, SS, TS);

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