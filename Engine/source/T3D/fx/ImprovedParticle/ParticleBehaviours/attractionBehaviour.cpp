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

#include "attractionBehaviour.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(AttractionBehaviour);

typedef AttractionBehaviour::EnumAttractionMode ParticleAttractionMode;
DefineEnumType( ParticleAttractionMode );

ImplementEnumType( ParticleAttractionMode,
				  "The way the particles are interacting with specific objects.\n"
				  "@ingroup FX\n\n")
{ AttractionBehaviour::none,		"Not attracted",        "The particles are not attracted to any object.\n" },
{ AttractionBehaviour::attract,		"Attract",				"The particles are attracted to the objects.\n" },
{ AttractionBehaviour::repulse,		"Repulse",				"The particles are repulsed by the object.\n" },
EndImplementEnumType;

//---------------------------------------------------------------
// Constructor
//---------------------------------------------------------------
AttractionBehaviour::AttractionBehaviour()
{
	attractionrange = 50;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = 0;
		Amount[i] = 1;
		Attraction_offset[i] = "0 0 0";
		attractedObjectID[i] = "";
	}
}

//----------------------IParticleBehaviour-----------------------
//---------------------------------------------------------------
// updateParticle
// --Description
//---------------------------------------------------------------
void AttractionBehaviour::updateParticle(Particle* part, F32 time)
{
	for(int i = 0; i < attrobjectCount; i++)
	{
		if(AttractionMode[i] != ParticleAttractionMode::none)
		{
			GameBase* GB = dynamic_cast<GameBase*>(Sim::findObject(attractedObjectID[i]));
			if(!GB)
				GB = dynamic_cast<GameBase*>(Sim::findObject(atoi(attractedObjectID[i])));
			if(GB){
				Point3F target = GB->getPosition();

				// strtok doesn't like pointers, so we copy the Attraction_offset to a 
				// local variable.
				char attrBuf[255];
				strcpy(attrBuf, Attraction_offset[i]);

				MatrixF trans = GB->getTransform();

				// Fetch the first word which is the x-offset
				char* xBuf = strtok(attrBuf, " ");
				F32 x = atof(xBuf);
				// Fetch the next word which is the y-offset
				char* yBuf = strtok(NULL, " ");
				F32 y = atof(yBuf);
				// Fetch the next word which is the z-offset
				char* zBuf = strtok(NULL, " ");
				F32 z = atof(zBuf);
				// Rotate the offset so it matches the objects rotation
				Point3F po;
				trans.mulV(Point3F(x,y,z), &po);
				// Add the offset to the target attraction location
				target += po;

				// Get the vector that points from the particle down to the target 
				// location.
            VectorF diff = (target - part->pos);
				// Get the attraction range as a vector
				VectorF attR = VectorF(attractionrange);
				// If the particle is really close to the target location, just
				// treat it as a length of one. This is to avoid huge scaling 
				// when particles get close to an object.
				if(diff.len() < 0.05f){
					diff.normalize();
					diff *= 0.05f;
				}
				// Get the normalized difference vector to have a direction vector
				VectorF ndiff = diff;
				ndiff.normalize();
				// Get a coefficient that gets bigger the closer it is to the
				// particle since attraction scales based on distance
				F32 fdiff = attractionrange/(diff.len())-1;
				if(fdiff < 0)
					fdiff = 0;
				// Multiply the direction vector with the coefficient and multiply
				// the product with the AttractionAmount
				if(AttractionMode[i] == ParticleAttractionMode::attract)
					part->acc += (ndiff * fdiff)*Amount[i];
				if(AttractionMode[i] == ParticleAttractionMode::repulse)
					part->acc -= (ndiff * fdiff)*Amount[i];
			}
		}
	}
}


//----------------------- SimDataBlock -------------------------
//---------------------------------------------------------------
// InitPersistFields
//--------------------------------------------------------------
void AttractionBehaviour::initPersistFields()
{
	addField( "attractedObjectID", TYPEID< StringTableEntry >(), Offset(attractedObjectID, AttractionBehaviour), attrobjectCount,
		"The ID or name of the object that the particles should interact with." );

	addField( "Attraction_offset", TYPEID< StringTableEntry >(), Offset(Attraction_offset, AttractionBehaviour), attrobjectCount,
		"Offset from the objects position the particles should be attracted to or repulsed from." );

	addField( "AttractionMode", TYPEID< ParticleAttractionMode >(), Offset(AttractionMode, AttractionBehaviour), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, AttractionBehaviour), attrobjectCount,
		"Amount of influence, combine with attraction range for the desired result." );

	addField( "attractionrange", TYPEID< F32 >(), Offset(attractionrange, AttractionBehaviour),
		"Range of influence, any objects further away than this length will not attract or repulse the particles." );

	Parent::initPersistFields();
}

//---------------------------------------------------------------
// OnAdd
//---------------------------------------------------------------
bool AttractionBehaviour::onAdd()
{
	if( !Parent::onAdd() )
		return false;
	return true;
}

//---------------------------------------------------------------
// PackData
//--------------------------------------------------------------
void AttractionBehaviour::packData(BitStream* stream)
{
	Parent::packData(stream);
	stream->writeInt(attractionrange*1000, 15);
	for(int i = 0; i < attrobjectCount; i++)
	{
		stream->writeInt(AttractionMode[i], 4);
		stream->writeInt(Amount[i]*1000, 15);
		stream->writeString(attractedObjectID[i]);
		stream->writeString(Attraction_offset[i]);
	}
}

//---------------------------------------------------------------
// UnpackData
//--------------------------------------------------------------
void AttractionBehaviour::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	attractionrange = stream->readInt(15)  / 1000.0f;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = stream->readInt(4);
		Amount[i] = stream->readInt(15) / 1000.0f;
		char buf[256];
		stream->readString(buf);
		attractedObjectID[i] = dStrdup(buf);
		char buf2[256];
		stream->readString(buf2);
		Attraction_offset[i] = dStrdup(buf2);
	}
}

//---------------------------------------------------------------
// Preload
//--------------------------------------------------------------
bool AttractionBehaviour::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;
	// Verify variables

	return true;
}