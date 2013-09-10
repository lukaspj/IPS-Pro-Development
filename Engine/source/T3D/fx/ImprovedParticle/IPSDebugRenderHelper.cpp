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

#include "IPSDebugRenderHelper.h"
#include "gfx/gfxDrawUtil.h"
#include "platform/platform.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "math/mathIO.h"

#include "ts\tsShapeInstance.h"
#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "T3D\staticShape.h"
#include "ts\TSMeshIntrinsics.h"

#include "gfx/sim/debugDraw.h"

IPSDebugRenderHelper::IPSDebugRenderHelper(StringTableEntry object)
{
	SB = dynamic_cast<SimObject*>(Sim::findObject(object));
	if(!SB)
		SB = dynamic_cast<SimObject*>(Sim::findObject(atoi(object)));
	if(SB){
		SS = dynamic_cast<ShapeBase*>(SB);
		TS = dynamic_cast<TSStatic*>(SB);
	}
}

void IPSDebugRenderHelper::drawSphere(Point3F center, F32 radius)
{
	if(SB && (SS || TS)){
		GFXDrawUtil* du = GFX->getDrawUtil();
		GFXStateBlockDesc d;
		d.cullDefined = true;
		d.cullMode = GFXCullNone;
		d.fillMode = GFXFillWireframe;
		GFX->setStateBlockByDesc(d);

		Point3F worldCenter;
		MatrixF trans;
		if(SS)
		{
			trans = SS->getTransform();
			trans.mulP(center,&worldCenter);
		}
		if(TS)
		{
			trans = TS->getTransform();
			trans.mulP(center,&worldCenter);
		}

		du->drawSphere(d,radius,worldCenter,ColorI(0, 255, 120, 255));
	}
}

void IPSDebugRenderHelper::drawFace(IPSCore::face tris, bool intersections, bool faces, bool isRadius = false, F32 radius = 0, Point3F center = Point3F(0), bool shadeByCoverage = false)
{
	Point3F worldCenter;
	MatrixF trans;
	MatrixF nodetrans;
	MatrixF mat;
	if(SS)
	{
		trans = SS->getTransform();
		trans.mulP(center,&worldCenter);
	}
	if(TS)
	{
		trans = TS->getTransform();
		trans.mulP(center,&worldCenter);
	}

	GFXDrawUtil* du = GFX->getDrawUtil();
	GFXStateBlockDesc d;
	d.cullDefined = true;
	d.cullMode = GFXCullNone;
	d.fillMode = GFXFillWireframe;
	GFX->setStateBlockByDesc(d);

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
	
	const TSShape::Detail& det = shape->details[0];
	S32 od = det.objectDetailNum;
	TSSkinMesh* sMesh;
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
		Point3F pos;
		if(SS)
		{
			trans = SS->getTransform();
			trans.mulP(v1.vert(),&p1);
			trans.mulP(v2.vert(),&p2);
			trans.mulP(v3.vert(),&p3);
			pos = SS->getPosition();

			Point3F worldProjCenter;
			if(tris.inboundSphere)
				du->drawArrow(d,pos+p1, pos+((p2-p1)*tris.sphereProjp1CoeffX) + ((p3-p1)*tris.sphereProjp1CoeffY),ColorI(255, 0, 255));
		}
		if(TS)
		{
			trans = TS->getTransform();
			nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
			trans.mulV(v1.vert(),&p1);
			trans.mulV(v2.vert(),&p2);
			trans.mulV(v3.vert(),&p3);

			mat.mul(trans,nodetrans);
			mat.mulV(v1.vert(),&p1);
			mat.mulV(v2.vert(),&p2);
			mat.mulV(v3.vert(),&p3);
			p1 *= TS->getScale();
			p2 *= TS->getScale();
			p3 *= TS->getScale();
			pos = TS->getPosition();

			Point3F V = mCross(p2-p1,p3-p1);
			Point3F N = V/sqrt(mDot(V,V));
			PlaneF plane = PlaneF(V, N);
			plane = PlaneF(p1,p2,p3);
			Point3F planeP;
			planeP = plane.project(center);
			Point3F worldProjCenter;
			mat.mulV(tris.sideAIntersect.intersection2,&worldProjCenter);

			S32 seedbuffer = gRandGen.getSeed();

			d.setFillModeSolid();

			// Point to intersections
			if(intersections)
			{
				gRandGen.setSeed(tris.triStart);
				gRandGen.randF();
				gRandGen.randF();
				U32 r = gRandGen.randI() % 255;
				U32 g = gRandGen.randI() % 255;
				U32 b = gRandGen.randI() % 255;
				gRandGen.setSeed(seedbuffer);
				if(tris.sideAIntersect.count == 1)
					du->drawArrow(d,worldCenter, pos+p1+(tris.sideAIntersect.intersection1Coeff*(p2-p1)),ColorI(r,g,b));
				if(tris.sideBIntersect.count == 1)
					du->drawArrow(d,worldCenter, pos+p2+(tris.sideBIntersect.intersection1Coeff*(p3-p2)),ColorI(r,g,b));
				if(tris.sideCIntersect.count == 1)
					du->drawArrow(d,worldCenter, pos+p3+(tris.sideCIntersect.intersection1Coeff*(p1-p3)),ColorI(r,g,b));
			}
			// 3 Verts inside
			if(faces)
			{
				gRandGen.setSeed(tris.triStart);
				gRandGen.randF();
				gRandGen.randF();
				F32 r = gRandGen.randF();
				F32 g = gRandGen.randF();
				F32 b = gRandGen.randF();
				gRandGen.setSeed(seedbuffer);

				if(tris.vertsInside == 3)
				{
					//d.setFillModeSolid();
					ColorI col = ColorI(255*r, 0, 0);
					//if(shadeByCoverage)
					//col = ColorI(255,255,255);
					d.setFillModeSolid();
					du->drawTriangle(d,pos+p1,pos+p2,pos+p3,col);
					//d.setFillModeWireframe();
					//du->drawTriangle(d,pos+p1+(p2-p1),pos+p2+(p3-p2),pos+p3+(p1-p3),ColorI(0,0,0));
					/*du->drawArrow(d,pos+p1+(p2-p1), pos+p2+(p3-p2),ColorI(r,g,b));
					du->drawArrow(d,pos+p2+(p3-p2), pos+p3+(p1-p3),ColorI(r,g,b));
					du->drawArrow(d,pos+p3+(p1-p3), pos+p1+(p2-p1),ColorI(r,g,b));*/
				}
				if(tris.vertsInside == 2)
				{
					ColorI col = ColorI(0, 255*g, 0);
					if(shadeByCoverage)
						col = ColorI(255*(tris.area/tris.fullArea),255*(tris.area/tris.fullArea),255*(tris.area/tris.fullArea));
					if(tris.v1Inside && tris.v2Inside && !tris.v3Inside)
					{
						col = ColorI(125*r, 255*g, 0);
						//trapezoid 1
						d.setFillModeSolid();
						du->drawTriangle(d,pos+p1,pos+p2,pos+(p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) )),col);

						//trapezoid 2
						d.setFillModeSolid();
						du->drawTriangle(d,pos+(p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ) )),pos+p2,pos+(p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) )),col);
					}
					if(tris.v1Inside && !tris.v2Inside && tris.v3Inside)
					{
						col = ColorI(0, 255*g, 125*b);
						//trapezoid 1
						d.setFillModeSolid();
						du->drawTriangle(d,pos+p1,pos+p1+(tris.sideAIntersect.intersection1Coeff*(p2-p1)), pos+p3, col);

						//trapezoid 2
						d.setFillModeSolid();
						du->drawTriangle(d,pos+p2+(tris.sideBIntersect.intersection1Coeff*(p3-p2)),pos+p1+(tris.sideAIntersect.intersection1Coeff*(p2-p1)), pos+p3, col);
					}
					if(!tris.v1Inside && tris.v2Inside && tris.v3Inside)
					{
						//trapezoid 1
						d.setFillModeSolid();
						du->drawTriangle(d, pos+p3+(tris.sideCIntersect.intersection1Coeff*(p1-p3)), pos+p2, pos+p3, col);

						//trapezoid 2
						d.setFillModeSolid();
						du->drawTriangle(d, pos+p3+(tris.sideCIntersect.intersection1Coeff*(p1-p3)), pos+p2, pos+p1+(tris.sideAIntersect.intersection1Coeff*(p2-p1)), col);
					}
				}
				if(tris.vertsInside == 1)
				{
					ColorI col = ColorI(0, 0, 255*b);
					if(shadeByCoverage)
						col = ColorI(255*(tris.area/tris.fullArea),255*(tris.area/tris.fullArea),255*(tris.area/tris.fullArea));
					if(tris.v1Inside && !tris.v2Inside && !tris.v3Inside)
					{
						d.setFillModeSolid();
						du->drawTriangle(d,pos+p1,pos+p1+(tris.sideAIntersect.intersection1Coeff*(p2-p1)),pos+p3+(tris.sideCIntersect.intersection1Coeff*(p1-p3)),col);
					}
					if(!tris.v1Inside && tris.v2Inside && !tris.v3Inside)
					{
						d.setFillModeSolid();
						du->drawTriangle(d,pos+p1+(tris.sideAIntersect.intersection1Coeff*(p2-p1)),pos+p2,pos+p2+(tris.sideBIntersect.intersection1Coeff*(p3-p2)),col);
					}
					if(!tris.v1Inside && !tris.v2Inside && tris.v3Inside)
					{
						d.setFillModeSolid();
						du->drawTriangle(d,pos+p3+(tris.sideCIntersect.intersection1Coeff*(p1-p3)),pos+p2+(tris.sideBIntersect.intersection1Coeff*(p3-p2)),pos+p3,col);
					}
				}
				if(tris.vertsInside == 0)
				{

					du->drawSphere(d,0.1f,pos+p1,ColorI(255,0,0));
					du->drawSphere(d,0.1f,pos+p2,ColorI(0,255,0));
					du->drawSphere(d,0.1f,pos+p3,ColorI(0,0,255));
					if(tris.inboundSphere){
						d.setFillModeSolid();

						Point3F sphereProjCenter = pos+p1+((p2-p1)* tris.sphereProjp1CoeffX)+(((p3-p2))* tris.sphereProjp1CoeffY);
						sphereProjCenter = pos+planeP;
						Point3F sphereProjObjCenter = p1+((p2-p1)* tris.sphereProjp1CoeffX)+(((p3-p2))* tris.sphereProjp1CoeffY);
						sphereProjObjCenter = planeP;
						IPSCore::intersection inter = IPSCore::raySphereIntersection(center,radius,p1,sphereProjObjCenter);
						Point3F centVec1 = inter.intersection1 - sphereProjObjCenter;
						Point3F centVec2 = mCross(centVec1,Point3F(plane.x,plane.y,plane.z));

						Vector<Point3F> verts;
						for(int resI = 0; resI < 64; resI++)
						{
							F32 at, relx, rely, radianIndex;
							radianIndex = ((((2*M_PI)/64)*resI))-M_PI;
							relx = cos(radianIndex);
							rely = sin(radianIndex);
							at = radianIndex + M_PI_F;
							F32 signedAngleAFrom = 0.0f, 
								signedAngleATo = 0.0f, 
								signedAngleBFrom = 0.0f, 
								signedAngleBTo = 0.0f, 
								signedAngleCFrom = 0.0f, 
								signedAngleCTo = 0.0f;
							if(tris.sideAIntersect.count == 2){
								Point3F V2 = centVec1;
								Point3F V1 = (p1+((p2-p1)*tris.sideAIntersect.intersection1Coeff))-planeP;
								Point3F N = plane;
								signedAngleAFrom = IPSCore::AngleBetween(N,V1,V2) + M_PI;
								V1 = (p1+((p2-p1)*tris.sideAIntersect.intersection2Coeff))-planeP;
								signedAngleATo = IPSCore::AngleBetween(N,V1,V2) + M_PI;
							}
							if(tris.sideBIntersect.count == 2){
								Point3F V2 = centVec1;
								Point3F V1 = (p2+((p3-p2)*tris.sideBIntersect.intersection1Coeff))-planeP;
								Point3F N = plane;
								signedAngleBFrom = IPSCore::AngleBetween(N,V1,V2) + M_PI;
								V1 = (p2+((p3-p2)*tris.sideBIntersect.intersection2Coeff))-planeP;
								signedAngleBTo = IPSCore::AngleBetween(N,V1,V2) + M_PI;
							}
							if(tris.sideCIntersect.count == 2){
								Point3F V2 = centVec1;
								Point3F V1 = (p3+((p1-p3)*tris.sideCIntersect.intersection1Coeff))-planeP;
								Point3F N = plane;
								signedAngleCFrom = IPSCore::AngleBetween(N,V1,V2) + M_PI;
								V1 = (p3+((p1-p3)*tris.sideCIntersect.intersection2Coeff))-planeP;
								signedAngleCTo = IPSCore::AngleBetween(N,V1,V2) + M_PI;
							}
							if(tris.sideAIntersect.count == 2 && at > signedAngleAFrom && at < signedAngleATo)
								continue;
							if(tris.sideBIntersect.count == 2 && !(at < signedAngleBFrom && at > signedAngleBTo))
								continue;
							if(tris.sideCIntersect.count == 2 && at > signedAngleCFrom && at < signedAngleCTo)
								continue;
							Point3F pointVec = (centVec1 * relx) + (centVec2 * rely);
							verts.push_back(sphereProjCenter + pointVec);
						}
						du->drawPolygon(d,verts.address(),verts.size(),ColorI(0,0,0));
					}
					else{
					}
				}
			}
		}
	}
}

void IPSDebugRenderHelper::drawIntersections(IPSCore::face tris)
{
	Point3F worldCenter;
	MatrixF trans;
	MatrixF nodetrans;
	MatrixF mat;

	GFXDrawUtil* du = GFX->getDrawUtil();
	GFXStateBlockDesc d;
	d.cullDefined = true;
	d.cullMode = GFXCullNone;
	d.fillMode = GFXFillWireframe;
	GFX->setStateBlockByDesc(d);

	const TSShape* shape;
	if(SS)
		shape = SS->getShape();
	else{
		shape = TS->getShape();
	}

	TSSkinMesh* sMesh;
	TSMesh* Mesh;
	Mesh = shape->meshes[tris.meshIndex];
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
		Point3F pos;
		if(SS)
		{
			trans = SS->getTransform();
			trans.mulP(v1.vert(),&p1);
			trans.mulP(v2.vert(),&p2);
			trans.mulP(v3.vert(),&p3);
			pos = SS->getPosition();

			Point3F worldProjCenter;
			if(tris.inboundSphere)
				du->drawArrow(d,pos+p1, pos+((p2-p1)*tris.sphereProjp1CoeffX) + ((p3-p1)*tris.sphereProjp1CoeffY),ColorI(255, 0, 255));
		}
		if(TS)
		{
			trans = TS->getTransform();
			nodetrans = TS->getShapeInstance()->mNodeTransforms[0];

			trans.mulV(v1.vert(),&p1);
			trans.mulV(v2.vert(),&p2);
			trans.mulV(v3.vert(),&p3);

			mat.mul(trans,nodetrans);
			mat.mulV(v1.vert(),&p1);
			mat.mulV(v2.vert(),&p2);
			mat.mulV(v3.vert(),&p3);
			p1 *= TS->getScale();
			p2 *= TS->getScale();
			p3 *= TS->getScale();
			pos = TS->getPosition();
		}
		if(tris.sideAIntersect.count == 1)
			du->drawSphere(d, 0.05f, pos+p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff), ColorI(255,0,0));
		if(tris.sideAIntersect.count == 2)
		{
			du->drawSphere(d, 0.05f, pos+p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff), ColorI(255,0,0));
			du->drawSphere(d, 0.05f, pos+p1+((p2-p1) * tris.sideAIntersect.intersection2Coeff), ColorI(255,0,0));
		}

		if(tris.sideBIntersect.count == 1)
			du->drawSphere(d, 0.05f, pos+p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff), ColorI(0,255,0));
		if(tris.sideBIntersect.count == 2)
		{
			du->drawSphere(d, 0.05f, pos+p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff), ColorI(0,255,0));
			du->drawSphere(d, 0.05f, pos+p2+((p3-p2) * tris.sideBIntersect.intersection2Coeff), ColorI(0,255,0));
		}

		if(tris.sideCIntersect.count == 1)
			du->drawSphere(d, 0.05f, pos+p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff), ColorI(0,0,255));
		if(tris.sideCIntersect.count == 2)
		{
			du->drawSphere(d, 0.05f, pos+p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff), ColorI(0,0,255));
			du->drawSphere(d, 0.05f, pos+p3+((p1-p3) * tris.sideCIntersect.intersection2Coeff), ColorI(0,0,255));
		}
	}
}

//S32 curTime = Platform::getTime();
IMPLEMENT_CO_NETOBJECT_V1(TimedDebugVector);

//-----------------------------------------------------------------------------
// Object setup and teardown
//-----------------------------------------------------------------------------
TimedDebugVector::TimedDebugVector()
{
   // Flag this object so that it will always
   // be sent across the network to clients
   mNetFlags.set( Ghostable | ScopeAlways );

   // Set it as a "static" object that casts shadows
   mTypeMask |= StaticObjectType | StaticShapeObjectType;
   
   mStart = Point3F(0);
   mEnd = Point3F(0);
   mColor = ColorI(gRandGen.randI() % 255, gRandGen.randI() % 255, gRandGen.randI() % 255);
   mInitialTime = Platform::getTime();
}

//-----------------------------------------------------------------------------
// Object Editing
//-----------------------------------------------------------------------------
void TimedDebugVector::initPersistFields()
{
   addField("start", TypePoint3F, Offset(mStart, TimedDebugVector), "");
   addField("end", TypePoint3F, Offset(mEnd, TimedDebugVector), "");
   addField("duration", TypeS32, Offset(mDuration, TimedDebugVector), "");

   // SceneObject already handles exposing the transform
   Parent::initPersistFields();
}

void TimedDebugVector::inspectPostApply()
{
   Parent::inspectPostApply();

   // Flag the network mask to send the updates
   // to the client object
   setMaskBits( UpdateMask );
}

bool TimedDebugVector::onAdd()
{
   if ( !Parent::onAdd() )
      return false;

   // Set up a bounding box
   mWorldBox.set( Point3F( getMin(mStart.x, mEnd.x),
                           getMin(mStart.y, mEnd.y),
                           getMin(mStart.z, mEnd.z)),
                  Point3F( getMax(mStart.x, mEnd.x),
                           getMax(mStart.y, mEnd.y),
                           getMax(mStart.z, mEnd.z)));

   resetObjectBox();

   // Add this object to the scene
   addToScene();

   return true;
}

void TimedDebugVector::onRemove()
{
   // Remove this object from the scene
   removeFromScene();

   Parent::onRemove();
}

void TimedDebugVector::setTransform(const MatrixF & mat)
{
   // Let SceneObject handle all of the matrix manipulation
   Parent::setTransform( mat );

   // Dirty our network mask so that the new transform gets
   // transmitted to the client object
   setMaskBits( TransformMask );
}

U32 TimedDebugVector::packUpdate( NetConnection *conn, U32 mask, BitStream *stream )
{
   // Allow the Parent to get a crack at writing its info
   U32 retMask = Parent::packUpdate( conn, mask, stream );

   // Write our transform information
   if ( stream->writeFlag( mask & TransformMask ) )
   {
      mathWrite(*stream, getTransform());
      mathWrite(*stream, getScale());
   }

   // Write out any of the updated editable properties
   if ( stream->writeFlag( mask & UpdateMask ) )
   {
      mathWrite(*stream, mStart);
      mathWrite(*stream, mEnd);
      stream->writeInt(mDuration, 32);
   }

   return retMask;
}

void TimedDebugVector::unpackUpdate(NetConnection *conn, BitStream *stream)
{
   // Let the Parent read any info it sent
   Parent::unpackUpdate(conn, stream);

   if ( stream->readFlag() )  // TransformMask
   {
      mathRead(*stream, &mObjToWorld);
      mathRead(*stream, &mObjScale);

      setTransform( mObjToWorld );
   }

   if ( stream->readFlag() )  // UpdateMask
   {
      mathRead(*stream, &mStart);
      mathRead(*stream, &mEnd);
      mDuration = stream->readInt(32);
   }
}

//-----------------------------------------------------------------------------
// Object Rendering
//-----------------------------------------------------------------------------
void TimedDebugVector::prepRenderImage( SceneRenderState *state )
{
   if(mDead)
      return;

   if(Platform::getTime() > mInitialTime + mDuration) {
      mDead = true;
      this->safeDeleteObject();
   }

   DebugDrawer::get()->drawLine(mStart, mEnd, mColor);
   DebugDrawer::get()->setLastTTL(0);
}

DefineEngineMethod( TimedDebugVector, postApply, void, (),,
   "A utility method for forcing a network update.\n")
{
	object->inspectPostApply();
}