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

#include "Tween.h"

#include "T3D\gameBase\gameBase.h"

IMPLEMENT_CONOBJECT(Tween);

//-------------------------------------------
// Tween
//-------------------------------------------

Tween::Tween()
{
   mDuration = 0.f;
   mCurrentTime = 0.f;
   mTarget = NULL;
   mValueName = NULL;
   mValueTarget = 0.f;
   mEaseDirection = Ease::InOut;
   mEaseType = Ease::Linear;
   mState = TweenState::Idle;
}

bool Tween::onAdd()
{
   if(!Parent::onAdd())
      return false;
   mCurrentTime = 0;
   SetInitialValue();
   return true;
}

void Tween::onRemove()
{
   Parent::onRemove();
}

void Tween::initPersistFields()
{
   addField("Duration", TypeF32, Offset(mDuration, Tween), "");
   addField("Target", TYPEID<SimObject>(), Offset(mTarget, Tween), "");
   addField("ValueName", TYPEID<const char*>(), Offset(mValueName, Tween), "");
   addField("ValueTarget", TypeF32, Offset(mValueTarget, Tween), "");
   addField("EaseDirection", TypeS32, Offset(mEaseDirection, Tween), "");
   addField("EaseType", TypeS32, Offset(mEaseType, Tween), "");

   Parent::initPersistFields();
}

void Tween::interpolateTick(F32 delta)
{
}

void Tween::processTick()
{
}

void Tween::advanceTime(F32 time)
{
   if(mTarget && mTarget->isDeleted()){
      mTarget = NULL;
      return;
   }
   switch (mState)
   {
   case Tween::Idle:
      break;
   case Tween::Playing:
      mCurrentTime += time;
      if(mCurrentTime >= mDuration)
      {
         mCurrentTime = mDuration;
         SetValueByTime(mDuration);
         mState = Tween::Idle;
      }
      else
         SetValueByTime(mCurrentTime);
      break;
   case Tween::Paused:
      break;
   case Tween::PlayingReversed:
      mCurrentTime += time;
      if(mCurrentTime >= mDuration)
      {
         mCurrentTime = mDuration;
         SetValueByTime(0);
         mState = Tween::Idle;
      }
      else
         SetValueByTime(mDuration - mCurrentTime);
      break;
   default:
      break;
   }
}

void Tween::Play()
{
   mState = TweenState::Playing;
   SetInitialValue();
}

void Tween::Pause()
{
   mState = TweenState::Paused;
}

void Tween::Rewind()
{
   mCurrentTime = 0;
   SetValueByTime(0);
   SetInitialValue();
}

void Tween::Reverse()
{
   mState = TweenState::PlayingReversed;
   SetInitialValue();
}

void Tween::SetValueByTime(F32 time)
{
   EaseF ease = EaseF(mEaseDirection, mEaseType);
   // t: current time, b: beginning value, c: change in value, d: duration
   F32 t = time;
   F32 b = mInitialValue;
   F32 c = mValueTarget - mInitialValue;
   F32 d = mDuration;
   F32 newValue = ease.getValue(t,b,c,d);
   if(mTarget)
      SetTargetField(newValue);
   else
      SetGlobalField(newValue);
}

void Tween::SetTargetField(F32 value)
{
   int size = strlen(mValueName);
   char buffer[18];
   dSprintf(buffer, sizeof(buffer), "%f", value);
   if(size <= 0)
   {
      Con::errorf("ValueName not set, pausing Tween %s", getId());
      Pause();
      return;
   }

   switch (mValueName[0])
   {
      // Position BEGIN -----
   case 'x': 
   case 'X':
      if(size == 1)
      {
         SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
         if(obj)
         {
            Point3F pos = obj->getPosition();
            obj->setPosition(Point3F(value, pos.y, pos.z));
         }
         return;
      }
      break;
   case 'y':
   case 'Y':
      if(size == 1)
      {
         SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
         if(obj)
         {
            Point3F pos = obj->getPosition();
            obj->setPosition(Point3F(pos.x, value, pos.z));
         }
         return;
      }
      break;
   case 'z':
   case 'Z':
      if(size == 1)
      {
         SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
         if(obj)
         {
            Point3F pos = obj->getPosition();
            obj->setPosition(Point3F(pos.x, pos.y, value));
         }
         return;
      }
      break;
      // Position END -----
      // Rotation BEGIN -----
   case 'R':
   case 'r':
      switch (mValueName[1])
      {
      case 'x':
      case 'X':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               EulerF euler = obj->getTransform().toEuler();
               euler = Point3F(value, euler.y, euler.z);
               obj->setTransform(MatrixF(euler));
            }
            return;
         }
         break;
      case 'y':
      case 'Y':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               EulerF euler = obj->getTransform().toEuler();
               euler = Point3F(euler.x, value, euler.z);
               obj->setTransform(MatrixF(euler));
            }
            return;
         }
         break;
      case 'z':
      case 'Z':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               EulerF euler = obj->getTransform().toEuler();
               euler = Point3F(euler.x, euler.y, value);
               obj->setTransform(MatrixF(euler));
            }
            return;
         }
         break;
      default:
         break;
      }
      break;
      // Rotation END -----
      // Scale BEGIN -----
   case 's':
   case 'S':
      switch (mValueName[1])
      {
      case 'x':
      case 'X':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               VectorF scale = obj->getScale();
               obj->setScale(VectorF(value, scale.y, scale.z));
            }
            return;
         }
         break;
      case 'y':
      case 'Y':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               VectorF scale = obj->getScale();
               obj->setScale(VectorF(scale.x, value, scale.z));
            }
            return;
         }
         break;
      case 'z':
      case 'Z':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               VectorF scale = obj->getScale();
               obj->setScale(VectorF(scale.z, scale.y, value));
            }
            return;
         }
         break;
      default:
         break;
      }
      break;
      // Scale END -----
   default:
      break;
   }
   mTarget->setDataField(mValueName, NULL, buffer);
}

void Tween::SetGlobalField(F32 value)
{
   char buffer[6];
   dSprintf(buffer, sizeof(buffer), "%s", value);
   Con::setVariable(mValueName, buffer);
}

void Tween::SetInitialValue()
{
   int size = strlen(mValueName);
   if(size <= 0)
   {
      Con::errorf("ValueName not set, pausing Tween %s", getId());
      Pause();
      return;
   }

   switch (mValueName[0])
   {
      // Position BEGIN -----
   case 'x': 
   case 'X':
      if(size == 1)
      {
         SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
         if(obj)
         {
            mInitialValue = obj->getPosition().x;
         }
         return;
      }
      break;
   case 'y':
   case 'Y':
      if(size == 1)
      {
         SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
         if(obj)
         {
            mInitialValue = obj->getPosition().y;
         }
         return;
      }
      break;
   case 'z':
   case 'Z':
      if(size == 1)
      {
         SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
         if(obj)
         {
            mInitialValue = obj->getPosition().z;
         }
         return;
      }
      break;
      // Position END -----
      // Rotation BEGIN -----
   case 'R':
   case 'r':
      switch (mValueName[1])
      {
      case 'x':
      case 'X':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               mInitialValue = obj->getTransform().toEuler().x;
            }
            return;
         }
         break;
      case 'y':
      case 'Y':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               mInitialValue = obj->getTransform().toEuler().y;
            }
            return;
         }
         break;
      case 'z':
      case 'Z':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               mInitialValue = obj->getTransform().toEuler().z;
            }
            return;
         }
         break;
      default:
         break;
      }
      break;
      // Rotation END -----
      // Scale BEGIN -----
   case 's':
   case 'S':
      switch (mValueName[1])
      {
      case 'x':
      case 'X':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               mInitialValue = obj->getScale().x;
            }
            return;
         }
         break;
      case 'y':
      case 'Y':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               mInitialValue = obj->getScale().y;
            }
            return;
         }
         break;
      case 'z':
      case 'Z':
         if(size == 2)
         {
            SceneObject* obj = dynamic_cast<SceneObject*>(mTarget);
            if(obj)
            {
               mInitialValue = obj->getScale().z;
            }
            return;
         }
         break;
      default:
         break;
      }
      break;
      // Scale END -----
   default:
      break;
   }
}

IMPLEMENT_CALLBACK(Tween, onFinished, void, (), (), "");

DefineEngineMethod(Tween, Play, void, (),,"")
{
   object->Play();
}

DefineEngineMethod(Tween, Pause, void, (),,"")
{
   object->Pause();
}

DefineEngineMethod(Tween, Rewind, void, (),,"")
{
   object->Rewind();
}

DefineEngineMethod(Tween, Reverse, void, (),,"")
{
   object->Reverse();
}