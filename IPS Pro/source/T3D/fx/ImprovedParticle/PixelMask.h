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
#ifndef _PIXELMASK_H_
#define _PIXELMASK_H_

#ifndef __RESOURCE_H__
#include "core/resource.h"
#endif
#include "math\mPoint2.h"

#ifndef _SIMOBJECT_H_
#include "console/simObject.h"
#endif

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif

//--------------------------------------------------------------------------
class PixelMask : public GameBaseData
{
	typedef GameBaseData Parent;

public:
	struct Cache{
		U8 Treshold_max;
		U8 Treshold_min;
		U32 Size;
		Vector<int> Sizes;
		bool firstRun;
	};
private:

	U32 Height;
	U32 Width;
	Vector<Point2I> pixels[256];
	FileName thePath;
public:
	void compile(const Torque::Path &path);
	void loadFromImage(const Torque::Path &path);
	void loadFromCompiledFile(const Torque::Path &path);
	Point2F getRandomUnitPixel(U8 treshold_min, U8 treshold_max, Cache &cache);

	// SimObject
	virtual bool onAdd();
	virtual void onRemove();

	// ConsoleObject
	static void initPersistFields();

	PixelMask();
	~PixelMask();


	DECLARE_CONOBJECT(PixelMask);
};

#endif // _PIXELMASK_H_

