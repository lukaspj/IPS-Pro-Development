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
void AttractionBehaviour::updateParticle(ParticleEmitter* emitter, Particle* part, F32 time)
{
	for(int i = 0; i < attrobjectCount; i++)
	{
		if(AttractionMode[i] > 0)
		{
			GameBase* GB = dynamic_cast<GameBase*>(Sim::findObject(attractedObjectID[i]));
			if(!GB)
				GB = dynamic_cast<GameBase*>(Sim::findObject(atoi(attractedObjectID[i])));
			if(GB){
				Point3F target = GB->getPosition();
				char attrBuf[255];
				strcpy(attrBuf,Attraction_offset[i]);

				MatrixF trans = GB->getTransform();

				char* xBuf = strtok(attrBuf, " ");
				F32 x = atof(xBuf);
				char* yBuf = strtok(NULL, " ");
				F32 y = atof(yBuf);
				char* zBuf = strtok(NULL, " ");
				F32 z = atof(zBuf);
				Point3F po;
				trans.mulV(Point3F(x,y,z), &po);
				target += po;

				Point3F diff = (target - part->pos);
				Point3F attR = Point3F(attractionrange);
				if(diff.len() < 1)
					diff.normalize();
				Point3F ndiff = diff;
				ndiff.normalize();
				F32 fdiff = attractionrange/(diff.len())-1;
				if(fdiff < 0)
					fdiff = 0;
				if(AttractionMode[i] == 1)
					part->acc += (ndiff * fdiff)*Amount[i];
				if(AttractionMode[i] == 2)
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