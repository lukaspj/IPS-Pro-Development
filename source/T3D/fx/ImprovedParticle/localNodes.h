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

#ifndef _H_LOCAL_NODES
#define _H_LOCAL_NODES
#include "graphEmitterNode.h"
#include "../particleEmitterNode.h"
#include "maskEmitterNode.h"
#include "groundEmitterNode.h"

class loc_GraphEmitterNode : public GraphEmitterNode
{
	typedef GraphEmitterNode Parent;
public:
	loc_GraphEmitterNode(){ mNetFlags.clear(Ghostable | ScopeAlways); mNetFlags.set(IsGhost);  };
};

class loc_ParticleEmitterNode : public ParticleEmitterNode
{
	typedef ParticleEmitterNode Parent;
public:
	loc_ParticleEmitterNode(){ mNetFlags.clear(Ghostable | ScopeAlways); mNetFlags.set(IsGhost);  };
};

class loc_GroundEmitterNode : public GroundEmitterNode
{
	typedef GroundEmitterNode Parent;
public:
	loc_GroundEmitterNode(){ mNetFlags.clear(Ghostable | ScopeAlways); mNetFlags.set(IsGhost);  };
};

class loc_MaskEmitterNode : public MaskEmitterNode
{
	typedef MaskEmitterNode Parent;
public:
	loc_MaskEmitterNode(){ mNetFlags.clear(Ghostable | ScopeAlways); mNetFlags.set(IsGhost);  };
};

#endif // _H_LOCAL_NODES
