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

#ifndef TWEEN_ENGINE_H
#define TWEEN_ENGINE_H

#include "platform/platform.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "console/simObject.h"
#include "core\iTickable.h"
#include "math/mConstants.h"
#include "math/mMathFn.h"
#include "math/mEase.h"

class Tween : public SimObject, public ITickable
{
   typedef SimObject Parent;

   enum TweenState{
      Idle,
      Playing,
      Paused,
      PlayingReversed
   };

public:
   Tween();
   void SetValueByTime(F32 time);
   void SetTargetField(F32 value);
   void SetGlobalField(F32 value);
   void SetInitialValue();

   void Play();
   void Reverse();
   void Rewind();
   void Pause();
   
   //-------------------------------------------
   // SimObject
   //-------------------------------------------
   virtual bool onAdd();
   virtual void onRemove();
   static void initPersistFields();
   
   //-------------------------------------------
   // ITickable
   //-------------------------------------------
   virtual void interpolateTick( F32 delta );
   virtual void processTick();
   virtual void advanceTime( F32 timeDelta );

   // Fields ------------------------------------
   F32 mDuration;
   F32 mCurrentTime;
   SimObject* mTarget;
   const char* mValueName;
   F32 mValueTarget;
   F32 mInitialValue;
   Ease::enumDirection mEaseDirection;
   Ease::enumType mEaseType;
   TweenState mState;

   DECLARE_CALLBACK(void, onFinished, ());
   DECLARE_CONOBJECT(Tween);
};

#endif // TWEEN_ENGINE_H
