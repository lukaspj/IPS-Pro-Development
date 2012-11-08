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

#ifndef _IPSEmitterHelper_H_
#define _IPSEmitterHelper_H_
#include "gfx/gfxDrawUtil.h"

#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "IPSCore.h"

class IPSEmitterHelper{
public:
	static void uniformPointInTriangle(Point3F p1, Point3F p2, Point3F p3, Point3F* out);
	static void uniformPointInTriangle(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F* out);
	static void uniformPointInTrapezoid(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F* out);
	static void uniformPointInTrapezoid(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F p4, Point3F* out);
	static bool uniformPointInInboundSphere(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F center, Point3F pos, F32 radius, Point3F* out);
	static bool uniformPointInArc(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F center, F32 radius, Point3F* out);
	static bool uniformPointInArc(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F center, F32 radius, bool secondArc, Point3F* out);
	static bool uniformPointInArc(PlaneF plane, Point3F chordStart, Point3F chordEnd, Point3F center, F32 radius, Point3F mirrorP, Point3F* out);
};
#endif