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

#include "IPSEmitterHelper.h"
#include "gfx/gfxDrawUtil.h"
#include "platform/platform.h"
#include "console/consoleTypes.h"

#include "T3D\fx\particleEmitter.h"

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
				vec2 = sideA1 - p3;
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
		if(	vert*h > h)
			hoz = hoz;
		*out = v1+(chord*hoz)+(perpendicular);
		return true;
	}
	return false;
}
/*
//-----------------------------------------------------------------------------
// alloc PrimitiveBuffer
// The datablock allocates this static index buffer because it's the same
// for all of the emitters - each particle quad uses the same index ordering
//-----------------------------------------------------------------------------
void IPSEmitterHelper::allocPrimBuffer( Vector<ParticleData*> &particleDataBlocks, U32 &partListInitSize, GFXPrimitiveBufferHandle &primBuff, U32 minPartLife, S32 overrideSize )
{
	// calculate particle list size
	AssertFatal(particleDataBlocks.size() > 0, "Error, no particles found." );
	U32 maxPartLife = particleDataBlocks[0]->lifetimeMS + particleDataBlocks[0]->lifetimeVarianceMS;
	for (S32 i = 1; i < particleDataBlocks.size(); i++)
	{
		U32 mpl = particleDataBlocks[i]->lifetimeMS + particleDataBlocks[i]->lifetimeVarianceMS;
		if (mpl > maxPartLife)
			maxPartLife = mpl;
	}

	partListInitSize = maxPartLife / minPartLife;
	partListInitSize += 8; // add 8 as "fudge factor" to make sure it doesn't realloc if it goes over by 1

	// if override size is specified, then the emitter overran its buffer and needs a larger allocation
	if( overrideSize != -1 )
	{
		partListInitSize = overrideSize;
	}

	// create index buffer based on that size
	U32 indexListSize = partListInitSize * 6; // 6 indices per particle
	U16 *indices = new U16[ indexListSize ];

	for( U32 i=0; i<partListInitSize; i++ )
	{
		// this index ordering should be optimal (hopefully) for the vertex cache
		U16 *idx = &indices[i*6];
		volatile U32 offset = i * 4;  // set to volatile to fix VC6 Release mode compiler bug
		idx[0] = 0 + offset;
		idx[1] = 1 + offset;
		idx[2] = 3 + offset;
		idx[3] = 1 + offset;
		idx[4] = 3 + offset;
		idx[5] = 2 + offset;
	}


	U16 *ibIndices;
	GFXBufferType bufferType = GFXBufferTypeStatic;

#ifdef TORQUE_OS_XENON
	// Because of the way the volatile buffers work on Xenon this is the only
	// way to do this.
	bufferType = GFXBufferTypeVolatile;
#endif
	primBuff.set( GFX, indexListSize, 0, bufferType );
	primBuff.lock( &ibIndices );
	dMemcpy( ibIndices, indices, indexListSize * sizeof(U16) );
	primBuff.unlock();

	delete [] indices;
}

bool IPSEmitterHelper::_setAlignDirection( void *object, const char *index, const char *data )
{
	ParticleEmitterData *p = static_cast<ParticleEmitterData*>( object );

	Con::setData( TypePoint3F, &p->alignDirection, 0, 1, &data );
	p->alignDirection.normalizeSafe();

	// we already set the field
	return false;
}

//-----------------------------------------------------------------------------
// getCollectiveColor
//-----------------------------------------------------------------------------
ColorF IPSEmitterHelper::getCollectiveColor(S32 n_parts, Particle part_list_head)
{
	U32 count = 0;
	ColorF color = ColorF(0.0f, 0.0f, 0.0f);

	count = n_parts;
	for( Particle* part = part_list_head.next; part != NULL; part = part->next )
	{
		color += part->color;
	}

	if(count > 0)
	{
		color /= F32(count);
	}

	//if(color.red == 0.0f && color.green == 0.0f && color.blue == 0.0f)
	//	color = color;

	return color;
}

//-----------------------------------------------------------------------------
// setSizes
//-----------------------------------------------------------------------------
void IPSEmitterHelper::setSizes( F32 *sizeList, F32 *sizes )
{
	for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
	{
		sizes[i] = sizeList[i];
	}
}

//-----------------------------------------------------------------------------
// setColors
//-----------------------------------------------------------------------------
void IPSEmitterHelper::setColors( ColorF *colorList, ColorF *colors )
{
	for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
	{
		colors[i] = colorList[i];
	}
}

//-----------------------------------------------------------------------------
// Set up particle for billboard style render
//-----------------------------------------------------------------------------
void IPSEmitterHelper::setupBillboard( Particle *part,
									 Point3F *basePts,
									 const MatrixF &camView,
									 const ColorF &ambientColor,
									 ParticleVertexType *lVerts,
									 const F32 AgedSpinToRadians,
									 F32 ambientFactor)
{
	F32 width     = part->size * 0.5f;
	F32 spinAngle = part->spinSpeed * part->currentAge * AgedSpinToRadians;

	F32 sy, cy;
	mSinCos(spinAngle, sy, cy);

	const F32 ambientLerp = mClampF( ambientFactor, 0.0f, 1.0f );
	ColorF partCol = mLerp( part->color, ( part->color * ambientColor ), ambientLerp );

	// fill four verts, use macro and unroll loop
#define fillVert(){ \
	lVerts->point.x = cy * basePts->x - sy * basePts->z;  \
	lVerts->point.y = 0.0f;                                \
	lVerts->point.z = sy * basePts->x + cy * basePts->z;  \
	camView.mulV( lVerts->point );                        \
	lVerts->point *= width;                               \
	lVerts->point += part->pos;                           \
	lVerts->color = partCol; } \

	// Here we deal with UVs for animated particle (billboard)
	if (part->dataBlock->animateTexture)
	{ 
		S32 fm = (S32)(part->currentAge*(1.0/1000.0)*part->dataBlock->framesPerSec);
		U8 fm_tile = part->dataBlock->animTexFrames[fm % part->dataBlock->numFrames];
		S32 uv[4];
		uv[0] = fm_tile + fm_tile/part->dataBlock->animTexTiling.x;
		uv[1] = uv[0] + (part->dataBlock->animTexTiling.x + 1);
		uv[2] = uv[1] + 1;
		uv[3] = uv[0] + 1;

		fillVert();
		// Here and below, we copy UVs from particle datablock's current frame's UVs (billboard)
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[0]];
		++lVerts;
		++basePts;

		fillVert();
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[1]];
		++lVerts;
		++basePts;

		fillVert();
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[2]];
		++lVerts;
		++basePts;

		fillVert();
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[3]];
		++lVerts;
		++basePts;

		return;
	}

	fillVert();
	// Here and below, we copy UVs from particle datablock's texCoords (billboard)
	lVerts->texCoord = part->dataBlock->texCoords[0];
	++lVerts;
	++basePts;

	fillVert();
	lVerts->texCoord = part->dataBlock->texCoords[1];
	++lVerts;
	++basePts;

	fillVert();
	lVerts->texCoord = part->dataBlock->texCoords[2];
	++lVerts;
	++basePts;

	fillVert();
	lVerts->texCoord = part->dataBlock->texCoords[3];
	++lVerts;
	++basePts;
}

//-----------------------------------------------------------------------------
// Set up oriented particle
//-----------------------------------------------------------------------------
void IPSEmitterHelper::setupOriented( Particle *part,
									const Point3F &camPos,
									const ColorF &ambientColor,
									ParticleVertexType *lVerts,
									bool orientOnVelocity,
									F32 ambientFactor)
{
	Point3F dir;

	if( orientOnVelocity )
	{
		// don't render oriented particle if it has no velocity
		if( part->vel.magnitudeSafe() == 0.0 ) return;
		dir = part->vel;
	}
	else
	{
		dir = part->orientDir;
	}

	Point3F dirFromCam = part->pos - camPos;
	Point3F crossDir;
	mCross( dirFromCam, dir, &crossDir );
	crossDir.normalize();
	dir.normalize();

	F32 width = part->size * 0.5f;
	dir *= width;
	crossDir *= width;
	Point3F start = part->pos - dir;
	Point3F end = part->pos + dir;

	const F32 ambientLerp = mClampF( ambientFactor, 0.0f, 1.0f );
	ColorF partCol = mLerp( part->color, ( part->color * ambientColor ), ambientLerp );

	// Here we deal with UVs for animated particle (oriented)
	if (part->dataBlock->animateTexture)
	{ 
		// Let particle compute the UV indices for current frame
		S32 fm = (S32)(part->currentAge*(1.0f/1000.0f)*part->dataBlock->framesPerSec);
		U8 fm_tile = part->dataBlock->animTexFrames[fm % part->dataBlock->numFrames];
		S32 uv[4];
		uv[0] = fm_tile + fm_tile/part->dataBlock->animTexTiling.x;
		uv[1] = uv[0] + (part->dataBlock->animTexTiling.x + 1);
		uv[2] = uv[1] + 1;
		uv[3] = uv[0] + 1;

		lVerts->point = start + crossDir;
		lVerts->color = partCol;
		// Here and below, we copy UVs from particle datablock's current frame's UVs (oriented)
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[0]];
		++lVerts;

		lVerts->point = start - crossDir;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[1]];
		++lVerts;

		lVerts->point = end - crossDir;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[2]];
		++lVerts;

		lVerts->point = end + crossDir;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[3]];
		++lVerts;

		return;
	}

	lVerts->point = start + crossDir;
	lVerts->color = partCol;
	// Here and below, we copy UVs from particle datablock's texCoords (oriented)
	lVerts->texCoord = part->dataBlock->texCoords[0];
	++lVerts;

	lVerts->point = start - crossDir;
	lVerts->color = partCol;
	lVerts->texCoord = part->dataBlock->texCoords[1];
	++lVerts;

	lVerts->point = end - crossDir;
	lVerts->color = partCol;
	lVerts->texCoord = part->dataBlock->texCoords[2];
	++lVerts;

	lVerts->point = end + crossDir;
	lVerts->color = partCol;
	lVerts->texCoord = part->dataBlock->texCoords[3];
	++lVerts;
}

void IPSEmitterHelper::setupAligned( const Particle *part, 
								   const ColorF &ambientColor,
								   ParticleVertexType *lVerts,
								   Point3F alignDirection,
								   const F32 AgedSpinToRadians,
								   F32 ambientFactor)
{
	// The aligned direction will always be normalized.
	Point3F dir = alignDirection;

	// Find a right vector for this particle.
	Point3F right;
	if (mFabs(dir.y) > mFabs(dir.z))
		mCross(Point3F::UnitZ, dir, &right);
	else
		mCross(Point3F::UnitY, dir, &right);
	right.normalize();

	// If we have a spin velocity.
	if ( !mIsZero( part->spinSpeed ) )
	{
		F32 spinAngle = part->spinSpeed * part->currentAge * AgedSpinToRadians;

		// This is an inline quaternion vector rotation which
		// is faster that QuatF.mulP(), but generates different
		// results and hence cannot replace it right now.

		F32 sin, qw;
		mSinCos( spinAngle * 0.5f, sin, qw );
		F32 qx = dir.x * sin;
		F32 qy = dir.y * sin;
		F32 qz = dir.z * sin;

		F32 vx = ( right.x * qw ) + ( right.z * qy ) - ( right.y * qz );
		F32 vy = ( right.y * qw ) + ( right.x * qz ) - ( right.z * qx );
		F32 vz = ( right.z * qw ) + ( right.y * qx ) - ( right.x * qy );
		F32 vw = ( right.x * qx ) + ( right.y * qy ) + ( right.z * qz );

		right.x = ( qw * vx ) + ( qx * vw ) + ( qy * vz ) - ( qz * vy );
		right.y = ( qw * vy ) + ( qy * vw ) + ( qz * vx ) - ( qx * vz );
		right.z = ( qw * vz ) + ( qz * vw ) + ( qx * vy ) - ( qy * vx );
	}

	// Get the cross vector.
	Point3F cross;
	mCross(right, dir, &cross);

	F32 width = part->size * 0.5f;
	right *= width;
	cross *= width;
	Point3F start = part->pos - right;
	Point3F end = part->pos + right;

	const F32 ambientLerp = mClampF( ambientFactor, 0.0f, 1.0f );
	ColorF partCol = mLerp( part->color, ( part->color * ambientColor ), ambientLerp );

	// Here we deal with UVs for animated particle
	if (part->dataBlock->animateTexture)
	{ 
		// Let particle compute the UV indices for current frame
		S32 fm = (S32)(part->currentAge*(1.0f/1000.0f)*part->dataBlock->framesPerSec);
		U8 fm_tile = part->dataBlock->animTexFrames[fm % part->dataBlock->numFrames];
		S32 uv[4];
		uv[0] = fm_tile + fm_tile/part->dataBlock->animTexTiling.x;
		uv[1] = uv[0] + (part->dataBlock->animTexTiling.x + 1);
		uv[2] = uv[1] + 1;
		uv[3] = uv[0] + 1;

		lVerts->point = start + cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[0]];
		++lVerts;

		lVerts->point = start - cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[1]];
		++lVerts;

		lVerts->point = end - cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[2]];
		++lVerts;

		lVerts->point = end + cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->animTexUVs[uv[3]];
		++lVerts;
	}
	else
	{
		// Here and below, we copy UVs from particle datablock's texCoords
		lVerts->point = start + cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->texCoords[0];
		++lVerts;

		lVerts->point = start - cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->texCoords[1];
		++lVerts;

		lVerts->point = end - cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->texCoords[2];
		++lVerts;

		lVerts->point = end + cross;
		lVerts->color = partCol;
		lVerts->texCoord = part->dataBlock->texCoords[3];
		++lVerts;
	}
}
*/