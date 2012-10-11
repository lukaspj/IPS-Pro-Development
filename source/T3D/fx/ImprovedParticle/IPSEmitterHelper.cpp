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

#include "IPSEmitterHelper.h"
#include "gfx/gfxDrawUtil.h"
#include "platform/platform.h"
#include "console/consoleTypes.h"

#include "ts\tsShapeInstance.h"
#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "T3D\staticShape.h"
#include "ts\TSMeshIntrinsics.h"
#include "IPSCore.h"
#include "IPSDebugRenderHelper.h"

void IPSEmitterHelper::uniformPointInTriangle(Point3F p1, Point3F p2, Point3F p3, Point3F* out)
{
	PROFILE_SCOPE(IPSCoreuniformPointInTriangle);
	Point3F vec1;
	Point3F vec2;
	Point3F base;
	base = p1;
	vec1 = p2-p1;
	vec2 = p3-p2;
	F32 K1 = rand() % 1000 + 1;
	F32 K2 = rand() % 1000 + 1;

	if(K2 <= K1)
		*out = base + (vec1 * (K1 / 1000)) + (vec2 * (K2 / 1000));
	else
		*out = base + (vec1 * (1-(K1 / 1000))) + (vec2 * (1-(K2 / 1000)));
}

void IPSEmitterHelper::uniformPointInTrapezoid(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F* out)
{
	PROFILE_SCOPE(IPSCoreuniformPointInTrapezoid);
	Point3F vec1;
	Point3F vec2;
	Point3F vec3;
	Point3F base;

	Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
	Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
	Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
	Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
	Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
	Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));
	if(tris.vertsInside > 0)
	{
		if(tris.v1Inside && tris.v2Inside && !tris.v3Inside)
		{
			F32 coef = gRandGen.randF();
			if(coef <= tris.trapezoidCoef)
			{
				//trapezoid 1
				vec1 = p2 - p1;
				vec2 = sideC1 - p2;
				base = p1;
			}
			else
			{

				//trapezoid 2
				vec1 = sideB1 - p2;
				vec2 = sideC1 - sideB1;
				base = p2;
			}

		}
		if(tris.v1Inside && !tris.v2Inside && tris.v3Inside)
		{
			F32 coef = gRandGen.randF();
			if(coef <= tris.trapezoidCoef)
			{
				//trapezoid 1
				vec1 = p3 - p1;
				vec2 = sideA1 - p3;
				base = p1;
			}
			else
			{
				//trapezoid 2
				vec1 = sideA1 - p3;
				vec2 = sideB1 - sideA1;
				base = p3;
			}
		}
		if(!tris.v1Inside && tris.v2Inside && tris.v3Inside)
		{
			F32 coef = gRandGen.randF();
			if(coef <= tris.trapezoidCoef)
			{
				//trapezoid 1
				vec1 = p3 - p2;
				vec2 = sideB1 - p3;
				base = p2;
			}
			else
			{
				//trapezoid 2
				vec1 = sideC1 - p3;
				vec2 = sideA1 - sideC1;
				base = p3;
			}
		}
	}
	else
	{
		if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2)
		{
			F32 coef = gRandGen.randF();
			if(coef <= tris.trapezoidCoef)
			{
				//trapezoid 1
				vec1 = sideB1 - sideA1;
				vec2 = sideB2 - sideB1;
				base = sideA1;
			}
			else
			{
				//trapezoid 2
				vec1 = sideA2 - sideA1;
				vec2 = sideB1 - sideA2;
				base = sideA1;
			}

		}
		if(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2)
		{
			F32 coef = gRandGen.randF();
			if(coef <= tris.trapezoidCoef)
			{
				//trapezoid 1
				vec1 = sideC1 - sideB1;
				vec2 = sideC2 - sideC1;
				base = sideB1;
			}
			else
			{
				//trapezoid 2
				vec1 = sideB2 - sideB1;
				vec2 = sideC1 - sideB2;
				base = sideB1;
			}
		}
		if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)
		{
			F32 coef = gRandGen.randF();
			if(coef <= tris.trapezoidCoef)
			{
				//trapezoid 1
				vec1 = sideA1 - sideC1;
				vec2 = sideA2 - sideA1;
				base = sideC1;
			}
			else
			{
				//trapezoid 2
				vec1 = sideC2 - sideC1;
				vec2 = sideA1 - sideC2;
				base = sideC1;
			}
		}
	}
	F32 K1 = rand() % 1000 + 1;
	F32 K2 = rand() % 1000 + 1;

	if(K2 <= K1)
		*out = base + (vec1 * (K1 / 1000)) + (vec2 * (K2 / 1000));
	else
		*out = base + (vec1 * (1-(K1 / 1000))) + (vec2 * (1-(K2 / 1000)));
}

void IPSEmitterHelper::uniformPointInTriangle(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F* out)
{
	PROFILE_SCOPE(IPSCoruniformPointInTriangle2);
	Point3F vec1;
	Point3F vec2;
	Point3F base;
	if(tris.v1Inside && !tris.v2Inside && !tris.v3Inside)
	{
		vec1 = (p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ) )) - p1;
		vec2 = (p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) )) - (p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ) ));
		base = p1;
	}
	if(!tris.v1Inside && tris.v2Inside && !tris.v3Inside)
	{
		vec1 = (p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ) )) - p2;
		vec2 = (p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ) )) - (p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ) ));
		base = p2;
	}
	if(!tris.v1Inside && !tris.v2Inside && tris.v3Inside)
	{
		vec1 = (p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) )) - p3;
		vec2 = (p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ) )) - (p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ) ));
		base = p3;
	}
	F32 K1 = rand() % 1000 + 1;
	F32 K2 = rand() % 1000 + 1;

	if(K2 <= K1)
		*out = base + (vec1 * (K1 / 1000)) + (vec2 * (K2 / 1000));
	else
		*out = base + (vec1 * (1-(K1 / 1000))) + (vec2 * (1-(K2 / 1000)));
}

bool IPSEmitterHelper::uniformPointInInboundSphere(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F center, Point3F pos, F32 radius, Point3F* out)
{
	PROFILE_SCOPE(IPSCoreuniformPointInInboundSphere);
	Point3F V = mCross(p2-p1,p3-p1);
	Point3F N = V/sqrt(mDot(V,V));
	PlaneF plane = PlaneF(V, N);
	plane = PlaneF(p1,p2,p3);
	Point3F PlaneP = plane.project(center);
	U32 rand = gRandGen.randI();

	Point3F sphereProjCenter = pos+p1+((p2-p1)* tris.sphereProjp1CoeffX)+(((p3-p2))* tris.sphereProjp1CoeffY);
	sphereProjCenter = pos+PlaneP;
	Point3F sphereProjObjCenter = p1+((p2-p1)* tris.sphereProjp1CoeffX)+(((p3-p2))* tris.sphereProjp1CoeffY);
	sphereProjObjCenter = PlaneP;
	IPSCore::intersection inter = IPSCore::raySphereIntersection(center,radius,p1,sphereProjObjCenter);
	Point3F centVec1 = inter.intersection1 - sphereProjObjCenter;
	F32 projRadius = centVec1.len();
	Point3F centVec2 = mCross(centVec1,Point3F(plane.x,plane.y,plane.z));
	F32 relx = cos((F32)rand) * gRandGen.randF();
	F32 rely = sin((F32)rand) * gRandGen.randF();
	Point3F result = sphereProjObjCenter + (centVec1 * relx) + (centVec2 * rely);
	bool accepted = true;
	if(tris.sideAIntersect.count == 2)
		if(IPSCore::IsPointInArc(center,radius,
			(p1+((p2-p1)*tris.sideAIntersect.intersection1Coeff)),
			(p1+((p2-p1)*tris.sideAIntersect.intersection2Coeff)),
			result))
			accepted = false;
	if(tris.sideBIntersect.count == 2)
		if(IPSCore::IsPointInArc(center,radius,
			(p2+((p3-p2)*tris.sideBIntersect.intersection1Coeff)),
			(p2+((p3-p2)*tris.sideBIntersect.intersection2Coeff)),
			result))
			accepted = false;
	if(tris.sideCIntersect.count == 2)
		if(IPSCore::IsPointInArc(center,radius,
			(p3+((p1-p3)*tris.sideCIntersect.intersection1Coeff)),
			(p3+((p1-p3)*tris.sideCIntersect.intersection2Coeff)),
			result))
			accepted = false;
	if(accepted == false)
		return false;
	*out = result;
	return true;
}

bool IPSEmitterHelper::uniformPointInArc(PlaneF plane, Point3F chordStart, Point3F chordEnd, Point3F center, F32 radius, Point3F mirrorP, Point3F* out)
{
	PROFILE_SCOPE(IPSCoreuniformPointInArc);
	Point3F PlaneP = plane.project(center);

	F32 hoz = gRandGen.randF();
	F32 coeff = hoz;
	F32 h = IPSCore::ChordHeight(chordStart, chordEnd, plane, center, radius,coeff);
	if(h == -1)
		return false;
	F32 vert = gRandGen.randF();
	Point3F chord = chordEnd-chordStart;
	Point3F perpendicular = IPSCore::perpendicularVector(chord,plane);
	perpendicular.normalize();
	perpendicular *= h;
	perpendicular *= vert;
	if(IPSCore::SameSideOfLine(chordStart, chordEnd, chordStart+(chord*hoz)+perpendicular,mirrorP))
		perpendicular *= -1;
	Point3F proj1 = IPSCore::project(center+(chordStart+(chord*hoz)-center),chordStart+(chord*hoz)+(perpendicular));
	Point3F proj2 = IPSCore::project(center+((chordStart+(chord*hoz)+perpendicular)-center),chordStart+(chord*hoz)+perpendicular);
	Point3F vertVec = proj1-proj2;
	F32 vertLen = vertVec.len();
	F32 verth = vert*h;
	if(	vert*h > h)
		hoz = hoz;
	*out = chordStart+(chord*hoz)+(perpendicular);
	return true;
}

bool IPSEmitterHelper::uniformPointInArc(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F center, F32 radius, bool secondArc, Point3F* out)
{
	PROFILE_SCOPE(IPSCoreuniformPointInArc2);
	Point3F veca;
	Point3F vecb;
	Point3F vecc;
	Point3F sideA1 = p1 + (tris.sideAIntersect.intersection1Coeff * ( p2 - p1 ));
	Point3F sideA2 = p1 + (tris.sideAIntersect.intersection2Coeff * ( p2 - p1 ));
	Point3F sideB1 = p2 + (tris.sideBIntersect.intersection1Coeff * ( p3 - p2 ));
	Point3F sideB2 = p2 + (tris.sideBIntersect.intersection2Coeff * ( p3 - p2 ));
	Point3F sideC1 = p3 + (tris.sideCIntersect.intersection1Coeff * ( p1 - p3 ));
	Point3F sideC2 = p3 + (tris.sideCIntersect.intersection2Coeff * ( p1 - p3 ));
	Point3F chordStart, chordEnd;

	PlaneF plane = PlaneF(p1,p2,p3);
	Point3F PlaneP = plane.project(center);

	if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count != 2)
	{
		chordStart = sideA1;
		chordEnd = sideB2;
		if(secondArc)
		{
			chordStart = sideB1;
			chordEnd = sideA2;
		}
	}
	if(tris.sideAIntersect.count != 2 && tris.sideBIntersect.count == 2 && tris.sideCIntersect.count == 2)
	{
		chordStart = sideB1;
		chordEnd = sideC2;
		if(secondArc)
		{
			chordStart = sideC1;
			chordEnd = sideB2;
		}
	}
	if(tris.sideAIntersect.count == 2 && tris.sideBIntersect.count != 2 && tris.sideCIntersect.count == 2)
	{
		chordStart = sideC1;
		chordEnd = sideA2;
		if(secondArc)
		{
			chordStart = sideA1;
			chordEnd = sideC2;
		}
	}

	F32 hoz = gRandGen.randF();
	F32 coeff = hoz;
	F32 h = IPSCore::ChordHeight(chordStart, chordEnd, plane, center, radius,coeff);
	F32 vert = gRandGen.randF();
	Point3F chord = chordEnd-chordStart;
	Point3F perpendicular = IPSCore::perpendicularVector(chord,plane);
	perpendicular.normalize();
	perpendicular *= h;
	perpendicular *= vert;
	Point3F proj1 = IPSCore::project(center+(chordStart+(chord*hoz)-center),chordStart+(chord*hoz)+(perpendicular));
	Point3F proj2 = IPSCore::project(center+((chordStart+(chord*hoz)+perpendicular)-center),chordStart+(chord*hoz)+perpendicular);
	Point3F vertVec = proj1-proj2;
	F32 vertLen = vertVec.len();
	F32 verth = vert*h;
	if(	vert*h > h)
		hoz = hoz;
	*out = chordStart+(chord*hoz)+(perpendicular);
	return true;
}

bool IPSEmitterHelper::uniformPointInArc(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F center, F32 radius, Point3F* out)
{
	PROFILE_SCOPE(IPSCoreuniformPointInArc3);
	PlaneF plane = PlaneF(p1,p2,p3);
	Point3F PlaneP = plane.project(center);
	Point3F v1, v2;
	if(tris.sideAIntersect.count == 2)
	{
		v1 = p1+((p2-p1) * tris.sideAIntersect.intersection1Coeff);
		v2 = p1+((p2-p1) * tris.sideAIntersect.intersection2Coeff);
		F32 hoz = gRandGen.randF();
		F32 coeff = hoz;
		F32 h = IPSCore::ChordHeight(v1, v2, plane, PlaneP, radius,coeff);
		F32 vert = gRandGen.randF();
		Point3F chord = v2-v1;
		Point3F perpendicular = IPSCore::perpendicularVector(chord,plane);
		perpendicular.normalize();
		perpendicular *= h;
		perpendicular *= vert;
		Point3F proj1 = IPSCore::project(PlaneP+(v1+(chord*hoz)-PlaneP),v1+(chord*hoz)+(perpendicular));
		Point3F proj2 = IPSCore::project(PlaneP+((v1+(chord*hoz)+perpendicular)-PlaneP),v1+(chord*hoz)+perpendicular);
		Point3F vertVec = proj1-proj2;
		F32 vertLen = vertVec.len();
		F32 verth = vert*h;
		if(	vert*h > h)
			hoz = hoz;
		*out = v1+(chord*hoz)+(perpendicular);
		return true;
	}

	if(tris.sideBIntersect.count == 2)
	{
		v1 = p2+((p3-p2) * tris.sideBIntersect.intersection1Coeff);
		v2 = p2+((p3-p2) * tris.sideBIntersect.intersection2Coeff);
		F32 inBoundRadius = (PlaneP - v1).len();
		F32 hoz = gRandGen.randF();
		F32 coeff = hoz;
		F32 h = IPSCore::ChordHeight(v1, v2, plane, PlaneP, inBoundRadius,coeff);
		F32 vert = gRandGen.randF();
		Point3F chord = v2-v1;
		Point3F perpendicular = IPSCore::perpendicularVector(chord,plane);
		perpendicular.normalize();
		perpendicular *= h;
		perpendicular *= vert;
		Point3F proj1 = IPSCore::project(PlaneP+(v1+(chord*hoz)-PlaneP),v1+(chord*hoz)+(perpendicular));
		Point3F proj2 = IPSCore::project(PlaneP+((v1+(chord*hoz)+perpendicular)-PlaneP),v1+(chord*hoz)+perpendicular);
		Point3F vertVec = proj1-proj2;
		F32 vertLen = vertVec.len();
		F32 verth = vert*h;
		if(	vert*h > h)
			hoz = hoz;
		*out = v1+(chord*hoz)+(perpendicular);
		return true;
	}

	if(tris.sideCIntersect.count == 2)
	{
		v1 = p3+((p1-p3) * tris.sideCIntersect.intersection1Coeff);
		v2 = p3+((p1-p3) * tris.sideCIntersect.intersection2Coeff);
		F32 inBoundRadius = (PlaneP - v1).len();
		F32 hoz = gRandGen.randF();
		F32 coeff = hoz;
		F32 h = IPSCore::ChordHeight(v1, v2, plane, PlaneP, inBoundRadius,coeff);
		F32 vert = gRandGen.randF();
		Point3F chord = v2-v1;
		Point3F perpendicular = IPSCore::perpendicularVector(chord,plane);
		perpendicular.normalize();
		perpendicular *= h;
		perpendicular *= vert;
		Point3F proj1 = IPSCore::project(PlaneP+(v1+(chord*hoz)-PlaneP),v1+(chord*hoz)+(perpendicular));
		Point3F proj2 = IPSCore::project(PlaneP+((v1+(chord*hoz)+perpendicular)-PlaneP),v1+(chord*hoz)+perpendicular);
		Point3F vertVec = proj1-proj2;
		F32 vertLen = vertVec.len();
		F32 verth = vert*h;
		if(	vert*h > h)
			hoz = hoz;
		*out = v1+(chord*hoz)+(perpendicular);
		return true;
	}
	return false;
}