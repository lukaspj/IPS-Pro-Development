//-----------------------------------------------------------------------------
// PixelMask helper class
// Copyright Lukas Joergensen, FuzzyVoidStudio 2012
//-----------------------------------------------------------------------------

#ifndef _PEFFECTREADER_H_
#define _PEFFECTREADER_H_

#include "console\engineAPI.h"
#include "core\util\path.h"
#include "core\util\tVector.h"
#include "rapidxml.hpp"
#include <vector>

using namespace rapidxml;

//--------------------------------------------------------------------------
class pEffectReader
{
public:
	enum emitterType{
		stockEmitter = 0,
		GraphEmitter,
		MeshEmitter,
		RadiusMeshEmitter,
		GroundEmitter,
		MaskEmitter,
		PathEmitter,
		ENDOFEMITTERS
	};
	struct point{
		float x;
		float y;
		std::string easing;
		bool easeIn;
		bool easeOut;
	};
	struct value{
		std::string name;
		F32 DeltaValue;
		Vector<point> points;
		bool ease;
		float setTime;
		float initialValue;
	};
	struct emitter{
		emitterType type;
		std::string datablock;
		std::string emitterblock;
		Vector<value> values;
		float start;
		float end;
		float x;
		float y;
		float z;
	};

	//Vector<emitter>* loadedEmitters;
	static void readFile(Torque::Path &path, Vector<emitter> &loadedEmitters);
	static int stringToEmitterType(char* str);
	static bool stringToBoolean(char* str);
	static char* UpToLow(char* str);
	static S32 stringToEase(char* str);
	static U8 inOutCompose( bool in, bool out);
};

#endif // _PEFFECTREADER_H_

