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

#ifndef _IPSEmitterHelper_H_
#define _IPSEmitterHelper_H_
#include "gfx/gfxDrawUtil.h"

#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "IPSCore.h"

#ifndef _GFXVERTEXBUFFER_H_
#include "gfx/gfxVertexBuffer.h"
#endif

#ifndef _PARTICLE_H_
#include "T3D/fx/particle.h"
#endif

class IPSEmitterHelper{
public:
	// Get a uniformly distributed point in a triangle
	static void uniformPointInTriangle(Point3F p1, Point3F p2, Point3F p3, Point3F* out);
	// Get a uniformly distributed point in a triangle
	static void uniformPointInTriangle(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F* out);
	// Get a uniformly distributed point in a trapezoid
	static void uniformPointInTrapezoid(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F* out);
	// Get a uniformly distributed point in a trapezoid
	static void uniformPointInTrapezoid(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F p4, Point3F* out);
	// Get a uniformly distributed point in an inbound sphere
	static bool uniformPointInInboundSphere(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F center, Point3F pos, F32 radius, Point3F* out);
	// Get a uniformly distributed point in an arch
	static bool uniformPointInArc(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F center, F32 radius, Point3F* out);
	// Get a uniformly distributed point in an arch
	static bool uniformPointInArc(IPSCore::face tris, Point3F p1, Point3F p2, Point3F p3, Point3F center, F32 radius, bool secondArc, Point3F* out);
	// Get a uniformly distributed point in an arch
	static bool uniformPointInArc(PlaneF plane, Point3F chordStart, Point3F chordEnd, Point3F center, F32 radius, Point3F mirrorP, Point3F* out);
	
	//--------------------------------------------
	// ParticleEmitter
	//--------------------------------------------
	
	#if defined(TORQUE_OS_XENON)
		typedef GFXVertexPCTT ParticleVertexType;
	#else
		typedef GFXVertexPCT ParticleVertexType;
	#endif

	static void allocPrimBuffer( Vector<ParticleData*> &particleDataBlocks, U32 &partListInitSize, GFXPrimitiveBufferHandle &primBuff, U32 minPartLife, S32 overrideSize = -1 );
	static bool _setAlignDirection( void *object, const char *index, const char *data );
	ColorF getCollectiveColor(S32 n_parts, Particle* part_list_head);
	/// Sets sizes of particles based on sizelist provided
	/// @param   sizeList   List of sizes
	static void setSizes( F32 *sizeList, F32 *sizes );

	/// Sets colors for particles based on color list provided
	/// @param   colorList   List of colors
	static void setColors( ColorF *colorList, ColorF *colors );
	
	static inline void setupBillboard( Particle *part,
		Point3F *basePts,
		const MatrixF &camView,
		const ColorF &ambientColor,
		ParticleVertexType *lVerts,
		const F32 AgedSpinToRadians,
		F32 ambientFactor );

	static inline void setupOriented( Particle *part,
		const Point3F &camPos,
		const ColorF &ambientColor,
		ParticleVertexType *lVerts,
		bool orientOnVelocity,
		F32 ambientFactor );

	static inline void setupAligned(  const Particle *part, 
		const ColorF &ambientColor,
		ParticleVertexType *lVerts,
		Point3F alignDirection,
		const F32 AgedSpinToRadians,
		F32 ambientFactor );
};
#endif