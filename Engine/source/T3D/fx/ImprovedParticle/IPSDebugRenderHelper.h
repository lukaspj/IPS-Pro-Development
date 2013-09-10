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

#ifndef _IPSDebugRenderHelper_H_
#define _IPSDebugRenderHelper_H_
#include "gfx/gfxDrawUtil.h"

#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "IPSCore.h"

class IPSDebugRenderHelper{
private:
	ShapeBase* SS;
	TSStatic* TS;
	SimObject* SB;
public:
	IPSDebugRenderHelper(StringTableEntry object);
	// Draws a triangle
	void drawFace(IPSCore::face object, bool intersections, bool faces, bool isRadius, F32 radius, Point3F center, bool shadeByCoverage);
	// Draws a sphere
	void drawSphere(Point3F center, F32 radius);
	// Draws a vector
	static void drawVector(Point3F V1, Point3F V2, ColorI col);
	// Draws intersections
	void drawIntersections(IPSCore::face tris);
};

class TimedDebugVector : public SceneObject
{
   typedef SceneObject Parent;

   enum MaskBits 
   {
      TransformMask = Parent::NextFreeMask << 0,
      UpdateMask    = Parent::NextFreeMask << 1,
      NextFreeMask  = Parent::NextFreeMask << 2
   };

public:

   TimedDebugVector();

   DECLARE_CONOBJECT( TimedDebugVector );

   // ConsoleObject
   virtual bool onAdd();
   virtual void onRemove();
   static void initPersistFields();
   virtual void inspectPostApply();   
   // Override this so that we can dirty the network flag when it is called
   void setTransform( const MatrixF &mat );

   // NetObject
   virtual U32 packUpdate( NetConnection *conn, U32 mask, BitStream *stream );
   virtual void unpackUpdate( NetConnection *conn, BitStream *stream );

   // SceneObject
   virtual void prepRenderImage( SceneRenderState *state );
   //void renderObject( ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance *mi );

   Point3F mStart;
   Point3F mEnd;
   ColorI mColor;
   U32 mDuration;
   U32 mInitialTime;
   bool mDead;
};
#endif