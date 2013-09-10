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

#include "IPSCore.h"
#include "gfx/gfxDrawUtil.h"
#include "platform/platform.h"
#include "console/consoleTypes.h"

#include "ts\tsShapeInstance.h"
#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "T3D\staticShape.h"
#include "ts\TSMeshIntrinsics.h"
#include <fstream>
#include "terrain\terrData.h"


// http://realtimecollisiondetection.net/blog/?p=103ds
// http://en.wikipedia.org/wiki/Plane%E2%80%93sphere_intersection
// http://www.blackpawn.com/texts/pointinpoly/default.html
void IPSCore::triSphereIntersection(StringTableEntry object, face* tri, Point3F center, F32 radius, Point3F p1, Point3F p2, Point3F p3)
{
	PROFILE_SCOPE(IPSCoretriSphereIntersection);
	tri->inboundSphere = false;
	Point3F A = p1 - center;
	Point3F B = p2 - center;
	Point3F C = p3 - center;

	Point3F V = mCross(B-A,C-A);
	Point3F N = V/sqrt(mDot(V,V));
	F32 d = abs(mDot(A,N));
	if(d > radius)
	{
		tri->inboundSphere = false;
		return;
	}

	PlaneF plane;
	plane = PlaneF(p1, p2, p3);
	Point3F planeP;

	planeP = plane.project(center);

	Point3F cp1 = mCross(p2-p1, planeP - p1);
	Point3F cp2 = mCross(p2-p1, p3 - p1);
	if(mDot(cp1,cp2) < 0)
	{
		tri->inboundSphere = false;
		return;
	}

	cp1 = mCross(p3-p2, planeP - p2);
	cp2 = mCross(p3-p2, p1 - p2);
	if(mDot(cp1,cp2) < 0)
	{
		tri->inboundSphere = false;
		return;
	}

	cp1 = mCross(p1-p3, planeP - p3);
	cp2 = mCross(p1-p3, p2 - p3);
	if(mDot(cp1,cp2) < 0)
	{
		tri->inboundSphere = false;
		return;
	}

	tri->sphereProjp1CoeffX = mDot(planeP - p1,(p2-p1)) / (p2-p1).len();
	tri->sphereProjp1CoeffY = mDot(planeP - (p1+((p2-p1) * tri->sphereProjp1CoeffX)),(p3-p2)) / (p3-p2).len();
	tri->inboundSphere = true;

	if(tri->sideAIntersect.count == 2 || tri->sideBIntersect.count == 2 || tri->sideCIntersect.count == 2)
	{
		SimObject* SB = dynamic_cast<SimObject*>(Sim::findObject(object));
		if(!SB)
			SB = dynamic_cast<SimObject*>(Sim::findObject(atoi(object)));
		ShapeBase* SS = NULL;
		TSStatic* TS = NULL;
		if(SB){
			SS = dynamic_cast<ShapeBase*>(SB);
			TS = dynamic_cast<TSStatic*>(SB);
		}
		// Make sure that we are dealing with some proper objects
		if(SB && (SS || TS)){
			intersection inter = raySphereIntersection(center,radius,p1,planeP);
			Point3F centVec1 = inter.intersection1 - planeP;
			Point3F centVec2 = mCross(centVec1,Point3F(plane.x,plane.y,plane.z));
			Point3F centVec1Norm = centVec1, centVec2Norm = centVec2;
			centVec1Norm.normalize();
			centVec2Norm.normalize();



			F32 lenx, leny, lenx2, leny2;

			// Side A
			if(tri->sideAIntersect.count == 2)
			{
				lenx = mDot(tri->sideAIntersect.intersection1 - planeP,centVec1) / radius;
				leny = mDot(tri->sideAIntersect.intersection1 - planeP,centVec2) / radius;
				lenx2 = mDot(tri->sideAIntersect.intersection2 - planeP,centVec1) / radius;
				leny2 = mDot(tri->sideAIntersect.intersection2 - planeP,centVec2) / radius;
				tri->sphereSideACosFrom = lenx / radius;
				tri->sphereSideASinFrom = leny / radius;
				tri->sphereSideACosTo = lenx2 / radius;
				tri->sphereSideASinTo = leny2 / radius;
			}
			else{
				tri->sphereSideACosFrom = 0;
				tri->sphereSideASinFrom = 0;
				tri->sphereSideACosTo = 0;
				tri->sphereSideASinTo = 0;
			}
			// End side A
			// Side B
			if(tri->sideBIntersect.count == 2)
			{
				lenx = mDot(tri->sideBIntersect.intersection1 - planeP,centVec1) / radius;
				leny = mDot(tri->sideBIntersect.intersection1 - planeP,centVec2) / radius;
				lenx2 = mDot(tri->sideBIntersect.intersection2 - planeP,centVec1) / radius;
				leny2 = mDot(tri->sideBIntersect.intersection2 - planeP,centVec2) / radius;
				tri->sphereSideBCosFrom = lenx / radius;
				tri->sphereSideBSinFrom = leny / radius;
				tri->sphereSideBCosTo = lenx2 / radius;
				tri->sphereSideBSinTo = leny2 / radius;
			}
			else{
				tri->sphereSideBCosFrom = 0;
				tri->sphereSideBSinFrom = 0;
				tri->sphereSideBCosTo = 0;
				tri->sphereSideBSinTo = 0;
			}
			// End side B
			// Side C
			if(tri->sideCIntersect.count == 2)
			{
				lenx = mDot(tri->sideCIntersect.intersection1 - planeP,centVec1) / radius;
				leny = mDot(tri->sideCIntersect.intersection1 - planeP,centVec2) / radius;
				lenx2 = mDot(tri->sideCIntersect.intersection2 - planeP,centVec1) / radius;
				leny2 = mDot(tri->sideCIntersect.intersection2 - planeP,centVec2) / radius;
				tri->sphereSideCCosFrom = lenx / radius;
				tri->sphereSideCSinFrom = leny / radius;
				tri->sphereSideCCosTo = lenx2 / radius;
				tri->sphereSideCSinTo = leny2 / radius;
			}
			else{
				tri->sphereSideCCosFrom = 0;
				tri->sphereSideCSinFrom = 0;
				tri->sphereSideCCosTo = 0;
				tri->sphereSideCSinTo = 0;
			}
		}
		// End side C
	}
}

// http://paulbourke.net/geometry/sphereline/
IPSCore::intersection IPSCore::raySphereIntersection(Point3F center, F32 radius, Point3F p1, Point3F p2)
{
	PROFILE_SCOPE(IPSCoreraySphereIntersection);
	intersection ret;
	ret.count = 0;
	// from  P1 coordinates (point of line)
	// to P2 coordinates (point of line)
	// center, r  P3 coordinates and radius (sphere)
	// x,y,z   intersection coordinates
	//
	// This function returns a pointer array which first index indicates
	// the number of intersection point, followed by coordinate pairs.

	float a, b, c, mu, i, u1, u2 ;
	
	a =  square(p2.x - p1.x) + square(p2.y - p1.y) + square(p2.z - p1.z);
	b =  2* ( (p2.x - p1.x)*(p1.x - center.x)
		+ (p2.y - p1.y)*(p1.y - center.y)
		+ (p2.z - p1.z)*(p1.z - center.z) ) ;
	c =  square(center.x) + square(center.y) +
		square(center.z) + square(p1.x) +
		square(p1.y) + square(p1.z) -
		2* ( center.x*p1.x + center.y*p1.y + center.z*p1.z ) - square(radius) ;
	i = square(b) - 4*a*c;
	if(i == 0)
		Con::printf("i=0");
	u1 = (-b + sqrt(i))/(2*a);
	u2 = (-b - sqrt(i))/(2*a);

	if((u1 < 0 && u2 < 0 )|| (u1 > 1 && u2 > 1))
	{
		// Outside sphere
		ret.count = 0;
		return ret;
	}
	if((u1 > 1 && u2 < 0 )|| (u1 < 0 && u2 > 1))
	{
		// Inside sphere
		ret.count = 0;
		return ret;
	}
	if((u1 >= 0 && u1 <= 1) && (u2 < 0 || u2 > 1))
	{
		// u1 intersect
		ret.count = 1;
		
		ret.intersection1 = Point3F( 
			p1.x + (u1*(p2.x-p1.x)),
			p1.y + (u1*(p2.y-p1.y)),
			p1.z + (u1*(p2.z-p1.z))
			);
		ret.intersection1Coeff = (ret.intersection1-p1).len() / (p2-p1).len();
		if(ret.intersection1Coeff > 1 || ret.intersection1Coeff < 0)
			Con::printf("ERROR");
		return ret;
	}
	if((u2 >= 0 && u2 <= 1) && (u1 < 0 || u1 > 1))
	{
		// u2 intersect
		ret.count = 1;
		
		ret.intersection1 = Point3F( 
			p1.x + (u2*(p2.x-p1.x)),
			p1.y + (u2*(p2.y-p1.y)),
			p1.z + (u2*(p2.z-p1.z))
			);
		ret.intersection1Coeff = (ret.intersection1-p1).len() / (p2-p1).len();
		if(ret.intersection1Coeff > 1 || ret.intersection1Coeff < 0)
			Con::printf("ERROR");
		return ret;
	}
	if((u2 >= 0 && u2 <= 1) && (u1 >= 0 && u1 <= 1))
	{
		// Both intersected
		ret.count = 2;
		
		// first intersection
		mu = (-b + sqrt( square(b) - 4*a*c )) / (2*a);
		ret.intersection1 = Point3F( 
			p1.x + (u1*(p2.x-p1.x)), 
			p1.y + (u1*(p2.y-p1.y)), 
			p1.z + (u1*(p2.z-p1.z))
			);
		ret.intersection1Coeff = (ret.intersection1-p1).len() / (p2-p1).len();
		if(ret.intersection1Coeff > 1 || ret.intersection1Coeff < 0)
		{
			ret.count--;
			ret.intersection1 = Point3F(0);
			ret.intersection1Coeff = 0;
			// second intersection
			mu = (-b - sqrt(square(b) - 4*a*c )) / (2*a);
			ret.intersection1 = Point3F( 
				p1.x + (u2*(p2.x-p1.x)), 
				p1.y + (u2*(p2.y-p1.y)), 
				p1.z + (u2*(p2.z-p1.z))
				);
			ret.intersection1Coeff = (ret.intersection1-p1).len() / (p2-p1).len();
			if(ret.intersection1Coeff > 1 || ret.intersection1Coeff < 0)
			{
				ret.count--;
				ret.intersection1 = Point3F(0);
				ret.intersection1Coeff = 0;
			}
		}
		else{
			// second intersection
			mu = (-b - sqrt(square(b) - 4*a*c )) / (2*a);
			ret.intersection2 = Point3F( 
				p1.x + (u2*(p2.x-p1.x)), 
				p1.y + (u2*(p2.y-p1.y)), 
				p1.z + (u2*(p2.z-p1.z))
				);
			ret.intersection2Coeff = (ret.intersection2-p1).len() / (p2-p1).len();
			if(ret.intersection2Coeff > 1 || ret.intersection2Coeff < 0)
			{
				ret.count--;
				ret.intersection2 = Point3F(0);
				ret.intersection2Coeff = 0;
			}
		}
		if(ret.intersection2Coeff < ret.intersection1Coeff)
		{
			swap(ret.intersection1, ret.intersection2);
			swap(ret.intersection1Coeff, ret.intersection2Coeff);
		}
		return ret;
	}
	ret.count = 0;
	return ret;
}

IPSCore::intersection IPSCore::lineSphereIntersection(Point3F center, F32 radius, Point3F p1, Point3F p2)
{
	PROFILE_SCOPE(IPSCorelineSphereIntersection);
	intersection ret;
	ret.count = 0;
	// from  P1 coordinates (point of line)
	// to P2 coordinates (point of line)
	// center, r  P3 coordinates and radius (sphere)
	// x,y,z   intersection coordinates
	//
	// This function returns a pointer array which first index indicates
	// the number of intersection point, followed by coordinate pairs.

	float a, b, c, mu, i, u1, u2 ;

	a =  square(p2.x - p1.x) + square(p2.y - p1.y) + square(p2.z - p1.z);
	b =  2* ( (p2.x - p1.x)*(p1.x - center.x)
		+ (p2.y - p1.y)*(p1.y - center.y)
		+ (p2.z - p1.z)*(p1.z - center.z) ) ;
	c =  square(center.x) + square(center.y) +
		square(center.z) + square(p1.x) +
		square(p1.y) + square(p1.z) -
		2* ( center.x*p1.x + center.y*p1.y + center.z*p1.z ) - square(radius) ;
	i = square(b) - 4*a*c;
	u1 = (-b + sqrt(i))/(2*a);
	u2 = (-b - sqrt(i))/(2*a);

	if(i >= 0)
	{
		// Both intersected
		ret.count++;

		// first intersection
		mu = (-b + sqrt( square(b) - 4*a*c )) / (2*a);
		ret.intersection1 = Point3F( 
			p1.x + (u1*(p2.x-p1.x)), 
			p1.y + (u1*(p2.y-p1.y)), 
			p1.z + (u1*(p2.z-p1.z))
			);
		ret.intersection1Coeff = (ret.intersection1-p1).len() / (p2-p1).len();

		// Both intersected
		ret.count++;

		// second intersection
		mu = (-b - sqrt(square(b) - 4*a*c )) / (2*a);
		ret.intersection2 = Point3F( 
			p1.x + (u2*(p2.x-p1.x)), 
			p1.y + (u2*(p2.y-p1.y)), 
			p1.z + (u2*(p2.z-p1.z))
			);
		ret.intersection2Coeff = (ret.intersection2-p1).len() / (p2-p1).len();
	}
	return ret;
}
F32 IPSCore::HeronsF(Point3F a, Point3F b, Point3F c)
{
	F32 bounds = (a.len() + b.len() + c.len()) / 2;
	F32 Mres = bounds * (bounds - a.len()) * (bounds - b.len()) * (bounds - c.len());
	if(Mres < 0)
		Mres = Mres * -1;
	F32 area = sqrt(Mres);
	return area;
}

// http://tomyeah.com/signed-angle-between-two-vectors3d-in-cc/
F32 IPSCore::AngleBetween(Point3F N, Point3F V1, Point3F V2)
{
	V1.normalize();
	V2.normalize();
	N.normalize();
	return atan2(
		mDot(N, mCross(V1,V2)),
		mDot(V1,V2)
		);
}

F32 IPSCore::ChordHeight(Point3F p1, Point3F p2, Point3F N, Point3F center, F32 radius)
{
	return radius*(1-cos((AngleBetween(N,(p1-center),(p2-center)))/2));
}

F32 IPSCore::ChordHeight(Point3F p1, Point3F p2, Point3F N, Point3F center, F32 radius, F32 coefficient)
{
	PROFILE_SCOPE(IPSCoreChordHeight);
	Point3F chord = p2-p1;
	chord *= (coefficient);
	Point3F perChord = IPSCore::perpendicularVector(chord, N);
	IPSCore::intersection inter = IPSCore::lineSphereIntersection(center, radius, p1+chord, p1+chord+perChord);
	F32 Height = 0;
	if( ((p1+chord) - inter.intersection1).len() < ((p1+chord) - inter.intersection2).len() )
		Height = ((p1+chord) - inter.intersection1).len();
	else
		Height = ((p1+chord) - inter.intersection2).len();
	if(inter.count != 1 && inter.count != 2)
		Height = -1;
	return Height;
}

Point3F IPSCore::perpendicularVector(Point3F V1, Point3F N)
{
	return mCross(V1,N);
}

Point3F IPSCore::project(Point3F V1, Point3F V2)
{
	Point3F V2norm = V2;
	V2norm.normalize();
	return mDot(V1,V2norm) * V2norm;
}

F32 IPSCore::AreaOfCircularSegment(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd)
{
	Point3F chord = chordEnd-chordStart;
	PlaneF normal = PlaneF(center,chordStart,chordEnd);
	F32 theta = AngleBetween(normal,center-chordStart,center-chordEnd);
	return abs(0.5*radius*(theta-sin(theta)));
}

bool IPSCore::IsPointInArc(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd, Point3F point)
{
	Point3F top = TopOfCircularSegment(center, radius, chordStart, chordEnd);
	if(SameSideOfLine(chordStart, chordEnd, point, top))
		return false;
	else
		return true;
}

bool IPSCore::SameSideOfLine(Point3F lineStart, Point3F lineEnd, Point3F p1, Point3F p2)
{
	Point3F cp1 = mCross(lineEnd-lineStart, p1-lineStart);
	Point3F cp2 = mCross(lineEnd-lineStart, p2-lineStart);
	if(mDot(cp1,cp2) >= 0)
		return true;
	else
		return false;
}

Point3F IPSCore::TopOfCircularSegment(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd)
{
	Point3F top = center-((chordStart+chordEnd)/2);
	top.normalize();
	top *= radius;
	return center+top;
}

bool IPSCore::GetTerrainInfoAtPosition(const F32 x, const F32 y, F32 &height, Point3F &normal)
{
	Point3F startPnt( x, y, 10000.0f );
	Point3F endPnt( x, y, -10000.0f );

	RayInfo ri;
	bool hit;         

	hit = gClientContainer.castRay(startPnt, endPnt, TerrainObjectType, &ri);   

	if ( hit ){
		height = ri.point.z;
		normal = ri.normal;
	}

	return hit;
}

bool IPSCore::GetTerrainInfoAtPosition(const F32 &x, const F32 &y, F32 &height, StringTableEntry &mat, Point3F &normal)
{
	PROFILE_SCOPE(GroundEmitGetTerHeight);
	Point3F startPnt( x, y, 10000.0f );
	Point3F endPnt( x, y, -10000.0f );

	RayInfo ri;
	bool hit;         

	// Cast a ray from very far up to very far down.
	hit = gClientContainer.castRay(startPnt, endPnt, TerrainObjectType, &ri);   

	if ( hit ){
		height = ri.point.z;
		normal = ri.normal;
		TerrainBlock* TB = dynamic_cast< TerrainBlock* > ( ri.object );  
		Point2I gridPos = TB->getGridPos( ri.point );
		U8 layer = TB->getFile()->getLayerIndex( gridPos.x, gridPos.y );
		mat = TB->getMaterialName(layer);
	}

	return hit;
}

DefineConsoleFunction(DumpIPSBenchmark, void,(),, "Null")
{
	PrintBenchMarks;
	return;
};