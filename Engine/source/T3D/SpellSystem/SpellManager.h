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

#ifndef SPELL_MANAGER_H
#define SPELL_MANAGER_H

#include "Spell.h"
#include "SpellTarget.h"
#include "SpellSystemInterfaces.h"
#include "SpellCooldowns.h"
#include "console/consoleTypes.h"
#include "core\iTickable.h"
#include "console\simDatablock.h"

class SpellManager : public SimObject, public virtual ITickable{
   typedef SimObject Parent;

   enum SpellState{
      Idle,
      Initializing,
      AwaitingTarget,
      PreChannel,
      Channel,
      PostChannel,
      PreCast,
      Cast,
      PostCast
   };

public:
   //--------------------------------------------
   // ITickable
   //--------------------------------------------
   virtual void interpolateTick( F32 delta );
   virtual void processTick();
   virtual void advanceTime( F32 timeDelta );

   //--------------------------------------------
   // SimObject
   //--------------------------------------------
   virtual bool onAdd();
   virtual void onRemove();
   static void initPersistFields();

   //--------------------------------------------
   // SpellManager
   //--------------------------------------------
   SpellManager();
   bool BeginCast(SpellData *spellDat);
   void setTarget(SpellTarget target);
   void findTarget();
   void breakCasting();
   CooldownManager* getCDManager() { return mCDManager; };
   void setCDManager(CooldownManager* CDM) { mCDManagerSynced = false; mCDManager = CDM; };
   U8 getProgress() { return mProgress; };
   F32 getProgressPercent() { return 255.f/(F32)mProgress; };

   static void PickCallback(void* _this, SceneObject* obj, SpellPickError ReturnCode);
   static void PickCallback(void* _this, Point3F pos, SpellPickError ReturnCode);

   static bool _ReadOnly( void *object, const char *index, const char *data );

   //--------------------------------------------
   // Fields
   //--------------------------------------------
   F32			mTimeSpent;
   U8          mProgress;
   SpellData*	mCurSpellData;
   Spell*		mCurSpell;
   SpellState	mSpellState;
   NetConnection* mClient;
   CooldownManager* mCDManager;
   bool mCDManagerSynced;

   SceneObject* mControllingObject;

   //--------------------------------------------
   // Callbacks
   //--------------------------------------------
   //DECLARE_CALLBACK(void, onPreCast, (SimObjectId dID, SimObjectId sID));

   DECLARE_CONOBJECT(SpellManager);

   DECLARE_CALLBACK(void, onOOM, (SimDataBlock* spellID));
   DECLARE_CALLBACK(void, onOutOfRange, (SimDataBlock* spellID));
   DECLARE_CALLBACK(void, onNoTarget, (SimDataBlock* spellID));
   DECLARE_CALLBACK(void, onCooldown, (SimDataBlock* spellID, U32 TimeLeft));
   DECLARE_CALLBACK(void, onTargetNotFound, (SimDataBlock* spellID));
   DECLARE_CALLBACK(void, onAlreadyCastingSpell, (SimDataBlock* spellID));
   DECLARE_CALLBACK(void, onTechnicalError, (SimDataBlock* spellID));
};

#endif // SPELL_MANAGER_H
