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

#include "sphereEmitterNode.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

#include "console/engineAPI.h"
#include "console\simPersistID.h"
#include "math/mTransform.h"

IMPLEMENT_CO_DATABLOCK_V1(SphereEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(SphereEmitterNode);

ConsoleDocClass( SphereEmitterNodeData,
	"@brief Contains additional data to be associated with a ParticleEmitterNode."
	"@ingroup FX\n"
	);

ConsoleDocClass( SphereEmitterNode,
	"@brief A particle emitter object that can be positioned in the world and "
	"dynamically enabled or disabled.\n\n"

	"@tsexample\n"
	"datablock ParticleEmitterNodeData( SimpleEmitterNodeData )\n"
	"{\n"
	"   timeMultiple = 1.0;\n"
	"};\n\n"

	"%emitter = new ParticleEmitterNode()\n"
	"{\n"
	"   datablock = SimpleEmitterNodeData;\n"
	"   active = true;\n"
	"   emitter = FireEmitterData;\n"
	"   velocity = 3.5;\n"
	"};\n\n"

	"// Dynamically change emitter datablock\n"
	"%emitter.setEmitterDataBlock( DustEmitterData );\n"
	"@endtsexample\n"

	"@note To change the emitter field dynamically (after the ParticleEmitterNode "
	"object has been created) you must use the setEmitterDataBlock() method or the "
	"change will not be replicated to other clients in the game.\n"
	"Similarly, use the setActive() method instead of changing the active field "
	"directly. When changing velocity, you need to toggle setActive() on and off "
	"to force the state change to be transmitted to other clients.\n\n"

	"@ingroup FX\n"
	"@see ParticleEmitterNodeData\n"
	"@see ParticleEmitterData\n"
	);

SphereEmitterNodeData::SphereEmitterNodeData()
{
}

void SphereEmitterNodeData::initPersistFields()
{
	
	Parent::initPersistFields();
}

bool SphereEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	//Validate variables here

	return true;
}

//-----------------------------------------------------------------------------
// SphereEmitterNode
//-----------------------------------------------------------------------------
SphereEmitterNode::SphereEmitterNode()
{
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void SphereEmitterNode::initPersistFields()
{
	addField( "emitter",  TYPEID< SphereEmitterData >(), Offset(mEmitterDatablock, SphereEmitterNode),
		"Datablock to use when emitting particles." );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool SphereEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	setmDataBlock(dynamic_cast<SphereEmitterNodeData*>( dptr ));
	SphereEmitterNodeData* DataBlock = getDataBlock();
	if ( !DataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

void SphereEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	SphereEmitterNodeData* DataBlock = getDataBlock();
	if(!mActive || !mEmitter || !mEmitter->isProperlyAdded() || !DataBlock)
		return;

	mEmitter->emitParticles( (U32)(dt * DataBlock->timeMultiple * 1000.0f), this );
}

DefineEngineMethod( SphereEmitterNode, mountToNode, bool,
				   ( SceneObject* objB, StringTableEntry node, TransformF txfm ), ( MatrixF::Identity ),
				   "@brief Mount objB to this object at the desired slot with optional transform.\n\n"

				   "@param objB  Object to mount onto us\n"
				   "@param slot  Mount slot ID\n"
				   "@param txfm (optional) mount offset transform\n"
				   "@return true if successful, false if failed (objB is not valid)" )
{
	ShapeBase* host = dynamic_cast<ShapeBase*>(objB);
	if ( host )
	{
		S32 slot = host->getShape()->findNode(node);
		host->mountObject( object, slot, txfm.getMatrix() );
		object->mNodeMounted = true;
		return true;
	}
	return false;
}

DefineEngineMethod(SphereEmitterNode, setActive, void, (bool active),,
				   "Turns the emitter on or off.\n"
				   "@param active New emitter state\n" )
{
	object->setActive( active );
}

DefineEngineMethod(SphereEmitterNode, setEmitterDataBlock, void, (ParticleEmitterData* emitterDatablock), (0),
   "Assigns the datablock for this emitter node.\n"
   "@param emitterDatablock ParticleEmitterData datablock to assign\n"
   "@tsexample\n"
   "// Assign a new emitter datablock\n"
   "%emitter.setEmitterDatablock( %emitterDatablock );\n"
   "@endtsexample\n" )
{
   if ( !emitterDatablock )
   {
      Con::errorf("ParticleEmitterData datablock could not be found when calling setEmitterDataBlock in particleEmitterNode.");
      return;
   }

   object->setEmitterDataBlock(emitterDatablock);
}