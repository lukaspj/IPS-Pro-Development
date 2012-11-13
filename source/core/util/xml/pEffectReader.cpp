#include "pEffectReader.h"
#include "platform\profiler.h"
#include "rapidxml.hpp"
#include <istream>
#include <fstream>
#include <vector>
using namespace std;

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

int pEffectReader::stringToValueType(char* str)
{
	UpToLow(str);
	if(!strcmp(str, "xposition"))
		return 0;
	if(!strcmp(str, "yposition"))
		return 1;
	if(!strcmp(str, "zposition"))
		return 2;
	if(!strcmp(str, "active"))
		return 3;
	if(!strcmp(str, "ejectionoffset"))
		return 4;
	if(!strcmp(str, "ejectionperiod"))
		return 5;
	if(!strcmp(str, "radius"))
		return 7;
	if(!strcmp(str, "upperboundary"))
		return 8;
	if(!strcmp(str, "lowerboundary"))
		return 9;
	if(!strcmp(str, "timescale"))
		return 10;
	if(!strcmp(str, "scale"))
		return 11;
	return 12;
}

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

bool pEffectReader::stringToBoolean(char* str)
{
	if(strcmp(UpToLow(str), "true") == 0)
		return true;
	return false;
}

char* pEffectReader::UpToLow(char* str) {
    for (int i=0;i<strlen(str);i++) 
		str[i] = tolower(str[i]);
    return str;
}
=======
#include "pEffectReader.h"
#include "platform\profiler.h"
#include "rapidxml.hpp"
#include <istream>
#include <fstream>
#include <vector>
using namespace std;

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
			val.name = std::string(attr->value());
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

int pEffectReader::stringToEmitterType(char* str)
{
	if(!strcmp(UpToLow(str), "stockemitter"))
		return 0;
	if(!strcmp(UpToLow(str), "graphemitter"))
		return 1;
	if(!strcmp(UpToLow(str), "meshemitter"))
		return 2;
	if(!strcmp(UpToLow(str), "radiusmeshemitter"))
		return 3;
	if(!strcmp(UpToLow(str), "groundemitter"))
		return 4;
	if(!strcmp(UpToLow(str), "maskemitter"))
		return 5;
	if(!strcmp(UpToLow(str), "pathemitter"))
		return 7;
	return 8;
}

S32 pEffectReader::stringToEase(char* str)
{
	if(!strcmp(UpToLow(str), "linear"))
		return 0;
	if(!strcmp(UpToLow(str), "quadratic"))
		return 1;
	if(!strcmp(UpToLow(str), "cubic"))
		return 2;
	if(!strcmp(UpToLow(str), "quartic"))
		return 3;
	if(!strcmp(UpToLow(str), "quintic"))
		return 4;
	if(!strcmp(UpToLow(str), "sinusoidal"))
		return 5;
	if(!strcmp(UpToLow(str), "exponential"))
		return 7;
	if(!strcmp(UpToLow(str), "circular"))
		return 7;
	if(!strcmp(UpToLow(str), "elastic"))
		return 7;
	if(!strcmp(UpToLow(str), "back"))
		return 7;
	if(!strcmp(UpToLow(str), "bounce"))
		return 7;
}

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

bool pEffectReader::stringToBoolean(char* str)
{
	if(strcmp(UpToLow(str), "true") == 0)
		return true;
	return false;
}

char* pEffectReader::UpToLow(char* str) {
    for (int i=0;i<strlen(str);i++) 
		str[i] = tolower(str[i]);
    return str;
}

