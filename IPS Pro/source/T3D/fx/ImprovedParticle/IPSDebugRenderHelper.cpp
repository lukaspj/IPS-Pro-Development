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

#include "IPSDebugRenderHelper.h"
#include "gfx/gfxDrawUtil.h"
#include "platform/platform.h"
#include "console/consoleTypes.h"

#include "ts\tsShapeInstance.h"
#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "T3D\staticShape.h"
#include "ts\TSMeshIntrinsics.h"

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

void IPSDebugRenderHelper::drawFace(IPSCore::face tris, bool intersections, bool faces, bool isRadius = false, F32 radius = 0, Point3F center = Point3F(0))
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
		if(indiceBool = 0)
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
		if(indiceBool = 0)
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
			//trans.mulP(tris.sphereProjCenter,&worldProjCenter);
			if(tris.inboundSphere)
				du->drawArrow(d,pos+p1, pos+((p2-p1)*tris.sphereProjp1CoeffX) + ((p3-p1)*tris.sphereProjp1CoeffY),ColorI(255, 0, 255));
		}
		if(TS)
		{
			trans = TS->getTransform();
			nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
			//trans.mul(nodetrans);
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
			//mat.mulV(planeP);
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
					//if(shadeByCoverage)
					//col = ColorI(255*(tris.area/tris.fullArea),255*(tris.area/tris.fullArea),255*(tris.area/tris.fullArea));
					if(tris.fullArea < tris.area)
						int i = 0;
					if(col.red < 30 && col.blue < 30 && col.green < 30)
						int i = 0;
					//else continue;
					if(tris.v1Inside && tris.v2Inside && !tris.v3Inside)
					{
						//trapezoid 1
						//d.setFillModeSolid();
						d.setFillModeSolid();
						du->drawTriangle(d,pos+p1,pos+p2,pos+(p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) )),col);
						//d.setFillModeWireframe();
						//du->drawTriangle(d,pos+p1,pos+p2,pos+(p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) )),ColorI(0,0,0));

						//trapezoid 2
						//d.setFillModeSolid();
						d.setFillModeSolid();
						du->drawTriangle(d,pos+(p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ) )),pos+p2,pos+(p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) )),col);
						//d.setFillModeWireframe();
						//du->drawTriangle(d,pos+(p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ) )),pos+p2,pos+(p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) )),ColorI(0,0,0));
					}
					if(tris.v1Inside && !tris.v2Inside && tris.v3Inside)
					{
						//trapezoid 1
						//d.setFillModeSolid();
						d.setFillModeSolid();
						du->drawTriangle(d,pos+p1,pos+p1+(tris.sideAIntersect.intersection1Coeff*(p2-p1)), pos+p3, col);
						//d.setFillModeWireframe();
						//du->drawTriangle(d,pos+p1,pos+(p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ) )), pos+p3, ColorI(0,0,0));
						//trapezoid 2
						//d.setFillModeSolid();
						d.setFillModeSolid();
						du->drawTriangle(d,pos+p2+(tris.sideBIntersect.intersection1Coeff*(p3-p2)),pos+p1+(tris.sideAIntersect.intersection1Coeff*(p2-p1)), pos+p3, col);
						//d.setFillModeWireframe();
						//du->drawTriangle(d,pos+(p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ) )),pos+(p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ) )), pos+p3, ColorI(0,0,0));
					}
					if(!tris.v1Inside && tris.v2Inside && tris.v3Inside)
					{
						//trapezoid 1
						//d.setFillModeSolid();
						d.setFillModeSolid();
						du->drawTriangle(d, pos+p3+(tris.sideCIntersect.intersection1Coeff*(p1-p3)), pos+p2, pos+p3, col);
						//d.setFillModeWireframe();
						//du->drawTriangle(d,pos+(p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ) )), pos+p2, pos+p3, ColorI(0,0,0));
						//trapezoid 2
						//d.setFillModeSolid();
						d.setFillModeSolid();
						du->drawTriangle(d, pos+p3+(tris.sideCIntersect.intersection1Coeff*(p1-p3)), pos+p2, pos+p1+(tris.sideAIntersect.intersection1Coeff*(p2-p1)), col);
						//d.setFillModeWireframe();
						//du->drawTriangle(d,pos+(p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ) )), pos+p2, pos+(p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) )), ColorI(0,0,0));
					}
				}
				if(tris.vertsInside == 1)
				{
					if(tris.fullArea < tris.area)
						int i = 0;
					ColorI col = ColorI(0, 0, 255*b);
					//if(shadeByCoverage)
					//col = ColorI(255*(tris.area/tris.fullArea),255*(tris.area/tris.fullArea),255*(tris.area/tris.fullArea));
					if(col.red < 30 && col.blue < 30 && col.green < 30)
						int i = 0;
					//else continue;
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

					du->drawSphere(d,0.1,pos+p1,ColorI(255,0,0));
					du->drawSphere(d,0.1,pos+p2,ColorI(0,255,0));
					du->drawSphere(d,0.1,pos+p3,ColorI(0,0,255));
					if(tris.inboundSphere){
						d.setFillModeSolid();

						Point3F sphereProjCenter = pos+p1+((p2-p1)* tris.sphereProjp1CoeffX)+(((p3-p2))* tris.sphereProjp1CoeffY);
						sphereProjCenter = pos+planeP;
						Point3F sphereProjObjCenter = p1+((p2-p1)* tris.sphereProjp1CoeffX)+(((p3-p2))* tris.sphereProjp1CoeffY);
						sphereProjObjCenter = planeP;
						// raySphereIntersection(center,radius,p1+((p2-p1)* tris.sphereProjp1CoeffX),sphereProjObjCenter);
						IPSCore::intersection inter = IPSCore::raySphereIntersection(center,radius,p1,sphereProjObjCenter);
						Point3F centVec1 = inter.intersection1 - sphereProjObjCenter;
						F32 projRadius = centVec1.len();
						Point3F centVec2 = mCross(centVec1,Point3F(plane.x,plane.y,plane.z));

						Vector<Point3F> verts;
						for(int resI = 0; resI < 64; resI++)
						{
							F32 at, relx, rely, radianIndex;
							radianIndex = ((((2*M_PI)/64)*resI))-M_PI;
							relx = cos(radianIndex);
							rely = sin(radianIndex);
							at = radianIndex + M_PI_F;
							// http://tomyeah.com/signed-angle-between-two-vectors3d-in-cc/
							F32 signedAngleAFrom, signedAngleATo, signedAngleBFrom, signedAngleBTo, signedAngleCFrom, signedAngleCTo;
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
void IPSDebugRenderHelper::drawVector(Point3F V1, Point3F V2, ColorI col)
{
	GFXDrawUtil* du = GFX->getDrawUtil();
	GFXStateBlockDesc d;
	d.cullDefined = true;
	d.cullMode = GFXCullNone;
	d.fillMode = GFXFillWireframe;
	GFX->setStateBlockByDesc(d);
	du->drawArrow(d,V1,V2,col);
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
		if(indiceBool = 0)
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
		if(indiceBool = 0)
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
			//trans.mulP(tris.sphereProjCenter,&worldProjCenter);
			if(tris.inboundSphere)
				du->drawArrow(d,pos+p1, pos+((p2-p1)*tris.sphereProjp1CoeffX) + ((p3-p1)*tris.sphereProjp1CoeffY),ColorI(255, 0, 255));
		}
		if(TS)
		{
			trans = TS->getTransform();
			nodetrans = TS->getShapeInstance()->mNodeTransforms[0];
			//trans.mul(nodetrans);
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
			du->drawSphere(d, 0.05, pos+p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff), ColorI(255,0,0));
		if(tris.sideAIntersect.count == 2)
		{
			du->drawSphere(d, 0.05, pos+p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff), ColorI(255,0,0));
			du->drawSphere(d, 0.05, pos+p1+((p2-p1) * tris.sideAIntersect.intersection2Coeff), ColorI(255,0,0));
		}

		if(tris.sideBIntersect.count == 1)
			du->drawSphere(d, 0.05, pos+p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff), ColorI(0,255,0));
		if(tris.sideBIntersect.count == 2)
		{
			du->drawSphere(d, 0.05, pos+p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff), ColorI(0,255,0));
			du->drawSphere(d, 0.05, pos+p2+((p3-p2) * tris.sideBIntersect.intersection2Coeff), ColorI(0,255,0));
		}

		if(tris.sideCIntersect.count == 1)
			du->drawSphere(d, 0.05, pos+p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff), ColorI(0,0,255));
		if(tris.sideCIntersect.count == 2)
		{
			du->drawSphere(d, 0.05, pos+p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff), ColorI(0,0,255));
			du->drawSphere(d, 0.05, pos+p3+((p1-p3) * tris.sideCIntersect.intersection2Coeff), ColorI(0,0,255));
		}
	}
}