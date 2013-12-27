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

#include "CubeImpact.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include <string>

#include "T3D\fx\ImprovedParticle\IPSCore.h"

IMPLEMENT_CONOBJECT(CubeImpact);

//---------------------------------------------------------------
// Constructor
//---------------------------------------------------------------
CubeImpact::CubeImpact()
{
   mCenter = Point3F(0);
   mSize = 0.0f;
   mTypeMask = 0;
}

//----------------------- SimObject -------------------------
//---------------------------------------------------------------
// InitPersistFields
//--------------------------------------------------------------
void CubeImpact::initPersistFields()
{
   addField("Center", TypePoint3F, Offset(mCenter, CubeImpact));
   addField("Size", TypeF32, Offset(mSize, CubeImpact));
   addField("TypeMask", TypeS32, Offset(mTypeMask, CubeImpact));

   Parent::initPersistFields();
}

//---------------------------------------------------------------
// OnAdd
//---------------------------------------------------------------
bool CubeImpact::onAdd()
{

   if(!SimObject::onAdd())
      return false;
   return true;
}

//----------------------- ITickable -------------------------
void CubeImpact::advanceTime( F32 timeDelta ) 
{
   if(isProperlyAdded())
   {
      gServerContainer.initCubeSearch(mCenter, mSize, mTypeMask);
      U32 objectId = gServerContainer.containerSearchNext();
      while(objectId != 0)
      {
         doImpact(SourceObject, objectId);
         objectId = gServerContainer.containerSearchNext();
      }
      safeDeleteObject();
   }
}