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

#include "AOEPick.h"
#include "../SpellDecalManager.h"
#include "../Spell.h"
#include "../SpellManager.h"

void AOEPick::PickPosition(void* caller, SpellData* spellDat, SceneObject* Obj, U32 mask, PickPositionCallback callback)
{
   NetConnection* client = ((SpellManager*)caller)->mClient;
   ShapeBase* scObj = dynamic_cast<ShapeBase*>(Obj);
   SpellDecalManager* SDM = new SpellDecalManager();
   SDM->mDataBlock = spellDat->mDecalManagerData;
   SDM->mCallback = callback;
   SDM->caller = caller;
   if(scObj)
      SDM->mDecalController = scObj;
   if(!SDM->registerObject())
   {
      Con::errorf("!SDM->registerObject()");
      delete SDM;
      callback(caller, Point3F(0), SpellPickError::TechnicalError);
      return;
   }
   SDM->SpawnDecal();
   client->objectLocalScopeAlways(SDM);
}

void AOEPick::PickObject(void* caller, SpellData* spellDat, SceneObject* Obj, U32 mask, PickObjectCallback callback)
{
   ShapeBase* scObj = dynamic_cast<ShapeBase*>(Obj);
   MatrixF mat;
   //Eyepoint = position, eyeVector = forwardvector
   if(scObj)
      scObj->getEyeTransform(&mat);
   else
      mat = Obj->getTransform();
   VectorF eyeVector = mat.getForwardVector();
   Point3F start = mat.getPosition();
   //F32 range = spell->getRange();
   F32 range = 100;
   VectorF rayDirection = eyeVector * range;
   VectorF end = start + rayDirection;
   RayInfo rInfo;
   bool hit = gServerContainer.castRay(start,end,mask,&rInfo);
   if(hit)
   {
      if(rInfo.object){
         callback(caller, rInfo.object, SpellPickError::Success);
      }
      else
         callback(caller, NULL, SpellPickError::NotFound);
   }
   else{
      callback(caller, NULL, SpellPickError::NotFound);
   }
}