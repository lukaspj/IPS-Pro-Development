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

#include "compositeEmitter.h"
#include "../particleEmitterNode.h"
#include "IPSCore.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(CompositeEmitterData);
IMPLEMENT_CONOBJECT(CompositeEmitter);

ConsoleDocClass( CompositeEmitter,
				"@brief This object is responsible for spawning particles.\n\n"

				"@note This class is not normally instantiated directly - to place a simple "
				"particle emitting object in the scene, use a ParticleEmitterNode instead.\n\n"

				"This class is the main interface for creating particles - though it is "
				"usually only accessed from within another object like ParticleEmitterNode "
				"or WheeledVehicle. If using this object class (via C++) directly, be aware "
				"that it does <b>not</b> track changes in source axis or velocity over the "
				"course of a single update, so emitParticles should be called at a fairly "
				"fine grain.  The emitter will potentially track the last particle to be "
				"created into the next call to this function in order to create a uniformly "
				"random time distribution of the particles.\n\n"

				"If the object to which the emitter is attached is in motion, it should try "
				"to ensure that for call (n+1) to this function, start is equal to the end "
				"from call (n). This will ensure a uniform spatial distribution.\n\n"

				"@ingroup FX\n"
				"@see ParticleEmitterData\n"
				"@see ParticleEmitterNode\n"
				);

ConsoleDocClass( CompositeEmitterData,
				"@brief Defines particle emission properties such as ejection angle, period "
				"and velocity for a ParticleEmitter.\n\n"

				"@tsexample\n"
				"datablock ParticleEmitterData( GrenadeExpDustEmitter )\n"
				"{\n"
				"   ejectionPeriodMS = 1;\n"
				"   periodVarianceMS = 0;\n"
				"   ejectionVelocity = 15;\n"
				"   velocityVariance = 0.0;\n"
				"   ejectionOffset = 0.0;\n"
				"   thetaMin = 85;\n"
				"   thetaMax = 85;\n"
				"   phiReferenceVel = 0;\n"
				"   phiVariance = 360;\n"
				"   overrideAdvance = false;\n"
				"   lifetimeMS = 200;\n"
				"   particles = \"GrenadeExpDust\";\n"
				"};\n"
				"@endtsexample\n\n"

				"@ingroup FX\n"
				"@see ParticleEmitter\n"
				"@see ParticleData\n"
				"@see ParticleEmitterNode\n"
				);

//-----------------------------------------------------------------------------
// MaskEmitterData
//-----------------------------------------------------------------------------
CompositeEmitterData::CompositeEmitterData()
{
   mEmitterData1 = NULL;
   mEmitterData2 = NULL;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void CompositeEmitterData::initPersistFields()
{
	addGroup( "CompositeEmitter" );

   addField("Emitter1", TYPEID<ParticleEmitterData>(), Offset(mEmitterData1, CompositeEmitterData), "");
   addField("Emitter2", TYPEID<ParticleEmitterData>(), Offset(mEmitterData2, CompositeEmitterData), "");

	endGroup( "CompositeEmitter" );
	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void CompositeEmitterData::packData(BitStream* stream)
{
	Parent::packData(stream);
   
   stream->writeRangedU32( mEmitterData1->getId(),
      DataBlockObjectIdFirst,
      DataBlockObjectIdLast );

   stream->writeRangedU32( mEmitterData2->getId(),
      DataBlockObjectIdFirst,
      DataBlockObjectIdLast );
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void CompositeEmitterData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

   SimObjectId id = stream->readRangedU32( DataBlockObjectIdFirst,
      DataBlockObjectIdLast );
   if ( !Sim::findObject( id, mEmitterData1 ) )
      mEmitterData1 = NULL;
   
   id = stream->readRangedU32( DataBlockObjectIdFirst,
      DataBlockObjectIdLast );
   if ( !Sim::findObject( id, mEmitterData2 ) )
      mEmitterData2 = NULL;
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool CompositeEmitterData::onAdd()
{
	if( Parent::onAdd() == false )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// SphereEmitter
//-----------------------------------------------------------------------------
CompositeEmitter::CompositeEmitter()
{
   mEmitter1 = NULL;
   mEmitter2 = NULL;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void CompositeEmitter::initPersistFields()
{

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 CompositeEmitter::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
	U32 retMask = Parent::packUpdate(con, mask, stream);

	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void CompositeEmitter::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);

}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool CompositeEmitter::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	Parent::onNewDataBlock(dptr, reload);
	CompositeEmitterData* DataBlock = getDataBlock();
	if ( !DataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

   if(isClientObject())
   {
      if(!mEmitter1.isNull())
         mEmitter1->deleteObject();
      if(!mEmitter2.isNull())
         mEmitter2->deleteObject();
      mEmitter1 = DataBlock->mEmitterData1->createEmitter();
      mEmitter2 = DataBlock->mEmitterData2->createEmitter();
      if(!mEmitter1->registerObject() || !mEmitter2->registerObject())
      {
         delete mEmitter1;
         delete mEmitter2;
         return false;
      }
      mEmitter1->setDataBlock(DataBlock->mEmitterData1);
      mEmitter2->setDataBlock(DataBlock->mEmitterData2);
   }

	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// addParticle
//-----------------------------------------------------------------------------
bool CompositeEmitter::addParticle(const Point3F& pos,
							  const Point3F& axis,
							  const Point3F& vel,
							  const Point3F& axisx, 
							  const MatrixF &trans)
{
	PROFILE_SCOPE(CompEmitAddPart);
	CompositeEmitterData* DataBlock = getDataBlock();
   mEmitter1->addParticle(pos, axis, vel, axisx, trans);
   mEmitter2->addParticle(pos, axis, vel, axisx, trans);
	return true;
}

//-----------------------------------------------------------------------------
// addParticle - Node
//-----------------------------------------------------------------------------
bool CompositeEmitter::addParticle(const Point3F& pos,
							  const Point3F& axis,
							  const Point3F& vel,
							  const Point3F& axisx,
							  ParticleEmitterNode* pnodeDat)
{
	PROFILE_SCOPE(CompEmitAddPart);
   mEmitter1->addParticle(pos, axis, vel, axisx, pnodeDat->getTransform());
   mEmitter2->addParticle(pos, axis, vel, axisx, pnodeDat->getTransform());
	return true;
}

void CompositeEmitter::onStaticModified(const char* slotName, const char*newValue)
{
	if(strcmp(slotName, "sa_Treshold_min") == 0 ||
		strcmp(slotName, "sa_Treshold_max") == 0)
		setMaskBits( sa_Mask );
	Parent::onStaticModified(slotName, newValue);
}

ParticleEmitter* CompositeEmitterData::createEmitter() { return new CompositeEmitter; }


//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void CompositeEmitter::emitParticles(const Point3F& point,
                                    const bool     useLastPosition,
                                    const Point3F& axis,
                                    const Point3F& velocity,
                                    const U32      numMilliseconds)
{
   mEmitter1->emitParticles(point, useLastPosition, axis, velocity, numMilliseconds);
   mEmitter2->emitParticles(point, useLastPosition, axis, velocity, numMilliseconds);
}

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void CompositeEmitter::emitParticles(const Point3F& start,
                                    const Point3F& end,
                                    const Point3F& axis,
                                    const Point3F& velocity,
                                    const U32      numMilliseconds)
{
   mEmitter1->emitParticles(start, end, axis, velocity, numMilliseconds);
   mEmitter2->emitParticles(start, end, axis, velocity, numMilliseconds);
}

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void CompositeEmitter::emitParticles( const U32 numMilliseconds, ParticleEmitterNode* node )
{
   mEmitter1->emitParticles(numMilliseconds, node);
   mEmitter2->emitParticles(numMilliseconds, node);
}

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void CompositeEmitter::emitParticles(const Point3F& rCenter,
                                    const Point3F& rNormal,
                                    const F32      radius,
                                    const Point3F& velocity,
                                    S32 count)
{
   mEmitter1->emitParticles(rCenter, rNormal, radius, velocity, count);
   mEmitter2->emitParticles(rCenter, rNormal, radius, velocity, count);
}
