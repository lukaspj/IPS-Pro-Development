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

#include "radiusMeshEmitter.h"
#include "core/stream/bitStream.h"

#include "IPSDebugRenderHelper.h"

#include "ts\tsShapeInstance.h"
#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "T3D\staticShape.h"
#include "ts\TSMeshIntrinsics.h"

IMPLEMENT_CO_DATABLOCK_V1(RadiusMeshEmitterData);
IMPLEMENT_CO_NETOBJECT_V1(RadiusMeshEmitter);

RadiusMeshEmitter::RadiusMeshEmitter()
{
	MeshEmitter::MeshEmitter();
	// RadiusMesh variables
	evenEmission = true;
	emitOnFaces = true;
	emitMesh = "";
	vertexCount = 0;
	center = Point3F(0,0,0);
	radius = 3;
	active = true;

	// Debug variables
	renderIntersections = false;
	renderEmittingFaces = false;
	shadeByCoverage = false;
	renderSphere = false;
}

void RadiusMeshEmitter::initPersistFields()
{
	addGroup( "RadiusMeshEmitter" );

	addField( "center", TYPEID< Point3F >(), Offset(center, RadiusMeshEmitter),
		"The center of the spherical boundary that limits emission." );

	addField( "radius", TYPEID< F32 >(), Offset(radius, RadiusMeshEmitter),
		"The radius of the spherical boundary that limits emission." );

	endGroup( "RadiusMeshEmitter" );

	addGroup( "Debug" );

	addField( "renderIntersections", TYPEID< bool >(), Offset(renderIntersections, RadiusMeshEmitter),
		"Render arrows from the center of the sphere to where the sphere "
										"intersects with an edge on the mesh." );

	addField( "renderEmittingFaces", TYPEID< bool >(), Offset(renderEmittingFaces, RadiusMeshEmitter),
		"Render the geometry inside the sphere." );

	addField( "renderSphere", TYPEID< bool >(), Offset(renderSphere, RadiusMeshEmitter),
		"Render the sphere." );

	addField( "shadeByCoverage", TYPEID< bool >(), Offset(shadeByCoverage, RadiusMeshEmitter),
		"Deprecated." );

	endGroup( "Debug" );
	Parent::initPersistFields();
}

bool RadiusMeshEmitter::getPointOnFace(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew)
{
	F32 initialVel = ejectionVelocity;
	initialVel    += (velocityVariance * 2.0f * gRandGen.randF()) - velocityVariance;
	const TSShape* shape;
	const TSShapeInstance* model;
	if(SS){
		model = SS->getShapeInstance();
		shape = SS->getShape();
	}
	else{
		model = TS->getShapeInstance();
		shape = TS->getShape();
	}
	TSSkinMesh* sMesh = NULL;
	const TSShape::Detail& det = shape->details[0];
	S32 od = det.objectDetailNum;
	if(evenEmission)
	{
		if(emitfaces.size())
		{
			PROFILE_SCOPE(RadiusMeshEmitEven);
			// Get a random face from our emitfaces vector.
			//  - then follow basically the same procedure as above.
			//  - Just slightly simplified
			S32 faceIndex = gRandGen.randI() % emitfaces.size();
			IPSCore::face tris = emitfaces[faceIndex];
			const TSShape::Object &obj = shape->objects[tris.meshIndex];
			TSMesh* Mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
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
					v1 = sMesh->mVertexData[sMesh->indices[tris.triStart]];
					v2 = sMesh->mVertexData[sMesh->indices[tris.triStart + 1]];
					v3 = sMesh->mVertexData[sMesh->indices[tris.triStart + 2]];
					/*v3 = sMesh->mVertexData[sMesh->indices[tris.triStart]];
					v2 = sMesh->mVertexData[sMesh->indices[tris.triStart + 1]];
					v1 = sMesh->mVertexData[sMesh->indices[tris.triStart + 2]];*/
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
					v1 = Mesh->mVertexData[Mesh->indices[tris.triStart]];
					v2 = Mesh->mVertexData[Mesh->indices[tris.triStart + 1]];
					v3 = Mesh->mVertexData[Mesh->indices[tris.triStart + 2]];
					/*
					v3 = RadiusMesh->mVertexData[RadiusMesh->indices[tris.triStart]];
					v2 = RadiusMesh->mVertexData[RadiusMesh->indices[tris.triStart + 1]];
					v1 = RadiusMesh->mVertexData[RadiusMesh->indices[tris.triStart + 2]];*/
				}
				p1 = v1.vert();
				p2 = v2.vert();
				p3 = v3.vert();
			}
			Point3F scale;
			if(SS)
				scale = Point3F(1);
			else
				scale = TS->getScale();
			Point3F planeVec;
			Point3F pnewPosition;
			Point3F pnewrelPos;
			Point3F pnewVel;
			Point3F pneworientDir;

			if(tris.vertsInside == 3){
				IPSEmitterHelper::uniformPointInTriangle(p1,p2,p3,&planeVec);

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
					pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
				}
				else{
					mat.mulV((*vertPos * TS->getScale()),p);
					mat.mulV(*normalV);
					pnewPosition = TS->getPosition() + *p + (*normalV * ejectionOffset);
				}
				pnewrelPos = *p +(*normalV * ejectionOffset);
				pnewVel = *normalV * initialVel;
				pneworientDir = *normalV;
				delete(*p);
				delete(*vertPos);
				delete(*normalV);
			}
			if(tris.vertsInside == 2)
			{
				IPSEmitterHelper::uniformPointInTrapezoid(tris, p1, p2, p3, &planeVec);


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
					pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
				}
				else{
					mat.mulV((*vertPos * TS->getScale()),p);
					mat.mulV(*normalV);
					pnewPosition = TS->getPosition() + *p + (*normalV * ejectionOffset);
				}
				pnewrelPos = *p +(*normalV * ejectionOffset);
				pnewVel = *normalV * initialVel;
				pneworientDir = *normalV;
				delete(*p);
				delete(*vertPos);
				delete(*normalV);
			}
			if(tris.vertsInside == 1)
			{
				MatrixF trans;
				MatrixF nodetrans;
				MatrixF mat;
				if(SS)
				{
					trans = SS->getTransform();
					trans.mulV(p1);
					trans.mulV(p2);
					trans.mulV(p3);
				}
				else
				{
					trans = TS->getTransform();
					nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
					mat.mul(trans, nodetrans);
					mat.mulV(p1);
					mat.mulV(p2);
					mat.mulV(p3);

					p1 *= TS->getScale();
					p2 *= TS->getScale();
					p3 *= TS->getScale();
				}

				F32 coeff = gRandGen.randF();
				if(coeff < tris.arcArea1Coeff){
					PlaneF plane = PlaneF(p1,p2,p3);
					Point3F PlaneP = plane.project(center);
					Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
					Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
					Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
					Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
					Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
					Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));

					Point3F chordStart, chordEnd, mirrorP;
					if(tris.v1Inside && !tris.v2Inside && !tris.v3Inside)
					{
						chordStart = sideA1;
						chordEnd = sideC1;
						mirrorP = p1;
					}
					if(!tris.v1Inside && tris.v2Inside && !tris.v3Inside)
					{
						chordStart = sideB1;
						chordEnd = sideA1;
						mirrorP = p2;
					}
					if(!tris.v1Inside && !tris.v2Inside && tris.v3Inside)
					{
						chordStart = sideC1;
						chordEnd = sideB1;
						mirrorP = p3;
					}
					if(!IPSEmitterHelper::uniformPointInArc(plane, chordStart, chordEnd, center, radius, mirrorP, &planeVec))
						return false;
				}
				else
					IPSEmitterHelper::uniformPointInTriangle(tris, p1, p2, p3, &planeVec);

				// Construct a vector from the 3 results
				const Point3F *vertPos = new const Point3F(planeVec);

				Point3F* normalV = new Point3F((v1.normal()+v2.normal()+v3.normal())/3);
				normalV->normalize();

				// Rotate our point by the rotation matrix

				if(SS)
				{
					pnewPosition = SS->getPosition() + *vertPos + (*normalV * ejectionOffset);
				}
				else{
					mat.mulV(*normalV);
					pnewPosition = TS->getPosition() + *vertPos + (*normalV * ejectionOffset);
				}
				pnewrelPos = *vertPos +(*normalV * ejectionOffset);
				PlaneF plane = PlaneF(p1,p2,p3);
				pnewVel = *normalV * initialVel;
				pneworientDir = *normalV;
				pneworientDir = IPSCore::perpendicularVector(*normalV,plane);
				delete(*vertPos);
				delete(*normalV);
			}
			if(tris.vertsInside == 0)
			{
				Point3F pos;
				Point3F planeVec;
				if(SS){
					MatrixF trans;
					trans = SS->getTransform();
					trans.mulP(v1.vert(),&p1);
					trans.mulP(v2.vert(),&p2);
					trans.mulP(v3.vert(),&p3);
					pos = SS->getPosition();
				}
				if(TS){
					pos = TS->getPosition();
					MatrixF trans, nodetrans, mat;
					trans = TS->getTransform();
					nodetrans = TS->getShapeInstance()->mNodeTransforms[0];

					mat.mul(trans,nodetrans);
					mat.mulV(v1.vert(),&p1);
					mat.mulV(v2.vert(),&p2);
					mat.mulV(v3.vert(),&p3);
					p1 *= TS->getScale();
					p2 *= TS->getScale();
					p3 *= TS->getScale();
				}

				//Implement inbound circle here
				if(tris.inboundSphere)
				{
					if(IPSEmitterHelper::uniformPointInInboundSphere(tris, p1, p2, p3, center, pos, radius, &planeVec))
					{
						pnewPosition = pos+planeVec;
						pnewrelPos = planeVec;
						pnewVel.set(0);
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
							pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
						}
						else{
							mat.mulV(*normalV);
							pnewPosition = TS->getPosition() + *vertPos + (*normalV * ejectionOffset);
						}
						pnewrelPos = *p +(*normalV * ejectionOffset);
						pnewVel = *normalV * initialVel;
						pneworientDir = *normalV;
						delete(*p);
						delete(*vertPos);
						delete(*normalV);
					}
				}

				if(((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count != 2) || 
					(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
					(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)) &&
					IPSEmitterHelper::uniformPointInArc(tris,p1,p2,p3,center,radius,&planeVec) && !tris.inboundSphere)
				{
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
						pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
					}
					else{
						//mat.mulV((*vertPos * TS->getScale()),p);
						//mat.mulV(*vertPos,p);
						mat.mulV(*normalV);
						pnewPosition = TS->getPosition() + *vertPos + (*normalV * ejectionOffset);
					}
					pnewrelPos = *p +(*normalV * ejectionOffset);
					pnewVel = *normalV * initialVel;
					pneworientDir = *normalV;
					delete(*p);
					delete(*vertPos);
					delete(*normalV);
				}
				if(((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
					(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2) || 
					(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)) && !tris.inboundSphere)
				{
					F32 coeff = gRandGen.randF();
					if(coeff < tris.arcArea1Coeff)
						IPSEmitterHelper::uniformPointInArc(tris,p1,p2,p3,center,radius,false,&planeVec);
					else if(coeff < tris.arcArea1Coeff + tris.arcArea2Coeff)
						IPSEmitterHelper::uniformPointInArc(tris,p1,p2,p3,center,radius,true,&planeVec);
					else
						IPSEmitterHelper::uniformPointInTrapezoid(tris, p1, p2, p3, &planeVec);

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
						pnewPosition = SS->getPosition() + *p + (*normalV * ejectionOffset);
					}
					else{
						//mat.mulV((*vertPos * TS->getScale()),p);
						//mat.mulV(*vertPos,p);
						mat.mulV(*normalV);
						pnewPosition = TS->getPosition() + *vertPos + (*normalV * ejectionOffset);
					}
					pnewrelPos = *p +(*normalV * ejectionOffset);
					pnewVel = *normalV * initialVel;
					pneworientDir = *normalV;
					delete(*p);
					delete(*vertPos);
					delete(*normalV);
				}
			}
         /*
			n_parts++;
			if (n_parts > n_part_capacity || n_parts > mDataBlock->partListInitSize)
			{
				// In an emergency we allocate additional particles in blocks of 16.
				// This should happen rarely.
				Particle* store_block = new Particle[16];
				part_store.push_back(store_block);
				n_part_capacity += 16;
				for (S32 i = 0; i < 16; i++)
				{
					store_block[i].next = part_freelist;
					part_freelist = &store_block[i];
				}
				mDataBlock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
			}

			Particle* pNew = part_freelist;
			part_freelist = pNew->next;
			pNew->next = part_list_head.next;
			part_list_head.next = pNew;

			pNew->pos = pnewPosition;
			pNew->relPos = pnewrelPos;
			pNew->vel = pnewVel;
			pNew->orientDir = pneworientDir;
			pNew->acc.set(0, 0, 0);
			pNew->currentAge = 0;

			// Choose a new particle datablack randomly from the list
			U32 dBlockIndex = gRandGen.randI() % mDataBlock->particleDataBlocks.size();
			mDataBlock->particleDataBlocks[dBlockIndex]->initializeParticle(pNew, pNew->vel);
			updateKeyData( pNew );
         */
			return true;
		}
	}
	return false;
}

bool RadiusMeshEmitter::getPointOnVertex(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew)
{
	return false;
}

U32 RadiusMeshEmitter::packUpdate(NetConnection* conn, U32 mask, BitStream *stream)
{
   U32 retMask = Parent::packUpdate(conn, mask | SkipReloadMask, stream);

   if( stream->writeFlag( mask & GeometryMask ) )
	{
		stream->writeVector(center,1000.0f,24,24);
		stream->writeFloat(radius/1000,18);
	}

   if( stream->writeFlag( mask & MeshEmitterMask ) )
   {
      stream->writeFlag(active);
		stream->writeFlag(renderIntersections);
		stream->writeFlag(renderEmittingFaces);
		stream->writeFlag(renderSphere);
		stream->writeFlag(shadeByCoverage);
		stream->writeFlag(mDeleteWhenEmpty);
   }

	return retMask;
}

void RadiusMeshEmitter::unpackUpdate(NetConnection* conn, BitStream *stream)
{
	Parent::unpackUpdate(conn, stream);

   // GeometryMask
   if( stream-> readFlag() )
   {
		stream->readVector(&center,1000.0f,24,24);
		radius = stream->readFloat(18) * 1000;
		loadFaces();
   }

	// RadiusMeshemitter mask
	if ( stream->readFlag() )
	{
		active = stream->readFlag();
		renderIntersections = stream->readFlag();
		renderEmittingFaces = stream->readFlag();
		renderSphere = stream->readFlag();
		shadeByCoverage = stream->readFlag();
		mDeleteWhenEmpty = stream->readFlag();
	}
}

//-----------------------------------------------------------------------------
// onStaticModified
// Added
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::onStaticModified(const char* slotName, const char*newValue)
{
	Parent::onStaticModified(slotName, newValue);
   if( strcmp(slotName, "center") == 0 ||
		strcmp(slotName, "radius") == 0)
      setMaskBits(GeometryMask);
	// Was it an sa_value that was edited? Then set the emitterEdited bit.
	if( strcmp(slotName, "renderIntersections") == 0 ||
		 strcmp(slotName, "renderEmittingFaces") == 0 ||
		 strcmp(slotName, "shadeByCoverage") == 0 ||
		 strcmp(slotName, "renderSphere") == 0 ||
		 strcmp(slotName, "active") == 0)
      setMaskBits(MeshEmitterMask);
}

//-----------------------------------------------------------------------------
// loadFaces
//  - This function calculates the area of all the triangles in the RadiusMesh
//  - finds the average area, and adds the triangles one or more times
//  - to the emitfaces vector based on their area relative to the other faces
//  - not a perfect way to do it, but it works fine.
// Custom
//-----------------------------------------------------------------------------
void RadiusMeshEmitter::loadFaces()
{
	PROFILE_SCOPE(RadiusMeshEmitLoadFaces);
	emitfaces.clear();
	vertexCount = 0;
	SimObject* SB = dynamic_cast<SimObject*>(Sim::findObject(emitMesh));
	if(!SB)
		SB = dynamic_cast<SimObject*>(Sim::findObject(atoi(emitMesh)));
	ShapeBase* SS = NULL;
	TSStatic* TS = NULL;
	if(SB){
		SS = dynamic_cast<ShapeBase*>(SB);
		TS = dynamic_cast<TSStatic*>(SB);
	}
	// Make sure that we are dealing with some proper objects
	if(SB && (SS || TS)){
		TSShapeInstance* model;
		if(SS)
			model = SS->getShapeInstance();
		else{
			model = TS->getShapeInstance();
		}
		TSShape* shape = model->getShape();
		const TSShape::Detail& det = shape->details[0];
		S32 od = det.objectDetailNum;
		S32 start = shape->subShapeFirstObject[det.subShapeNum];
		S32 end   = start + shape->subShapeNumObjects[det.subShapeNum];
		std::vector<IPSCore::face> triangles;
		bool skinMesh = false;
		for(S32 meshIndex = start; meshIndex < end; meshIndex++)
		{
			const TSShape::Object &obj = shape->objects[meshIndex];
			TSMesh* Mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
			TSSkinMesh* sMesh = dynamic_cast<TSSkinMesh*>(Mesh);
			if(sMesh)
			{
				if(sMesh->mVertexData.size()){
					skinMesh = true;
					break;
				}
			}
		}
		for (S32 meshIndex = start; meshIndex < end; meshIndex++)  
		{
			const TSShape::Object &obj = shape->objects[meshIndex];
			TSMesh* Mesh = ( od < obj.numMeshes ) ? shape->meshes[obj.startMeshIndex + od] : NULL;
			TSSkinMesh* sMesh = dynamic_cast<TSSkinMesh*>(Mesh);

			U32 numVerts, numPrims, numIndices;
			if(skinMesh)
			{
				if(!sMesh)
					continue;
				numVerts = sMesh->mVertexData.size();
				numPrims = sMesh->primitives.size();
				numIndices = sMesh->indices.size();
			}
			else{
				if(!Mesh)
					continue;
				numVerts = Mesh->mVertexData.size();
				numPrims = Mesh->primitives.size();
				numIndices = Mesh->indices.size();
			}


			if(!numVerts)
				continue;
			vertexCount += numVerts;

			if(!numPrims)
				continue;

			if(!numIndices)
				continue;

			for( S32 primIndex = 0; primIndex < numPrims; primIndex++ )
			{
				U32 start, numElements;
				if(skinMesh){
					start = sMesh->primitives[primIndex].start;
					numElements = sMesh->primitives[primIndex].numElements;
				}
				else{
					start = Mesh->primitives[primIndex].start;
					numElements = Mesh->primitives[primIndex].numElements;
				}
				if ( (Mesh->primitives[primIndex].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles) 
				{
					TSMesh::__TSMeshVertexBase v1, v2, v3;
					Point3F p1, p2, p3;
					for (U32 triIndex = 0; triIndex < numElements; triIndex+=3)
					{
						U32 triStart = start + triIndex;
						if(skinMesh){
							v1 = sMesh->mVertexData[sMesh->indices[triStart]];
							v2 = sMesh->mVertexData[sMesh->indices[triStart + 1]];
							v3 = sMesh->mVertexData[sMesh->indices[triStart + 2]];
						}
						else{
							v1 = Mesh->mVertexData[Mesh->indices[triStart]];
							v2 = Mesh->mVertexData[Mesh->indices[triStart + 1]];
							v3 = Mesh->mVertexData[Mesh->indices[triStart + 2]];
						}

						Point3F scale;
						if(SS)
							scale = SS->getScale();
						else
							scale = TS->getScale();
						Point3F worldCenter;
						MatrixF trans;
						MatrixF nodetrans;
						MatrixF mat;
						if(SS)
						{
							trans = SS->getTransform();
                     nodetrans = SS->getShapeInstance()->mNodeTransforms[0];
						}
						if(TS)
						{
							trans = TS->getTransform();
							nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
						}
						mat.mul(trans,nodetrans);
                  mat.mulV(center,&worldCenter);
						mat.mulV(v1.vert(),&p1);
						mat.mulV(v2.vert(),&p2);
						mat.mulV(v3.vert(),&p3);
						p1 *= trans.getScale();
						p2 *= trans.getScale();
						p3 *= trans.getScale();

						IPSCore::face tris;
						tris.meshIndex = meshIndex;
						tris.skinMesh = false;
						tris.inboundSphere = false;
						tris.triStart = triStart;
						tris.sideAIntersect = IPSCore::raySphereIntersection(center,radius,p1,p2);
						tris.sideBIntersect = IPSCore::raySphereIntersection(center,radius,p2,p3);
						tris.sideCIntersect = IPSCore::raySphereIntersection(center,radius,p3,p1);
						tris.v1Inside = false;
						tris.v2Inside = false;
						tris.v3Inside = false;
						tris.vertsInside = 0;
						if((p1-worldCenter).len() <= radius){
							tris.v1Inside = true;
							tris.vertsInside++;
						}
						if((p2-worldCenter).len() <= radius){
							tris.v2Inside = true;
							tris.vertsInside++;
						}
						if((p3-worldCenter).len() <= radius){
							tris.v3Inside = true;
							tris.vertsInside++;
						}
						// 3 Verts inside - The whole face is covered
						if(tris.vertsInside == 3){
							Point3F veca = p1 - p2;
							Point3F vecb = p2 - p3;
							Point3F vecc = p3 - p1;
							tris.area = IPSCore::HeronsF(veca, vecb, vecc);
							triangles.push_back(tris);
						}
						// 2 Verts inside - The face is intersected at 2 points
						//  and can therefore be described as a trapezoid made of
						//  2 triangles.
						// TODO: calculate the arc here aswell?
						if(tris.vertsInside == 2)
						{
							Point3F veca;
							Point3F vecb;
							Point3F vecc;
							if(tris.v1Inside && tris.v2Inside && !tris.v3Inside)
							{
								veca = p2-p1;
								vecb = tris.sideCIntersect.intersection1 - p2;
								vecc = p1 - tris.sideCIntersect.intersection1;
								tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

								veca = tris.sideBIntersect.intersection1 - p2;
								vecb = tris.sideCIntersect.intersection1 - tris.sideBIntersect.intersection1;
								vecc = p2 - tris.sideCIntersect.intersection1;
								tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);
								tris.area = tris.trapezoidArea1 + tris.trapezoidArea2;
								tris.trapezoidCoef = tris.trapezoidArea1 / tris.area;
							}
							else if(tris.v1Inside && !tris.v2Inside && tris.v3Inside)
							{
								veca = p3 - p1;
								vecb = tris.sideAIntersect.intersection1 - p3;
								vecc = p1 - tris.sideAIntersect.intersection1;
								tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

								veca = tris.sideAIntersect.intersection1 - p3;
								vecb = tris.sideBIntersect.intersection1 - tris.sideAIntersect.intersection1;
								vecc = p3 - tris.sideBIntersect.intersection1;
								tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);
								tris.area = tris.trapezoidArea1 + tris.trapezoidArea2;
								tris.trapezoidCoef = tris.trapezoidArea1 / tris.area;
							}
							else if(!tris.v1Inside && tris.v2Inside && tris.v3Inside)
							{
								veca = p3 - p2;
								vecb = tris.sideAIntersect.intersection1 - p3;
								vecc = p2 - tris.sideAIntersect.intersection1;
								tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

								veca = tris.sideCIntersect.intersection1 - p3;
								vecb = tris.sideAIntersect.intersection1 - tris.sideCIntersect.intersection1;
								vecc = p3 - tris.sideAIntersect.intersection1;
								tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);
								tris.area = tris.trapezoidArea1 + tris.trapezoidArea2;
								tris.trapezoidCoef = tris.trapezoidArea1 / tris.area;
							}
							veca = p1 - p2;
							vecb = p2 - p3;
							vecc = p3 - p1;
							tris.fullArea = IPSCore::HeronsF(veca, vecb, vecc);
							triangles.push_back(tris);
						}
						// Only a single vertex is inside - The face can be described as a
						//  triangle with an arc at the end.
						if(tris.vertsInside == 1)
						{
							Point3F veca,vecb,vecc;
							Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
							Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
							Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
							Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
							Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
							Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));

							PlaneF plane = PlaneF(p1,p2,p3);
							Point3F PlaneP = plane.project(center);
							if(tris.v1Inside && !tris.v2Inside && !tris.v3Inside)
							{
								veca = sideA1 - p1;
								vecb = sideC1 - p1;
								vecc = sideA1 - sideC1;

								tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, radius, sideC1, sideA1);

							}
							if(!tris.v1Inside && tris.v2Inside && !tris.v3Inside)
							{
								veca = sideB1 - p2;
								vecb = sideA1 - p2;
								vecc = sideB1 - sideA1;

								tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, radius, sideA1, sideB1);
							}
							if(!tris.v1Inside && !tris.v2Inside && tris.v3Inside)
							{
								veca = sideC1 - p3;
								vecb = sideB1 - p3;
								vecc = sideC1 - sideB1;

								tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, (PlaneP-sideB1).len(), sideB1, sideC1);
							}
							tris.area = IPSCore::HeronsF(veca, vecb, vecc);
							tris.fullArea = IPSCore::HeronsF(veca, vecb, vecc) + tris.arcArea1;
							tris.arcArea1Coeff = tris.arcArea1 / tris.area;
							triangles.push_back(tris);
						}
						// No vertexes is inside the sphere - the face is either not inside the sphere,
						//  or the sphere is small enough to fir inside the triangle.
						if(tris.vertsInside == 0)
						{
							PlaneF plane = PlaneF(p1,p2,p3);
							Point3F PlaneP = plane.project(center);
							IPSCore::triSphereIntersection(emitMesh, &tris, center, radius, p1, p2, p3);
							if(tris.inboundSphere)
							{
								tris.area = M_PI_F * IPSCore::square(radius);
								if(tris.sideAIntersect.count == 2)
								{
									Point3F c1 = p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff);
									Point3F c2 = p1+((p2-p1) * tris.sideAIntersect.intersection2Coeff);
									tris.area -= IPSCore::AreaOfCircularSegment(center,radius,c1,c2);
								}
								if(tris.sideBIntersect.count == 2)
								{
									Point3F c1 = p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff);
									Point3F c2 = p2+((p3-p2) * tris.sideBIntersect.intersection2Coeff);
									tris.area -= IPSCore::AreaOfCircularSegment(center,radius,c1,c2);
								}
								if(tris.sideCIntersect.count == 2)
								{
									Point3F c1 = p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff);
									Point3F c2 = p3+((p1-p3) * tris.sideCIntersect.intersection2Coeff);
									tris.area -= IPSCore::AreaOfCircularSegment(center,radius,c1,c2);
								}
								triangles.push_back(tris);
							}
							if(((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count != 2) || 
								(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
								(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)) &&
								!tris.inboundSphere)
							{
								PlaneF plane = PlaneF(p1,p2,p3);
								Point3F PlaneP = plane.project(center);
								if(tris.sideAIntersect.count == 2)
								{
									Point3F c1 = p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff);
									Point3F c2 = p1+((p2-p1) * tris.sideAIntersect.intersection2Coeff);
									tris.area = IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
								}
								if(tris.sideBIntersect.count == 2)
								{
									Point3F c1 = p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff);
									Point3F c2 = p2+((p3-p2) * tris.sideBIntersect.intersection2Coeff);
									tris.area = IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
								}
								if(tris.sideCIntersect.count == 2)
								{
									Point3F c1 = p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff);
									Point3F c2 = p3+((p1-p3) * tris.sideCIntersect.intersection2Coeff);
									tris.area = IPSCore::AreaOfCircularSegment(PlaneP,radius,c1,c2);
								}
								triangles.push_back(tris);
							}

							// Trapezoid with arcs -- 4 intersections
							if((tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2) || 
								(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2) || 
								(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2) &&
								!tris.inboundSphere)
							{
								Point3F veca = p1 - p2;
								Point3F vecb = p2 - p3;
								Point3F vecc = p3 - p1;
								Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
								Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
								Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
								Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
								Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
								Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));
								PlaneF plane = PlaneF(p1,p2,p3);
								Point3F PlaneP = plane.project(center);
								F32 a1radius = (sideA1 - PlaneP).len();
								F32 b1radius = (sideB1 - PlaneP).len();
								F32 c1radius = (sideC1 - PlaneP).len();

								tris.area = IPSCore::HeronsF(veca, vecb, vecc);
								if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2)
								{
									//trapezoid 1
									veca = sideB1 - sideA1;
									vecb = sideB2 - sideB1;
									vecc = sideA1 - sideB2;
									tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

									//trapezoid 2
									veca = sideA2 - sideA1;
									vecb = sideB1 - sideA2;
									vecc = sideA1 - sideB1;
									tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);

									tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, a1radius, sideA1, sideB2);
									tris.arcArea2 = IPSCore::AreaOfCircularSegment(PlaneP, a1radius, sideB1, sideA2);

									tris.area = tris.trapezoidArea1 + tris.trapezoidArea2 + tris.arcArea1 + tris.arcArea2;
									tris.trapezoidCoef = tris.trapezoidArea1 / (tris.trapezoidArea1 + tris.trapezoidArea2);
									tris.arcArea1Coeff = tris.arcArea1 / tris.area;
									tris.arcArea2Coeff = tris.arcArea2 / tris.area;
								}
								if(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2)
								{
									//trapezoid 1
									veca = sideC1 - sideB1;
									vecb = sideC2 - sideC1;
									vecc = sideB1 - sideC2;
									tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

									//trapezoid 2
									veca = sideB2 - sideB1;
									vecb = sideC1 - sideB2;
									vecc = sideB1 - sideC1;
									tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);

									tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, b1radius, sideB1, sideC2);
									tris.arcArea2 = IPSCore::AreaOfCircularSegment(PlaneP, b1radius, sideC1, sideB2);

									tris.area = tris.trapezoidArea1 + tris.trapezoidArea2 + tris.arcArea1 + tris.arcArea2;
									tris.trapezoidCoef = tris.trapezoidArea1 / (tris.trapezoidArea1 + tris.trapezoidArea2);
									tris.arcArea1Coeff = tris.arcArea1 / tris.area;
									tris.arcArea2Coeff = tris.arcArea2 / tris.area;
								}
								if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)
								{
									//trapezoid 1
									veca = sideA1 - sideC1;
									vecb = sideA2 - sideA1;
									vecc = sideC1 - sideA2;
									tris.trapezoidArea1 = IPSCore::HeronsF(veca, vecb, vecc);

									//trapezoid 2
									veca = sideC2 - sideC1;
									vecb = sideA1 - sideC2;
									vecc = sideC1 - sideA1;
									tris.trapezoidArea2 = IPSCore::HeronsF(veca, vecb, vecc);

									tris.arcArea1 = IPSCore::AreaOfCircularSegment(PlaneP, c1radius, sideC1, sideA2);
									tris.arcArea2 = IPSCore::AreaOfCircularSegment(PlaneP, c1radius, sideA1, sideC2);

									tris.area = tris.trapezoidArea1 + tris.trapezoidArea2 + tris.arcArea1 + tris.arcArea2;
									tris.trapezoidCoef = tris.trapezoidArea1 / (tris.trapezoidArea1 + tris.trapezoidArea2);
									tris.arcArea1Coeff = tris.arcArea1 / tris.area;
									tris.arcArea2Coeff = tris.arcArea2 / tris.area;
								}
								triangles.push_back(tris);
							}
						}
					}
				}
				else
				{
					Con::printf("Not tris");
				}
			}
		}

		F32 averageArea = 0;
		for(S32 index = 0; index < triangles.size(); index++)
		{
			averageArea += triangles[index].area;
		}
		averageArea = averageArea / triangles.size();

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

void RadiusMeshEmitter::debugRenderDelegate(ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance* overrideMat)
{
	IPSDebugRenderHelper* DRH = new IPSDebugRenderHelper(emitMesh);
	if(renderSphere)
		DRH->drawSphere(center, radius);
	for(S32 faceIndex = 0; faceIndex < emitfaces.size(); faceIndex++)
	{
		IPSCore::face tris = emitfaces[faceIndex];
		DRH->drawFace(tris, renderIntersections, renderEmittingFaces, true, radius, center, shadeByCoverage);
		if(tris.inboundSphere)
		{
			DRH->drawIntersections(tris);
		}
	}
}