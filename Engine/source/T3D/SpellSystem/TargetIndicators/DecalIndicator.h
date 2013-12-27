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

#ifndef DECAL_INDICATOR_H
#define DECAL_INDICATOR_H

#include "console\consoleTypes.h"
#include "console\simDatablock.h"
#include "TargetIndicator.h"
#include "T3D\decal\decalManager.h"
#include "../SpellTarget.h"
#include "core\iTickable.h"

class DecalIndicatorData : public SimDataBlock, public TargetIndicatorData
{
   typedef SimDataBlock Parent;

public:
   //--------------------------------------------
   // SimDataBlock
   //--------------------------------------------
   bool onAdd();
   void onRemove();
   void packData(BitStream*);
   void unpackData(BitStream*);
   bool preload(bool server, String &errorStr);
   static void initPersistFields();

   //--------------------------------------------
   // DecalIndicatorData
   //--------------------------------------------
   DecalIndicatorData();
   ~DecalIndicatorData();

   virtual TargetIndicator* getIndicator();

   //------- Fields -------
   DecalData* mDecalData;

   DECLARE_CONOBJECT(DecalIndicatorData);
};

class DecalIndicator : public TargetIndicator
{
   //--------------------------------------------
   // TargetIndicator
   //--------------------------------------------
   void Initialize(SpellTarget target);
   void Dispose();
   void Update(F32 Delta, SpellTarget target);
   void Update(SpellTarget target);
   void SetDataBlock(TargetIndicatorDataHost* data) { mDataBlock = (DecalIndicatorData*)data; };
   bool isInitialized() { return mInitialized; };

   //--------------------------------------------
   // DecalIndicator
   //--------------------------------------------
public:
   DecalIndicator();
   ~DecalIndicator();

   //--------------------------------------------
   // Fields
   //--------------------------------------------
   DecalIndicatorData* mDataBlock;
   DecalInstance*		mDecalInstance;
   bool				mInitialized;
};

#endif // DECAL_INDICATOR_H
