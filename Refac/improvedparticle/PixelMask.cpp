//-----------------------------------------------------------------------------
// IPS Pro
// Copyright Lukas Jørgensen 2012 - FuzzyVoidStudio
// License:
// 'the product' refers to the IPS Pro.
// If you have not paid for the product you are not allowed to use it
//  - personally nor commercially.
// Assuming you have paid for the product you are allowed to
//  - include this in any commercial product as long as the IPS Pro
//  - source code is never released along with the commercial product.
// You may not resell or redistribute the product.
// You'll need one license for each man on your team, unless you fall under
//  - the indie license.
// You may not use the code to create new products in the same category.
//  - e.g. you may not use this code to construct a mathEmitter and sell that
//  - as a new product. Not if it is released for another GameEngine either.
// The indie license: As long as your products using IPS Pro has a revenue of
//  - less than 40.000$ annually you fall under this license.
// Email me at LukasPJ@FuzzyVoidStudio.com 
//  - if you have further questions regarding license
// http://fuzzyvoidstudio.com
//-----------------------------------------------------------------------------

#include "PixelMask.h"
#include "gfx\bitmap\gBitmap.h"
#include "core\color.h"
#include "math/mRandom.h"
#include "platform\profiler.h"
#include "console/consoleTypes.h"

IMPLEMENT_CO_DATABLOCK_V1(PixelMask);

PixelMask::PixelMask()
{
	thePath = Torque::Path();
}

PixelMask::~PixelMask()
{

}

bool PixelMask::onAdd()
{
	if( !Parent::onAdd() )
      return false;

	loadFromImage(Torque::Path(thePath), Treshold);

   return true;
}

void PixelMask::onRemove()
{
	Parent::onRemove();
}

void PixelMask::initPersistFields()
{
	addField("MaskPath", TypeStringFilename, Offset(thePath, PixelMask), "Some description");
	addField("Treshold", TypeS8 , Offset(Treshold, PixelMask), "Some description");
	Parent::initPersistFields();
}

void PixelMask::loadFromImage(const Torque::Path &path, U8 Treshold)
{
	PROFILE_SCOPE(PixelMaskImageLoad);
	pixels.clear();
	Resource<GBitmap> bitmap;
	bitmap = GBitmap::load(path);
	if(bitmap)
	{
		Height = bitmap->getHeight();
		Width = bitmap->getWidth();
		if(bitmap->checkForTransparency())
		{
			for(U32 iy = 0; iy < Height; iy++)
			{
				for(U32 ix = 0; ix < Width; ix++)
				{
					ColorI col;
					if( bitmap->getColor(ix, iy, col) )
					{
						if(col.alpha > Treshold)
							pixels.push_back(Point2I(ix,iy));
					}
				}
			}
		}
		else
		{
			for(U32 iy = 0; iy < Height; iy++)
			{
				for(U32 ix = 0; ix < Width; ix++)
				{
					ColorI col;
					if( bitmap->getColor(ix, iy, col) )
					{
						if((col.red + col.green + col.blue)/3 < Treshold)
							pixels.push_back(Point2I(ix,iy));
					}
				}
			}
		}
	}
}

Point2F PixelMask::getRandomUnitPixel()
{
	PROFILE_SCOPE(PixelMaskrndPixel);
	Point2F retPt;
	if(pixels.size() > 0)
	{
		U32 ptIdx = gRandGen.randI() % pixels.size();
		Point2F pixel = Point2F(pixels[ptIdx].x,pixels[ptIdx].y);
		retPt = Point2F(((pixel.x / Width)-0.5)*-1, (pixel.y / Height)-0.5)*2;
		return retPt;
	}
	else
		return retPt.Max;
}