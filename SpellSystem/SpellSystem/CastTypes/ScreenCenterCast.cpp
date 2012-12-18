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

#include "ScreenCenterCast.h"

void ScreenCenterCast::Cast(ShapeBase* Obj, Spell* spell)
{
	spell->onChannel();
	ShapeBase* scObj = dynamic_cast<ShapeBase*>(Obj);
	MatrixF mat;
	//Eyepoint = position, eyeVector = forwardvector
	scObj->getEyeTransform(&mat);
	VectorF eyeVector = mat.getForwardVector();
	Point3F start = mat.getPosition();
	//F32 range = spell->getRange();
	F32 range = 100;
	VectorF rayDirection = eyeVector * range;
	VectorF end = start + rayDirection;
	RayInfo rInfo;
	bool hit = gServerContainer.castRay(start,end,PlayerObjectType,&rInfo);
	if(hit)
	{
		spell->Cast(start, rInfo.point);
	}
	else{
		spell->Cast(start, end);
	}
}