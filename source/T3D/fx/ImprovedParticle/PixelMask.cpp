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
	/*for(int i = 0; i < 255; i++)
	{
		pixels[i] = new Vector();
	}*/
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
	//addField("Treshold", TypeS8 , Offset(Treshold, PixelMask), "Some description");
	Parent::initPersistFields();
}

void PixelMask::loadFromImage(const Torque::Path &path)
{
	PROFILE_SCOPE(PixelMaskImageLoad);
	//pixels.clear();
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
						if(col.alpha == 255)
							bool a = false;
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
			/*if(cache.Treshold_max == cache.Treshold_min)
			{
				U32 s = pixels[cache.Treshold_max].size();
				size += s;
				sizes.push_back(s);
			}*/
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