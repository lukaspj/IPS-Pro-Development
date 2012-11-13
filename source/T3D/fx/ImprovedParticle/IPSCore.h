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

#ifndef _IPSCore_H_
#define _IPSCore_H_
#include "gfx/gfxDrawUtil.h"

#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"

class IPSCore{
public:
	struct intersection
	{
		int count;
		Point3F intersection1;
		Point3F intersection2;
		F32 intersection1Coeff;
		F32 intersection2Coeff;
	};

	struct face
	{
		S32 triStart;
		S32 meshIndex;
		bool skinMesh;
		F32 fullArea;
		F32 area;
		intersection sideAIntersect;
		intersection sideBIntersect;
		intersection sideCIntersect;
		bool v1Inside;
		bool v2Inside;
		bool v3Inside;
		int vertsInside;
		bool inboundSphere;
		F32 sphereProjp1CoeffX;
		F32 sphereProjp1CoeffY;
		F32 sphereSideACosFrom;
		F32 sphereSideACosTo;
		F32 sphereSideBCosFrom;
		F32 sphereSideBCosTo;
		F32 sphereSideCCosFrom;
		F32 sphereSideCCosTo;
		F32 sphereSideASinFrom;
		F32 sphereSideASinTo;
		F32 sphereSideBSinFrom;
		F32 sphereSideBSinTo;
		F32 sphereSideCSinFrom;
		F32 sphereSideCSinTo;

		F32 trapezoidArea1;
		F32 trapezoidArea2;
		F32 trapezoidCoef;

		F32 arcArea1;
		F32 arcArea1Coeff;
		F32 arcArea2;
		F32 arcArea2Coeff;
	};

	static IPSCore::intersection raySphereIntersection(Point3F center, F32 radius, Point3F from, Point3F to);
	static IPSCore::intersection lineSphereIntersection(Point3F center, F32 radius, Point3F from, Point3F to);
	static void triSphereIntersection(StringTableEntry object, face* tri, Point3F center, F32 radius, Point3F p1, Point3F p2, Point3F p3);
	static F32 HeronsF(Point3F a, Point3F b, Point3F c);
	static F32 AngleBetween(Point3F N, Point3F V1, Point3F V2);
	static F32 ChordHeight(Point3F p1, Point3F p2, Point3F N, Point3F center, F32 radius);
	static F32 ChordHeight(Point3F p1, Point3F p2, Point3F N, Point3F center, F32 radius, F32 coefficient);
	static float square( float f ) { return (f*f); };
	static Point3F perpendicularVector(Point3F V1, Point3F N);
	static Point3F project(Point3F V1, Point3F V2);
	static F32 AreaOfCircularSegment(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd);
	static bool IsPointInArc(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd, Point3F point);
	static bool SameSideOfLine(Point3F lineStart, Point3F lineEnd, Point3F p1, Point3F p2);
	static Point3F TopOfCircularSegment(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd);
};
#endif