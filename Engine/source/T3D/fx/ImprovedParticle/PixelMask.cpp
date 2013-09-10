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

	loadFromImage(Torque::Path(thePath));

   return true;
}

void PixelMask::onRemove()
{
	Parent::onRemove();
}

void PixelMask::initPersistFields()
{
	addField("MaskPath", TypeStringFilename, Offset(thePath, PixelMask), "Some description");
	Parent::initPersistFields();
}

void PixelMask::loadFromImage(const Torque::Path &path)
{
	PROFILE_SCOPE(PixelMaskImageLoad);
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
						pixels[col.alpha].push_back(Point2I(ix,iy));
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
						pixels[(int)mFloor(F32((col.red + col.green + col.blue)/3))].push_back(Point2I(ix,iy));
					}
				}
			}
		}
	}
}

Point2F PixelMask::getRandomUnitPixel(U8 min, U8 max, Cache &cache)
{
	AssertWarn(min <= max, "PixelMask::getRandomUnitPixel min treshold is larger than max treshold");
	if(min > max) swap(min,max);
	AssertWarn(min <= 255, "PixelMask::getRandomUnitPixel min treshold is larger than 255");
	if(min > 255) min = max = 255;
	AssertWarn(max <= 255, "PixelMask::getRandomUnitPixel max treshold is larger than 255");
	if(max > 255) max = 255;

	if(cache.firstRun && cache.Treshold_min == cache.Treshold_max)
	{
		cache.Sizes.push_back(pixels[min].size());
		cache.Size = pixels[min].size();
	}
	cache.firstRun = false;
	PROFILE_SCOPE(PixelMaskrndPixel);
	U32 size = 0;
	bool onlymin, onlymax, maxminsame, oldsize = false;
	onlymin = maxminsame = onlymax = oldsize;
	if(max == min)
		maxminsame = true;
	if(max == cache.Treshold_max && min == cache.Treshold_min)
		oldsize = true;
	else if(max == cache.Treshold_max)
		onlymin = true;
	else if(min == cache.Treshold_min)
		onlymax = true;
	Vector<int> sizes;
	if(oldsize || onlymin || onlymax){
		sizes = cache.Sizes;
		size = cache.Size;
	}
	if(maxminsame && !oldsize)
	{
		sizes.clear();
		sizes.push_back(pixels[min].size());
		size = pixels[min].size();
	}
	else if(onlymin)
	{
		if(min < cache.Treshold_min) // Add new vectors
		{
			for(int i = cache.Treshold_min-1; i >= min; i--){
				U32 s = pixels[i].size();
				size += s;
				sizes.push_front(s);
			}
		}
		else if ( min > cache.Treshold_min ) // Remove vectors
		{
			for(int i = min-1; i >= cache.Treshold_min; i--){
				U32 s = pixels[i].size();
				size -= s;
				sizes.pop_front();
			}
		}
	}
	else if(onlymax)
	{
		if(max > cache.Treshold_max) // Add new vectors
		{
			for(int i = cache.Treshold_max+1; i <= max; i++){
				U32 s = pixels[i].size();
				size += s;
				sizes.push_back(s);
			}
		}
		else if ( max < cache.Treshold_max ) // Remove vectors
		{
			for(int i = cache.Treshold_max; i > max; i--){
				U32 s = pixels[i].size();
				size -= s;
				sizes.pop_back();
			}
		}
	}
	else if(!oldsize)
	{
		for(int i = min; i <= max; i++){
			U32 s = pixels[i].size();
			size += s;
			sizes.push_back(s);
		}
	}
	cache.Treshold_max = max;
	cache.Treshold_min = min;
	cache.Size = size;
	cache.Sizes = sizes;
	if(size == 0)
		return Point2F::Max;
	Point2F pixel;
	U32 ptIdx = (gRandGen.randI() % size) + 1;
	for(int i = 0; i <= max - min; i++)
	{
		if(sizes[i] < ptIdx)
		{
			ptIdx -= sizes[i];
			continue;
		}
		else{
			pixel = Point2F(pixels[min+i][ptIdx-1].x,pixels[min+i][ptIdx-1].y);
			break;
		}
	}
	pixel = Point2F(((pixel.x / Width)-0.5)*-1, (pixel.y / Height)-0.5)*2;
	return pixel;
}