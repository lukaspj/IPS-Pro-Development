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

#include "platform/platform.h"
#include "guiSpellCtrl.h"

#include "console/console.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxDrawUtil.h"


IMPLEMENT_CONOBJECT(GuiSpellCtrl);

ConsoleDocClass( GuiSpellCtrl,
                "@brief A gui control that is used to display an image.\n\n"

                "The image is stretched to the constraints of the control by default. However, the control can also\n"
                "tile the image as well.\n\n"

                "The image itself is stored inside the GuiBitmapCtrl::bitmap field. The boolean value that decides\n"
                "whether the image is stretched or tiled is stored inside the GuiBitmapCtrl::wrap field.\n"

                "@tsexample\n"
                "// Create a tiling GuiBitmapCtrl that displays \"myImage.png\"\n"
                "%bitmapCtrl = new GuiBitmapCtrl()\n"
                "{\n"
                "   bitmap = \"myImage.png\";\n"
                "   wrap = \"true\";\n"
                "};\n"
                "@endtsexample\n\n"

                "@ingroup GuiControls"
                );

GuiSpellCtrl::GuiSpellCtrl(void)
   : mBitmapName(),
   mStartPoint( 0, 0 )
{
   mSpellData = NULL;
   mCDManager = NULL;
   mSpellDataName.clear();
}

bool GuiSpellCtrl::setSpellData( void *object, const char *index, const char *data )
{
   SpellData* spelldata;
   GuiSpellCtrl* ctrl = static_cast<GuiSpellCtrl *>( object );
   if(Sim::findObject(data, spelldata))
   {
      ctrl->setSpellData( spelldata );
      ctrl->setBitmap(spelldata->mLogo);
      return false;
   }
   else if(Sim::findObject(dAtoi(data), spelldata))
   {
      ctrl->setSpellData( spelldata );
      ctrl->setBitmap(spelldata->mLogo);
      return false;
   }
   else
      ctrl->mSpellDataName = String(data);
   return false;
}

void GuiSpellCtrl::initPersistFields()
{
   addGroup( "Bitmap" );

   addProtectedField( "Spell", TYPEID<SpellData>(), Offset( mSpellData, GuiSpellCtrl ),
      &setSpellData, &defaultProtectedGetFn,
      "");

   endGroup( "Bitmap" );

   Parent::initPersistFields();
}

bool GuiSpellCtrl::onWake()
{
   if (! Parent::onWake())
      return false;
   setActive(true);
   if(mSpellDataName.isNotEmpty())
      setSpellData(this, NULL, mSpellDataName.c_str());
   return true;
}

void GuiSpellCtrl::onSleep()
{
   if ( !mBitmapName.equal("texhandle", String::NoCase) )
      mTextureObject = NULL;

   Parent::onSleep();
}

//-------------------------------------
void GuiSpellCtrl::inspectPostApply()
{
   // if the extent is set to (0,0) in the gui editor and appy hit, this control will
   // set it's extent to be exactly the size of the bitmap (if present)
   Parent::inspectPostApply();

   if ((getExtent().x == 0) && (getExtent().y == 0) && mTextureObject)
   {
      setExtent( mTextureObject->getWidth(), mTextureObject->getHeight());
   }
}

void GuiSpellCtrl::setBitmap( const char *name, bool resize )
{
   mBitmapName = name;
   if ( !isAwake() )
      return;

   if ( mBitmapName.isNotEmpty() )
   {
      if ( !mBitmapName.equal("texhandle", String::NoCase) )
         mTextureObject.set( mBitmapName, &GFXDefaultGUIProfile, avar("%s() - mTextureObject (line %d)", __FUNCTION__, __LINE__) );

      // Resize the control to fit the bitmap
      if ( mTextureObject && resize )
      {
         setExtent( mTextureObject->getWidth(), mTextureObject->getHeight() );
         updateSizing();
      }
   }
   else
      mTextureObject = NULL;

   setUpdate();
}

void GuiSpellCtrl::updateSizing()
{
   if(!getParent())
      return;
   // updates our bounds according to our horizSizing and verSizing rules
   RectI fakeBounds( getPosition(), getParent()->getExtent());
   parentResized( fakeBounds, fakeBounds);
}

void GuiSpellCtrl::setBitmapHandle(GFXTexHandle handle, bool resize)
{
   mTextureObject = handle;

   mBitmapName = String("texhandle");

   // Resize the control to fit the bitmap
   if (resize) 
   {
      setExtent(mTextureObject->getWidth(), mTextureObject->getHeight());
      updateSizing();
   }
}

void GuiSpellCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   if(!mSpellData)
   {
      RectI rect(offset.x, offset.y, getExtent().x, getExtent().y);
      GFX->getDrawUtil()->drawRect(rect, ColorI(0,0,0));

      renderChildControls(offset, updateRect);
      return;
   }

   if (mTextureObject)
   {
      GFX->getDrawUtil()->clearBitmapModulation();
      RectI rect(offset, getExtent());
      GFX->getDrawUtil()->drawBitmapStretch(mTextureObject, rect, GFXBitmapFlip_None, GFXTextureFilterLinear, false);
      if(!mCDManager){
         U32 CDMID = atoi(NetConnection::getConnectionToServer()->getDataField("SpellSystemCDManager", NULL));
         CooldownManager* CDM;
         if(Sim::findObject(CDMID, CDM))
            setCooldownManager(CDM);
      }
      if(mCDManager)
      {
         U16 cd = mCDManager->GetCooldown(mSpellData);
         F32 coeff = F32(cd) / mSpellData->mCooldown;
         RectI rect(offset.x, offset.y + (getExtent().y * (1-coeff)), getExtent().x, getExtent().y * coeff);
         GFX->getDrawUtil()->drawRectFill(rect, ColorI(0,0,0,100));
      }
   }

   if (mProfile->mBorder || !mTextureObject)
   {
      RectI rect(offset.x, offset.y, getExtent().x, getExtent().y);
      GFX->getDrawUtil()->drawRect(rect, mProfile->mBorderColor);
   }

   renderChildControls(offset, updateRect);
}

void GuiSpellCtrl::setValue(S32 x, S32 y)
{
   if (mTextureObject)
   {
      x += mTextureObject->getWidth() / 2;
      y += mTextureObject->getHeight() / 2;
   }
   while (x < 0)
      x += 256;
   mStartPoint.x = x % 256;

   while (y < 0)
      y += 256;
   mStartPoint.y = y % 256;
}

DefineEngineMethod( GuiSpellCtrl, setValue, void, ( S32 x, S32 y ),,
                   "Set the offset of the bitmap within the control.\n"
                   "@param x The x-axis offset of the image.\n"
                   "@param y The y-axis offset of the image.\n")
{
   object->setValue(x, y);
}

DefineEngineMethod( GuiSpellCtrl, setCooldownManager, void, (SpellManager* MGR),,
                   "")
{
   object->setCooldownManager(MGR);
}


//

static ConsoleDocFragment _sGuiBitmapCtrlSetBitmap1(
   "@brief Assign an image to the control.\n\n"
   "Child controls with resize according to their layout settings.\n"
   "@param filename The filename of the image.\n"
   "@param resize Optional parameter. If true, the GUI will resize to fit the image.",
   "GuiBitmapCtrl", // The class to place the method in; use NULL for functions.
   "void setBitmap( String filename, bool resize );" ); // The definition string.

static ConsoleDocFragment _sGuiBitmapCtrlSetBitmap2(
   "@brief Assign an image to the control.\n\n"
   "Child controls will resize according to their layout settings.\n"
   "@param filename The filename of the image.\n"
   "@param resize A boolean value that decides whether the ctrl refreshes or not.",
   "GuiBitmapCtrl", // The class to place the method in; use NULL for functions.
   "void setBitmap( String filename );" ); // The definition string.


//"Set the bitmap displayed in the control. Note that it is limited in size, to 256x256."
ConsoleMethod( GuiSpellCtrl, setBitmap, void, 3, 4,
              "( String filename | String filename, bool resize ) Assign an image to the control.\n\n"
              "@hide" )
{
   char filename[1024];
   Con::expandScriptFilename(filename, sizeof(filename), argv[2]);
   object->setBitmap(filename, argc > 3 ? dAtob( argv[3] ) : false );
}
