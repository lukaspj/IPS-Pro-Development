#include "pEffectReader.h"
#include "platform\profiler.h"
#include "rapidxml.hpp"
#include <istream>
#include <fstream>
#include <vector>
using namespace std;

// Parse the .pEffect file using RapidXML
void pEffectReader::readFile(Torque::Path &path, Vector<emitter> &loadedEmitters)
{
	PROFILE_SCOPE(pEffectReadFile);
	loadedEmitters.clear();
	ifstream theFile;
	xml_document<> doc;
	theFile.open(path.getFullPath());
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>( ));
	buffer.push_back('\0');
	if(theFile.is_open())
	{
		doc.parse<0>(&buffer[0]);
	}
	else return;
	xml_node<> *base = doc.first_node("ParticleEffect");
	base = base->first_node("Emitters");
	xml_node<> *node = base->first_node("Emitter");
	int emitterCount = 0, valueCount = 0, pointCount = 0;
	while(node)
	{
		emitter em;
		xml_attribute<> *attr = node->first_attribute("Type");
		em.type = (emitterType)stringToEmitterType(attr->value());
		attr = node->first_attribute("x");
		em.x = atof(attr->value());
		attr = node->first_attribute("y");
		em.y = atof(attr->value());
		attr = node->first_attribute("z");
		em.z = atof(attr->value());
		attr = node->first_attribute("datablock");
		em.datablock = std::string(attr->value());
		attr = node->first_attribute("emitter");
		em.emitterblock =  std::string(attr->value());
		attr = node->first_attribute("Start");
		em.start = atof(attr->value());
		attr = node->first_attribute("End");
		em.end = atof(attr->value());
		xml_node<> *valnode = node->first_node("Value");
		while(valnode)
		{
			value val;
			attr = valnode->first_attribute("Ease");
			val.ease = stringToBoolean(attr->value());
			attr = valnode->first_attribute("Name");
			val.type = (valueType)stringToValueType(attr->value());
			attr = valnode->first_attribute("DeltaValue");
			val.DeltaValue = atof(attr->value());
			if(!val.ease)
			{
				attr = valnode->first_attribute("setTime");
				val.setTime = atof(attr->value());
			}
			else{
				xml_node<> *pointnode = valnode->first_node("Point");
				while(pointnode)
				{
					point p;
					attr = pointnode->first_attribute("X");
					p.x = atof(attr->value());
					attr = pointnode->first_attribute("Y");
					p.y = atof(attr->value());
					attr = pointnode->first_attribute("Easing");
					p.easing = std::string(attr->value());
					attr = pointnode->first_attribute("EaseIn");
					p.easeIn = stringToBoolean(attr->value());
					attr = pointnode->first_attribute("EaseOut");
					p.easeOut = stringToBoolean(attr->value());
					val.points.push_back(p);
					pointnode = pointnode->next_sibling();
				}
			}
			em.values.push_back(val);
			valnode = valnode->next_sibling();
		}
		loadedEmitters.push_back(em);
		node = node->next_sibling();
	}
}

// Helper function, convert a string to an emitter type
int pEffectReader::stringToEmitterType(char* str)
{
	UpToLow(str);
	if(!strcmp(str, "stockemitter"))
		return 0;
	if(!strcmp(str, "graphemitter"))
		return 1;
	if(!strcmp(str, "meshemitter"))
		return 2;
	if(!strcmp(str, "radiusmeshemitter"))
		return 3;
	if(!strcmp(str, "groundemitter"))
		return 4;
	if(!strcmp(str, "maskemitter"))
		return 5;
	if(!strcmp(str, "pathemitter"))
		return 7;
	return 8;
}

// Helper function, convert a string to a value type
int pEffectReader::stringToValueType(char* str)
{
	UpToLow(str);
	int ret = 0;
	if(!strcmp(str, "xposition"))
		return ret;
	ret++;
	if(!strcmp(str, "yposition"))
		return ret;
	ret++;
	if(!strcmp(str, "zposition"))
		return ret;
	ret++;
	if(!strcmp(str, "active"))
		return ret;
	ret++;
	if(!strcmp(str, "ejectionoffset"))
		return ret;
	ret++;
	if(!strcmp(str, "ejectionperiod"))
		return ret;
	ret++;
	if(!strcmp(str, "radius"))
		return ret;
	ret++;
	if(!strcmp(str, "upperboundary"))
		return ret;
	ret++;
	if(!strcmp(str, "lowerboundary"))
		return ret;
	ret++;
	if(!strcmp(str, "timescale"))
		return ret;
	ret++;
	if(!strcmp(str, "scale"))
		return ret;
	ret++;
	if(!strcmp(str, "treshold_max"))
		return ret;
	ret++;
	if(!strcmp(str, "treshold_min"))
		return ret;
	ret++;
	return ret;
}

// Helper function, convert a string to an easing method
S32 pEffectReader::stringToEase(char* str)
{
	UpToLow(str);
	if(!strcmp(str, "linear"))
		return 0;
	if(!strcmp(str, "quadratic"))
		return 1;
	if(!strcmp(str, "cubic"))
		return 2;
	if(!strcmp(str, "quartic"))
		return 3;
	if(!strcmp(str, "quintic"))
		return 4;
	if(!strcmp(str, "sinusoidal"))
		return 5;
	if(!strcmp(str, "exponential"))
		return 7;
	if(!strcmp(str, "circular"))
		return 7;
	if(!strcmp(str, "elastic"))
		return 7;
	if(!strcmp(str, "back"))
		return 7;
	if(!strcmp(str, "bounce"))
		return 7;
}

// Helper function, compose an inOut argument from 2 bools
U8 pEffectReader::inOutCompose( bool in, bool out)
{
	if(in && out)
		return 0;
	if(in)
		return 1;
	if(out)
		return 2;
	return 3;
}

// Helper function, Convert string to a boolean
bool pEffectReader::stringToBoolean(char* str)
{
	if(strcmp(UpToLow(str), "true") == 0)
		return true;
	return false;
}

// Helper function, convert to lower case
char* pEffectReader::UpToLow(char* str) {
    for (int i=0;i<strlen(str);i++) 
		str[i] = tolower(str[i]);
    return str;
}