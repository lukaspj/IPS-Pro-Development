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

#ifndef _GUIBITMAPCTRL_H_
#define _GUIBITMAPCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#include "../SpellCooldowns.h"
#include "../SpellManager.h"
#include "../Spell.h"

/// Renders a bitmap.
class GuiSpellCtrl : public GuiControl
{
public:

   typedef GuiControl Parent;

protected:

   /// Name of the bitmap file.  If this is 'texhandle' the bitmap is not loaded
   /// from a file but rather set explicitly on the control.
   String mBitmapName;

   /// Loaded texture.
   GFXTexHandle mTextureObject;

   Point2I mStartPoint;

   CooldownManager* mCDManager;
   SpellData* mSpellData;

   static bool setSpellData( void *object, const char *index, const char *data );
   static const char *getSpellData( void *obj, const char *data );

public:

   GuiSpellCtrl();
   static void initPersistFields();

   void setBitmap(const char *name,bool resize = false);
   void setBitmapHandle(GFXTexHandle handle, bool resize = false);

   void setCooldownManager(CooldownManager* CDM) { mCDManager = CDM; };
   void setCooldownManager(SpellManager* MGR) { mCDManager = MGR->getCDManager(); };
   void setSpellData(SpellData* dat) { mSpellData = dat; };

   // GuiControl.
   bool onWake();
   void onSleep();
   void inspectPostApply();

   void updateSizing();

   void onRender(Point2I offset, const RectI &updateRect);
   void setValue(S32 x, S32 y);

   // Fields---------
   String mSpellDataName;

   DECLARE_CONOBJECT( GuiSpellCtrl );
   DECLARE_CATEGORY( "Gui Images" );
   DECLARE_DESCRIPTION( "A control that displays a single, static image from a file.\n"
   "The bitmap can either be tiled or stretched inside the control." );
};

#endif
