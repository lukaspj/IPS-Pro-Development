<<<<<<< HEAD
//-----------------------------------------------------------------------------
// IPS Pro
// Copyright Lukas J�rgensen 2012 - FuzzyVoidStudio
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

#include "graphEmitterNode.h"
#include "IPSCore.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "sim/netConnection.h"
#include "console/engineAPI.h"

IMPLEMENT_CO_DATABLOCK_V1(GraphEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(GraphEmitterNode);

ConsoleDocClass( GraphEmitterNodeData,
	"@brief Contains additional data to be associated with a ParticleEmitterNode."
	"@ingroup FX\n"
	);

ConsoleDocClass( GraphEmitterNode,
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

typedef GraphEmitterNode::EnumProgressMode gnProgressMode;
DefineEnumType( gnProgressMode );

ImplementEnumType( gnProgressMode,
	"The way the t value of the graphEmitter is increased.\n"
	"@ingroup FX\n\n")
{ GraphEmitter::byParticleCount,				"ParticleCount",	"Increase t value when a new particle is emitted.\n" },
{ GraphEmitter::byTime,							"ByTime",			"Increase t value with time.\n" },
EndImplementEnumType;

GraphEmitterNodeData::GraphEmitterNodeData()
{
	xFunc = "0";
	yFunc = "0";
	zFunc = "0";

	funcMax = 2000;
	funcMin = 0;

	ProgressMode = 0;

	Reverse = false;
	Loop = true;
	Grounded = false;
}

void GraphEmitterNodeData::initPersistFields()
{
	addGroup( "Expression");

	addField( "Grounded", TYPEID< bool >(), Offset(Grounded, GraphEmitterNodeData),
		"Ground the graphEmitter." );

	addField( "xFunc", TYPEID< StringTableEntry >(), Offset(xFunc, GraphEmitterNodeData),
		"The expression specifying the emitted particles X coordinate." );

	addField( "yFunc", TYPEID< StringTableEntry >(), Offset(yFunc, GraphEmitterNodeData),
		"The expression specifying the emitted particles Y coordinate." );

	addField( "zFunc", TYPEID< StringTableEntry >(), Offset(zFunc, GraphEmitterNodeData),
		"The expression specifying the emitted particles Z coordinate." );

	addField( "funcMax", TYPEID< S32 >(), Offset(funcMax, GraphEmitterNodeData),
		"TThe expressions interval, maximum." );

	addField( "funcMin", TYPEID< S32 >(), Offset(funcMin, GraphEmitterNodeData),
		"The expressions interval, minimum." );

	addField( "timeScale", TYPEID< F32 >(), Offset(timeScale, GraphEmitterNodeData),
		"The amount to scale the t value with." );

	addField( "ProgressMode", TYPEID< gnProgressMode >(), Offset(ProgressMode, GraphEmitterNodeData),
		"String value that controls how the t value is increased." );

	addField( "Reverse", TYPEID< bool >(), Offset(Reverse, GraphEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "Loop", TYPEID< bool >(), Offset(Loop, GraphEmitterNodeData),
		"Loop the graphEmitter." );

	endGroup( "Expression" );

	Parent::initPersistFields();
}

bool GraphEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	//Validate variables here

	return true;
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void GraphEmitterNodeData::packData(BitStream* stream)
{
	Parent::packData(stream);
	
	stream->writeString(xFunc);
	stream->writeString(yFunc);
	stream->writeString(zFunc);
	stream->writeInt(funcMax,32);
	stream->writeInt(funcMin,32);
	stream->writeInt(timeScale,32);
	stream->writeInt(ProgressMode, 1);
	stream->writeFlag(Reverse);
	stream->writeFlag(Loop);
	stream->writeFlag(Grounded);
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void GraphEmitterNodeData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	char buf[256];

	stream->readString(buf);
	xFunc = dStrdup(buf);

	stream->readString(buf);
	yFunc = dStrdup(buf);

	stream->readString(buf);
	zFunc = dStrdup(buf);

	funcMax = stream->readInt(32);
	funcMin = stream->readInt(32);

	timeScale = stream->readInt(32);

	ProgressMode = stream->readInt(1);
	Reverse = stream->readFlag();
	Loop = stream->readFlag();
	Grounded = stream->readFlag();
}

//-----------------------------------------------------------------------------
// SphereEmitterNode
//-----------------------------------------------------------------------------
GraphEmitterNode::GraphEmitterNode()
{
	particleProg = 0.0;

	xFunc = "0";
	yFunc = "0";
	zFunc = "0";

	funcMax = 2000;
	funcMin = 0;

	ProgressMode = 0;

	Reverse = false;
	Loop = true;
	Grounded = false;

	timeScale = 0.1f;

	for(int i=0;i<100;i++)
	{
		xVariables[i].token = NULL;
		yVariables[i].token = NULL;
		zVariables[i].token = NULL;
		xVariables[i].value = NULL;
		yVariables[i].value = NULL;
		zVariables[i].value = NULL;
	}

	xfuncParser.DefineVar("t", &particleProg);
	xfuncParser.SetExpr(xFunc);

	yfuncParser.DefineVar("t", &particleProg);
	yfuncParser.SetExpr(yFunc);

	zfuncParser.DefineVar("t", &particleProg);
	zfuncParser.SetExpr(zFunc);

	cb_Max = false;

	xMxDist = 0;
	xMnDist = 0;
	yMxDist = 0;
	yMnDist = 0;
	zMxDist = 0;
	zMnDist = 0;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GraphEmitterNode::initPersistFields()
{
	addGroup( "Expression");

	addField( "Grounded", TYPEID< bool >(), Offset(Grounded, GraphEmitterNode),
		"Ground the graphEmitter." );

	addField( "xFunc", TYPEID< StringTableEntry >(), Offset(xFunc, GraphEmitterNode),
		"The expression specifying the emitted particles X coordinate." );

	addField( "yFunc", TYPEID< StringTableEntry >(), Offset(yFunc, GraphEmitterNode),
		"The expression specifying the emitted particles Y coordinate." );

	addField( "zFunc", TYPEID< StringTableEntry >(), Offset(zFunc, GraphEmitterNode),
		"The expression specifying the emitted particles Z coordinate." );

	addField( "funcMax", TYPEID< S32 >(), Offset(funcMax, GraphEmitterNode),
		"TThe expressions interval, maximum." );

	addField( "funcMin", TYPEID< S32 >(), Offset(funcMin, GraphEmitterNode),
		"The expressions interval, minimum." );

	addField( "timeScale", TYPEID< F32 >(), Offset(timeScale, GraphEmitterNode),
		"The amount to scale the t value with." );

	addField( "ProgressMode", TYPEID< gnProgressMode >(), Offset(ProgressMode, GraphEmitterNode),
		"String value that controls how the t value is increased." );

	addField( "Reverse", TYPEID< bool >(), Offset(Reverse, GraphEmitterNode),
		"Reverse the graphEmitter." );

	addField( "Loop", TYPEID< bool >(), Offset(Loop, GraphEmitterNode),
		"Loop the graphEmitter." );

	endGroup( "Expression" );
	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool GraphEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	setmDataBlock(dynamic_cast<GraphEmitterNodeData*>( dptr ));
	GraphEmitterNodeData* DataBlock = getDataBlock();
	if ( !DataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	// Synchronize the expression data.
	xFunc = DataBlock->xFunc;
	yFunc = DataBlock->yFunc;
	zFunc = DataBlock->zFunc;

	funcMax = DataBlock->funcMax;
	funcMin = DataBlock->funcMin;

	ProgressMode = DataBlock->ProgressMode;

	Reverse = DataBlock->Reverse;
	if(Reverse)
		particleProg = funcMax;
	else
		particleProg = funcMin;
	Loop = DataBlock->Loop;
	Grounded = DataBlock->Grounded;

	timeScale = DataBlock->timeScale;

	for(int i = 0; i < initialValues.size(); i=i+2)
	{
		if(strcmp("xFunc",initialValues[i].c_str()) == 0)
			xFunc = const_cast<char*>(initialValues[i+1].c_str());
		if(strcmp("yFunc",initialValues[i].c_str()) == 0)
			yFunc = const_cast<char*>(initialValues[i+1].c_str());
		if(strcmp("zFunc",initialValues[i].c_str()) == 0)
			zFunc = const_cast<char*>(initialValues[i+1].c_str());
		if(strcmp("funcMax",initialValues[i].c_str()) == 0)
			funcMax = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("funcMin",initialValues[i].c_str()) == 0)
			funcMin = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("timeScale",initialValues[i].c_str()) == 0)
			timeScale = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("ProgressMode",initialValues[i].c_str()) == 0)
			ProgressMode = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Reverse",initialValues[i].c_str()) == 0)
			Reverse = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Loop",initialValues[i].c_str()) == 0)
			Loop = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Grounded",initialValues[i].c_str()) == 0)
			Grounded = atoi(const_cast<char*>(initialValues[i+1].c_str()));
	}

	xfuncParser.SetExpr(xFunc);
	yfuncParser.SetExpr(yFunc);
	zfuncParser.SetExpr(zFunc);

	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 GraphEmitterNode::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{	
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if( stream->writeFlag( mask & exprEdited) )
	{
		stream->writeString(xFunc);
		stream->writeString(yFunc);
		stream->writeString(zFunc);
		stream->writeInt(funcMax,32);
		stream->writeInt(funcMin,32);
		stream->writeInt(timeScale * 1000, 15);
		stream->writeInt(ProgressMode, 1);
		stream->writeFlag(Reverse);
		stream->writeFlag(Loop);
		stream->writeFlag(Grounded);
	}
	if( stream->writeFlag(mask & dynamicMod) )
	{
		int xmodCount = 0;
		for(int i=0;i<100;i++)
		{
			if(xVariables[i].token != NULL)
				xmodCount++;
		}

		int ymodCount = 0;
		for(int i=0;i<100;i++)
		{
			if(yVariables[i].token != NULL)
				ymodCount++;
		}

		int zmodCount = 0;
		for(int i=0;i<100;i++)
		{
			if(zVariables[i].token != NULL)
				zmodCount++;
		}
		stream->writeInt(xmodCount,7);
		stream->writeInt(ymodCount,7);
		stream->writeInt(zmodCount,7);
		for(int i=0;i<100;i++)
		{
			if(xVariables[i].token != NULL)
			{
				stream->writeString(&xVariables[i].token,3);
				stream->write(xVariables[i].value);
			}
		}
		for(int i=0;i<100;i++)
		{
			if(yVariables[i].token != NULL)
			{
				stream->writeString(&yVariables[i].token,3);
				stream->write(yVariables[i].value);
			}
		}
		for(int i=0;i<100;i++)
		{
			if(zVariables[i].token != NULL)
			{
				stream->writeString(&zVariables[i].token,3);
				stream->write(zVariables[i].value);
			}
		}
	}

	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void GraphEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);

	//ExprEditted
	if ( stream->readFlag() )
	{
		char buf[256];

		stream->readString(buf);
		xFunc = IPSCore::UpToLow(dStrdup(buf));

		stream->readString(buf);
		yFunc = IPSCore::UpToLow(dStrdup(buf));
		//yFunc = dStrdup(buf);

		stream->readString(buf);
		zFunc = IPSCore::UpToLow(dStrdup(buf));
		//zFunc = dStrdup(buf);

		xfuncParser.SetExpr(xFunc);

		yfuncParser.SetExpr(yFunc);

		zfuncParser.SetExpr(zFunc);

		funcMax = stream->readInt(32);
		funcMin = stream->readInt(32);

		timeScale = stream->readInt(15) / 1000.0f;
		ProgressMode = stream->readInt(1);
		Reverse = stream->readFlag();
		Loop = stream->readFlag();
		Grounded = stream->readFlag();
	}
	if( stream->readFlag() )
	{
		dMemset(xVariables,0,sizeof(xVariables));
		dMemset(yVariables,0,sizeof(yVariables));
		dMemset(zVariables,0,sizeof(zVariables));
		int xi = stream->readInt(7);
		int yi = stream->readInt(7);
		int zi = stream->readInt(7);

		char buf[3];

		for(int i=0;i<xi;i++)
		{
			stream->readString(buf);
			xVariables[i].token = *dStrdup(buf);
			stream->read((F32 *)&xVariables[i].value);

			varmap_type xVars = xfuncParser.GetVar();
			varmap_type::const_iterator item = xVars.begin();
			// We don't want to call the DefineVar unless it is absolutely necessary ( for optimization )
			bool found = false;
			for(; item!= xVars.end(); ++item)
			{
				if(IPSCore::UpToLow(const_cast<char*>(item->first.c_str())) == IPSCore::UpToLow(&xVariables[i].token))
					found = true;
			}
			if(!found)
			{
				xfuncParser.DefineVar(IPSCore::UpToLow(&xVariables[i].token), &xVariables[i].value);
			}
		}
		for(int i=0;i<yi;i++)
		{
			stream->readString(buf);
			yVariables[i].token = *dStrdup(buf);
			stream->read((F32 *)&yVariables[i].value);

			varmap_type yvars = yfuncParser.GetVar();
			varmap_type::const_iterator item = yvars.begin();
			bool found = false;
			for(; item!= yvars.end(); ++item)
			{
				if(IPSCore::UpToLow(const_cast<char*>(item->first.c_str())) == IPSCore::UpToLow(&yVariables[i].token))
					found = true;
			}
			if(!found)
				yfuncParser.DefineVar(IPSCore::UpToLow(&yVariables[i].token), &yVariables[i].value);
		}
		for(int i=0;i<zi;i++)
		{
			stream->readString(buf);
			zVariables[i].token = *dStrdup(buf);
			stream->read((F32 *)&zVariables[i].value);

			varmap_type zVars = zfuncParser.GetVar();
			varmap_type::const_iterator item = zVars.begin();
			bool found = false;
			for(; item!= zVars.end(); ++item)
			{
				if(IPSCore::UpToLow(const_cast<char*>(item->first.c_str())) == IPSCore::UpToLow(&zVariables[i].token))
					found = true;
			}
			if(!found)
				zfuncParser.DefineVar(IPSCore::UpToLow(&zVariables[i].token), &zVariables[i].value);
		}
	}
}

void GraphEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	// Was it an expression related value? Then set the exprEdited bit.
	if(strcmp(slotName, "xFunc") == 0 ||
		strcmp(slotName, "yFunc") == 0 ||
		strcmp(slotName, "zFunc") == 0 ||
		strcmp(slotName, "funcMax") == 0 ||
		strcmp(slotName, "funcMin") == 0 ||
		strcmp(slotName, "ProgressMode") == 0 ||
		strcmp(slotName, "Reverse") == 0 ||
		strcmp(slotName, "Loop") == 0 ||
		strcmp(slotName, "Grounded") == 0 ||
		strcmp(slotName, "timeScale") == 0)
	{
		setMaskBits(exprEdited);
		updateMaxMinDistances();
	}

	// If it was a function, update the parsers 
	// ( we don't want it to be case sensitive hence the IPSCore::UpToLow )
	if(strcmp(slotName, "xFunc") == 0)
		xfuncParser.SetExpr(IPSCore::UpToLow(xFunc));
	if(strcmp(slotName, "yFunc") == 0)
		yfuncParser.SetExpr(IPSCore::UpToLow(yFunc));
	if(strcmp(slotName, "zFunc") == 0)
		zfuncParser.SetExpr(IPSCore::UpToLow(zFunc));

	Parent::onStaticModified(slotName, newValue);
}

void GraphEmitterNode::onDynamicModified(const char* slotName, const char*newValue)
{
	/**Syntax check the dynamic field to see if it follows the pattern:
	CoorVarIndexToken e.g. xVar1c**/
	if(slotName[0] == 'x' || slotName[0] == 'X' ||
		slotName[0] == 'y' || slotName[0] == 'Y' ||
		slotName[0] == 'z' || slotName[0] == 'Z')
	{
		if(slotName[1] == 'v' || slotName[1] == 'V')
		{
			if(slotName[2] == 'a' || slotName[2] == 'A')
			{
				if(slotName[3] == 'r' || slotName[3] == 'R')
				{
					//Is the 5th bit a digit? If it is not the syntax is wrong
					if(isdigit(slotName[4]))
					{
						U8 index = atoi(&slotName[4]);
						char name;

						int ccnt = 0;
						//If the 6th bit is a digit as well, append the bit to the index as the
						// - least significant bit
						if(isdigit(slotName[5]))
						{
							//U8 second = atoi(&slotName[4]); I don't know why this isn't needed but it isn't
							char combined[3];
							strcpy(combined, &slotName[4]);
							index = atoi(combined);
						}
						else if(!isalpha(slotName[5]))
						{
							// If slotName bit 5 isn't a digit and it isn't an alphabetical character something must be wrong.
							Con::warnf("Dynamic variable specified on graphEmitter %s but no token set", getIdString());
							return;
						}
						else{
							//Else add slotName bit 5 to bit 0 of name.
							//name[ccnt] = slotName[5];
							name = slotName[5];
							ccnt++;
						}
						/*if(isalpha(slotName[6])){
						name[ccnt] = slotName[6];
						ccnt++;
						if(isalpha(slotName[7])){
						name[ccnt] = slotName[7];
						ccnt++;
						}
						}*/
						//If name has been set to something
						if(name != NULL)
						{
							F32 value = atof(newValue);
							if(isdigit(*newValue))
							{
								if(slotName[0] == 'x' || slotName[0] == 'X')
								{
									// If we already have a variable with that name and it is not equal to the current token
									//  - then we want to remove it before we define it again.
									if(xVariables[index].token != NULL && xVariables[index].token != name)
									{
										xfuncParser.RemoveVar(IPSCore::UpToLow(&xVariables[index].token));
									}
									// Add the new variable to the xVariables list
									xVariables[index].value = value;
									xVariables[index].token = name;
									std::string s;
									std::stringstream ss;
									ss << xVariables[index].token;
									ss >> s;
									try{
										xfuncParser.DefineVar(s,&xVariables[index].value);
									}
									catch(mu::Parser::exception_type &e)
									{
										std::string expr = e.GetExpr();
										std::string tok = e.GetToken();
										size_t pos = e.GetPos();
										std::string msg = e.GetMsg();
										Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
									}
									// If we are running this on the server, be sure to update the client aswell.
									if(isServerObject())
										setMaskBits(dynamicMod);
								}
								if(slotName[0] == 'y' || slotName[0] == 'Y')
								{
									if(yVariables[index].token != NULL && yVariables[index].token != name)
									{
										yfuncParser.RemoveVar(IPSCore::UpToLow(&yVariables[index].token));
									}
									yVariables[index].value = value;
									yVariables[index].token = name;
									std::string s;
									std::stringstream ss;
									ss << xVariables[index].token;
									ss >> s;
									try{
										yfuncParser.DefineVar(s,&yVariables[index].value);
									}
									catch(mu::Parser::exception_type &e)
									{
										std::string expr = e.GetExpr();
										std::string tok = e.GetToken();
										size_t pos = e.GetPos();
										std::string msg = e.GetMsg();
										Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
									}

									if(isServerObject())
										setMaskBits(dynamicMod);
								}
								if(slotName[0] == 'z' || slotName[0] == 'Z')
								{
									if(zVariables[index].token != NULL && zVariables[index].token != name)
									{
										zfuncParser.RemoveVar(IPSCore::UpToLow(&zVariables[index].token));
									}
									zVariables[index].value = value;
									zVariables[index].token = name;
									std::string s;
									std::stringstream ss;
									ss << xVariables[index].token;
									ss >> s;
									try{
										zfuncParser.DefineVar(s,&zVariables[index].value);
									}
									catch(mu::Parser::exception_type &e)
									{
										std::string expr = e.GetExpr();
										std::string tok = e.GetToken();
										size_t pos = e.GetPos();
										std::string msg = e.GetMsg();
										Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
									}

									if(isServerObject())
										setMaskBits(dynamicMod);
								}
							}
						}
					}
					else
					{
						Con::warnf("Dynamic variable specified on graphEmitter %s but no index set", getIdString());
					}
				}
			}
		}
	}
}

void GraphEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	GraphEmitterNodeData* DataBlock = getDataBlock();
	if(!mActive || !mEmitter->isProperlyAdded() || !DataBlock)
		return;

	mEmitter->emitParticles( (U32)(dt * DataBlock->timeMultiple * 1000.0f), this );
}

void GraphEmitterNode::updateMaxMinDistances()
{
	F32 tmpPartProg = particleProg;
	xMxDist = 0;
	xMnDist = 0;
	yMxDist = 0;
	yMnDist = 0;
	zMxDist = 0;
	zMnDist = 0;
	
	for(int i = funcMin; i <= funcMax; i++)
	{
		F32 xRes;
		F32 yRes;
		F32 zRes;
		particleProg = i;
		try{
			xRes = xfuncParser.Eval() * sa_ejectionOffset;
			yRes = yfuncParser.Eval() * sa_ejectionOffset;
			zRes = zfuncParser.Eval() * sa_ejectionOffset;
		}
		catch(mu::Parser::exception_type &e)
		{
			// Most likely cause is: The expression were updated before the dynamic variables were
			//  Don't want it to throw an error all the time, even tho it should throw an exception
			//  When thing really does go wrong -- Needs fix.
			//std::string expr = e.GetExpr();
			//std::string tok = e.GetToken();
			//size_t pos = e.GetPos();
			//std::string msg = e.GetMsg();
			//Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
			break;
		}
		catch(...) { }

		if(xRes > xMxDist)
			xMxDist = xRes;
		if(xRes < xMnDist)
			xMnDist = xRes;

		if(yRes > yMxDist)
			yMxDist = yRes;
		if(yRes < yMnDist)
			yMnDist = yRes;

		if(zRes > zMxDist)
			zMxDist = zRes;
		if(zRes < zMnDist)
			zMnDist = zRes;
	}
	particleProg = tmpPartProg;
	if(xMxDist == 0)
		xMxDist = 0.5;
	if(xMnDist == 0)
		xMnDist = -0.5;
	if(yMxDist == 0)
		yMxDist = 0.5;
	if(yMnDist == 0)
		yMnDist = -0.5;
	if(zMxDist == 0)
		zMxDist = 0.5;
	if(zMnDist == 0)
		zMnDist = -0.5;
}

void GraphEmitterNode::setEmitterDataBlock(ParticleEmitterData* data)
{
	Parent::setEmitterDataBlock(data);
}

//-----Netevent---
// Document this later.
nodeCallbackEvent::nodeCallbackEvent(S32 id, bool max)
{
	mNode = id;
	Max = max;
}

nodeCallbackEvent::~nodeCallbackEvent()
{
	//dFree(&Max);
	//dFree(&mNode);
}

void nodeCallbackEvent::pack(NetConnection* conn, BitStream *bstream)
{
	bstream->write(Max);
	bstream->write(mNode);
}

void nodeCallbackEvent::unpack(NetConnection *conn, BitStream *bstream)
{
	bstream->read(&Max);
	bstream->read(&mNode);
}

// This just prints the event in the console. You might
// want to do something more clever here -- BJG
void nodeCallbackEvent::process(NetConnection *conn)
{
	try{
		GraphEmitterNode* m_Node = dynamic_cast< GraphEmitterNode* >(conn->resolveObjectFromGhostIndex(mNode));
		if(m_Node)
			m_Node->onBoundaryLimit(Max);
	}
	catch(...){
		Con::printf("Callback error");
	}
}

void nodeCallbackEvent::write(NetConnection*, BitStream *bstream)
{
	bstream->write(Max);
}

IMPLEMENT_CO_NETEVENT_V1(nodeCallbackEvent);

IMPLEMENT_CALLBACK(GraphEmitterNodeData, onBoundaryLimit, void, ( GameBase* obj, bool Max), ( obj, Max ),
	"@brief Informs a graphEmitter that it has hit a boundary and is now resetting the t value.n"
	"@param Max - did it hit the maximum boundary or the minimum boundary?.n"
	);

//-----------------------------------------------------------------------------
// The onBoundaryLimit callback handler.
//-----------------------------------------------------------------------------
void GraphEmitterNode::onBoundaryLimit(bool Max)
{
	cb_Max = Max;
	if (getDataBlock() && !isGhost())
		getDataBlock()->onBoundaryLimit_callback( this, Max );
	else
	{
		NetConnection* conn = NetConnection::getConnectionToServer();
		if(conn)
			conn->postNetEvent(new nodeCallbackEvent(conn->getGhostIndex(this), Max));
	}
=======
//-----------------------------------------------------------------------------
// IPS Pro
// Copyright Lukas J�rgensen 2012 - FuzzyVoidStudio
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

#include "graphEmitterNode.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "graphEmitter.h"
#include "math/mathIO.h"
#include "sim/netConnection.h"
#include "console/engineAPI.h"

IMPLEMENT_CO_DATABLOCK_V1(GraphEmitterNodeData);
IMPLEMENT_CO_NETOBJECT_V1(GraphEmitterNode);

ConsoleDocClass( GraphEmitterNodeData,
	"@brief Contains additional data to be associated with a GraphEmitterNode."
	"@ingroup FX\n"
	);

ConsoleDocClass( GraphEmitterNode,
	"@brief A particle emitter object that can be positioned in the world and "
	"dynamically enabled or disabled.\n\n"

	"@tsexample\n"
	"datablock GraphEmitterNodeData( SimpleEmitterNodeData )\n"
	"{\n"
	"   timeMultiple = 1.0;\n"
	"};\n\n"

	"%emitter = new GraphEmitterNode()\n"
	"{\n"
	"   datablock = SimpleEmitterNodeData;\n"
	"   active = true;\n"
	"   emitter = FireEmitterData;\n"
	"   velocity = 3.5;\n"
	"};\n\n"

	"// Dynamically change emitter datablock\n"
	"%emitter.setEmitterDataBlock( DustEmitterData );\n"
	"@endtsexample\n"

	"@ingroup FX\n"
	"@see GraphEmitterNodeData\n"
	"@see GraphEmitterData\n"
	);

//-----------------------------------------------------------------------------
// GraphEmitterNodeData
//-----------------------------------------------------------------------------
GraphEmitterNodeData::GraphEmitterNodeData()
{
	timeMultiple = 1.0;

	xFunc = "0";
	yFunc = "0";
	zFunc = "0";

	funcMax = 2000;
	funcMin = 0;

	ProgressMode = 0;

	Reverse = false;
	Loop = true;
	Grounded = false;

	timeScale = 0.1f;

	attractionrange = 50;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = 0;
		Amount[i] = 1;
		Attraction_offset[i] = "0 0 0";
	}
	sticky = false;
	ParticleCollision = false;

	standAloneEmitter = false;

	sa_ejectionPeriodMS = 100;    // 10 Graphs Per second
	sa_periodVarianceMS = 0;      // exactly

	sa_ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	sa_velocityVariance = 1.0f;
	sa_ejectionOffset   = 0.0f;   // ejection from the emitter point
}

GraphEmitterNodeData::~GraphEmitterNodeData()
{

}


typedef GraphEmitterNode::EnumProgressMode gProgressMode;
DefineEnumType( gProgressMode );

ImplementEnumType( gProgressMode,
	"The way the t value of the graphEmitter is increased.\n"
	"@ingroup FX\n\n")
{ GraphEmitterNode::byParticleCount,				"ParticleCount",	"Increase t value when a new particle is emitted.\n" },
{ GraphEmitterNode::byTime,							"ByTime",			"Increase t value with time.\n" },
EndImplementEnumType;

typedef GraphEmitterNode::EnumAttractionMode gAttractionMode;
DefineEnumType( gAttractionMode );

ImplementEnumType( gAttractionMode,
	"The way the particles are interacting with specific objects.\n"
	"@ingroup FX\n\n")
{ GraphEmitterNode::none,			"Not attracted",        "The particles are not attracted to any object.\n" },
{ GraphEmitterNode::attract,		"Attract",				"The particles are attracted to the objects.\n" },
{ GraphEmitterNode::repulse,		"Repulse",				"The particles are repulsed by the object.\n" },
EndImplementEnumType;

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GraphEmitterNodeData::initPersistFields()
{
	addField( "timeMultiple", TYPEID< F32 >(), Offset(timeMultiple, GraphEmitterNodeData),
		"@brief Time multiplier for particle emitter nodes.\n\n"
		"Increasing timeMultiple is like running the emitter at a faster rate - single-shot "
		"emitters will complete in a shorter time, and continuous emitters will generate "
		"particles more quickly.\n\n"
		"Valid range is 0.01 - 100." );

	addGroup( "Independent emitters");

	addField( "standAloneEmitter", TYPEID< bool >(), Offset(standAloneEmitter, GraphEmitterNodeData),
		"@brief If true, this datablock is not connected other datablocks of the same type .\n"
		"Useful for animated datablocks." );

	addField("sa_ejectionPeriodMS", TYPEID< S32 >(), Offset(sa_ejectionPeriodMS,   GraphEmitterNodeData),
		"Time (in milliseconds) between each particle ejection." );

	addField("sa_periodVarianceMS", TYPEID< S32 >(), Offset(sa_periodVarianceMS,   GraphEmitterNodeData),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "sa_ejectionVelocity", TYPEID< F32 >(), Offset(sa_ejectionVelocity, GraphEmitterNodeData),
		"Particle ejection velocity." );

	addField( "sa_velocityVariance", TYPEID< F32 >(), Offset(sa_velocityVariance, GraphEmitterNodeData),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "sa_ejectionOffset", TYPEID< F32 >(), Offset(sa_ejectionOffset, GraphEmitterNodeData),
		"The amount to scale the expression with." );

	endGroup( "Independent emitters" );

	addGroup( "Expression");

	addField( "Grounded", TYPEID< bool >(), Offset(Grounded, GraphEmitterNodeData),
		"Ground the graphEmitter." );

	addField( "xFunc", TYPEID< StringTableEntry >(), Offset(xFunc, GraphEmitterNodeData),
		"The expression specifying the emitted particles X coordinate." );

	addField( "yFunc", TYPEID< StringTableEntry >(), Offset(yFunc, GraphEmitterNodeData),
		"The expression specifying the emitted particles Y coordinate." );

	addField( "zFunc", TYPEID< StringTableEntry >(), Offset(zFunc, GraphEmitterNodeData),
		"The expression specifying the emitted particles Z coordinate." );

	addField( "funcMax", TYPEID< S32 >(), Offset(funcMax, GraphEmitterNodeData),
		"TThe expressions interval, maximum." );

	addField( "funcMin", TYPEID< S32 >(), Offset(funcMin, GraphEmitterNodeData),
		"The expressions interval, minimum." );

	addField( "timeScale", TYPEID< F32 >(), Offset(timeScale, GraphEmitterNodeData),
		"The amount to scale the t value with." );

	addField( "ProgressMode", TYPEID< GraphEmitterNode::EnumProgressMode >(), Offset(ProgressMode, GraphEmitterNodeData),
		"String value that controls how the t value is increased." );

	addField( "Reverse", TYPEID< bool >(), Offset(Reverse, GraphEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "Loop", TYPEID< bool >(), Offset(Loop, GraphEmitterNodeData),
		"Loop the graphEmitter." );

	endGroup( "Expression" );

	addGroup( "Physics" );

	addField( "RayCollision", TYPEID< bool >(), Offset(ParticleCollision, GraphEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "AttractionMode", TYPEID< GraphEmitterNode::EnumAttractionMode >(), Offset(AttractionMode, GraphEmitterNodeData), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Attraction range", TYPEID< F32 >(), Offset(attractionrange, GraphEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, GraphEmitterNodeData), attrobjectCount,
		"Reverse the graphEmitter." );

	addField( "Sticky", TYPEID< bool >(), Offset(sticky, GraphEmitterNodeData),
		"Reverse the graphEmitter." );

	addField( "Attraction_offset;", TYPEID< StringTableEntry >(), Offset(Attraction_offset, GraphEmitterNodeData), attrobjectCount,
		"testReverse the graphEmitter." );

	endGroup( "Physics" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool GraphEmitterNodeData::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if( timeMultiple < 0.01 || timeMultiple > 100 )
	{
		Con::warnf("GraphEmitterNodeData::onAdd(%s): timeMultiple must be between 0.01 and 100", getName());
		timeMultiple = timeMultiple < 0.01 ? 0.01 : 100;
	}


	return true;
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool GraphEmitterNodeData::preload(bool server, String &errorStr)
{
	if( Parent::preload(server, errorStr) == false )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void GraphEmitterNodeData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->write(timeMultiple);

	stream->writeString(xFunc);
	stream->writeString(yFunc);
	stream->writeString(zFunc);
	stream->writeInt(funcMax,32);
	stream->writeInt(funcMin,32);
	stream->writeInt(timeScale,32);
	stream->writeInt(ProgressMode, 1);
	stream->writeFlag(Reverse);
	stream->writeFlag(Loop);
	stream->writeFlag(Grounded);
	stream->writeInt(sa_ejectionPeriodMS, 10);
	stream->writeInt(sa_periodVarianceMS, 10);
	stream->writeFloat(sa_ejectionVelocity, 15);
	stream->writeFloat(sa_velocityVariance, 15);
	stream->writeFloat(sa_ejectionOffset, 15);
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void GraphEmitterNodeData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	stream->read(&timeMultiple);


	char buf[256];

	stream->readString(buf);
	xFunc = dStrdup(buf);

	stream->readString(buf);
	yFunc = dStrdup(buf);

	stream->readString(buf);
	zFunc = dStrdup(buf);

	funcMax = stream->readInt(32);
	funcMin = stream->readInt(32);

	timeScale = stream->readInt(32);

	ProgressMode = stream->readInt(1);
	Reverse = stream->readFlag();
	Loop = stream->readFlag();
	Grounded = stream->readFlag();

	sa_ejectionPeriodMS = stream->readInt(10);
	sa_periodVarianceMS = stream->readInt(10);
	sa_ejectionVelocity = stream->readFloat(15);
	sa_velocityVariance = stream->readFloat(15);
	sa_ejectionOffset = stream->readFloat(15);
}

//-----------------------------------------------------------------------------
// GraphEmitterNode
//-----------------------------------------------------------------------------
GraphEmitterNode::GraphEmitterNode()
{
	// Todo: ScopeAlways?
	mNetFlags.set(Ghostable);
	mTypeMask |= EnvironmentObjectType;

	mActive = true;
	shuttingDown = false;

	mDataBlock          = NULL;
	mEmitterDatablock   = NULL;
	mEmitterDatablockId = 0;
	mEmitter            = NULL;
	mVelocity           = 1.0;

	standAloneEmitter = false;

	sa_ejectionPeriodMS = 100;    // 10 Graphs Per second
	sa_periodVarianceMS = 0;      // exactly

	sa_ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
	sa_velocityVariance = 1.0f;
	sa_ejectionOffset   = 0.0f;   // ejection from the emitter point

	particleProg = 0.0;

	xFunc = "0";
	yFunc = "0";
	zFunc = "0";

	funcMax = 2000;
	funcMin = 0;

	ProgressMode = 0;


	sticky = false;
	ParticleCollision = false;
	attractionrange = 50;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = 0;
		Amount[i] = 1;
		Attraction_offset[i] = "0 0 0";
		attractedObjectID[i] = "";
	}

	Reverse = false;
	Loop = true;
	Grounded = false;

	timeScale = 0.1f;

	for(int i=0;i<100;i++)
	{
		xVariables[i].token = NULL;
		yVariables[i].token = NULL;
		zVariables[i].token = NULL;
		xVariables[i].value = NULL;
		yVariables[i].value = NULL;
		zVariables[i].value = NULL;
	}

	xfuncParser.DefineVar("t", &particleProg);
	xfuncParser.SetExpr(UpToLow(xFunc));

	yfuncParser.DefineVar("t", &particleProg);
	yfuncParser.SetExpr(UpToLow(yFunc));

	zfuncParser.DefineVar("t", &particleProg);
	zfuncParser.SetExpr(UpToLow(zFunc));

	cb_Max = false;
	initialValues = *new std::vector<std::string>();

	xMxDist = 0;
	xMnDist = 0;
	yMxDist = 0;
	yMnDist = 0;
	zMxDist = 0;
	zMnDist = 0;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GraphEmitterNode::~GraphEmitterNode()
{
	//
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GraphEmitterNode::initPersistFields()
{
	addField( "active", TYPEID< bool >(), Offset(mActive,GraphEmitterNode),
		"Controls whether particles are emitted from this node." );
	addField( "emitter",  TYPEID< GraphEmitterData >(), Offset(mEmitterDatablock, GraphEmitterNode),
		"Datablock to use when emitting particles." );
	addField( "velocity", TYPEID< F32 >(), Offset(mVelocity, GraphEmitterNode),
		"Velocity to use when emitting particles (in the direction of the "
		"GraphEmitterNode object's up (Z) axis)." );

	// Add our variables to the worldeditor

	addGroup( "Independent emitters");

	addField( "standAloneEmitter", TYPEID< bool >(), Offset(standAloneEmitter, GraphEmitterNode),
		"@brief If true, this datablock is not connected other datablocks of the same type .\n"
		"Useful for animated datablocks." );

	addField("sa_ejectionPeriodMS", TYPEID< S32 >(), Offset(sa_ejectionPeriodMS,   GraphEmitterNode),
		"Time (in milliseconds) between each particle ejection." );

	addField("sa_periodVarianceMS", TYPEID< S32 >(), Offset(sa_periodVarianceMS,   GraphEmitterNode),
		"Variance in ejection period, from 1 - ejectionPeriodMS." );

	addField( "sa_ejectionVelocity", TYPEID< F32 >(), Offset(sa_ejectionVelocity, GraphEmitterNode),
		"Particle ejection velocity." );

	addField( "sa_velocityVariance", TYPEID< F32 >(), Offset(sa_velocityVariance, GraphEmitterNode),
		"Variance for ejection velocity, from 0 - ejectionVelocity." );

	addField( "sa_ejectionOffset", TYPEID< F32 >(), Offset(sa_ejectionOffset, GraphEmitterNode),
		"The amount to scale the expression with." );

	endGroup( "Independent emitters" );

	addGroup( "Expression");

	addField( "Grounded", TYPEID< bool >(), Offset(Grounded, GraphEmitterNode),
		"Ground the graphEmitter." );

	addField( "xFunc", TYPEID< StringTableEntry >(), Offset(xFunc, GraphEmitterNode),
		"The expression specifying the emitted particles X coordinate." );

	addField( "yFunc", TYPEID< StringTableEntry >(), Offset(yFunc, GraphEmitterNode),
		"The expression specifying the emitted particles Y coordinate." );

	addField( "zFunc", TYPEID< StringTableEntry >(), Offset(zFunc, GraphEmitterNode),
		"The expression specifying the emitted particles Z coordinate." );

	addField( "funcMax", TYPEID< S32 >(), Offset(funcMax, GraphEmitterNode),
		"TThe expressions interval, maximum." );

	addField( "funcMin", TYPEID< S32 >(), Offset(funcMin, GraphEmitterNode),
		"The expressions interval, minimum." );

	addField( "timeScale", TYPEID< F32 >(), Offset(timeScale, GraphEmitterNode),
		"The amount to scale the t value with." );

	addField( "ProgressMode", TYPEID< gProgressMode >(), Offset(ProgressMode, GraphEmitterNode),
		"String value that controls how the t value is increased." );

	addField( "Reverse", TYPEID< bool >(), Offset(Reverse, GraphEmitterNode),
		"Reverse the graphEmitter." );

	addField( "Loop", TYPEID< bool >(), Offset(Loop, GraphEmitterNode),
		"Loop the graphEmitter." );

	endGroup( "Expression" );

	addGroup( "Physics" );

	addField( "RayCollision", TYPEID< bool >(), Offset(ParticleCollision, GraphEmitterNode),
		"If true, the particles will stick to their original spawn place relative to the emitter." );

	addField( "attractedObjectID", TYPEID< StringTableEntry >(), Offset(attractedObjectID, GraphEmitterNode), attrobjectCount,
		"The ID or name of the object that the particles should interact with." );

	addField( "Attraction_offset", TYPEID< StringTableEntry >(), Offset(Attraction_offset, GraphEmitterNode), attrobjectCount,
		"Offset from the objects position the particles should be attracted to or repulsed from." );

	addField( "AttractionMode", TYPEID< gAttractionMode >(), Offset(AttractionMode, GraphEmitterNode), attrobjectCount,
		"String value that controls how the particles interact." );

	addField( "Amount", TYPEID< F32 >(), Offset(Amount, GraphEmitterNode), attrobjectCount,
		"Amount of influence, combine with attraction range for the desired result." );

	addField( "attractionrange", TYPEID< F32 >(), Offset(attractionrange, GraphEmitterNode),
		"Range of influence, any objects further away than this length will not attract or repulse the particles." );

	addField( "sticky", TYPEID< bool >(), Offset(sticky, GraphEmitterNode),
		"If true, the particles will stick to their original spawn place relative to the emitter." );

	endGroup( "Physics" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool GraphEmitterNode::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if( !mEmitterDatablock && mEmitterDatablockId != 0 )
	{
		if( Sim::findObject(mEmitterDatablockId, mEmitterDatablock) == false )
			Con::errorf(ConsoleLogEntry::General, "GraphEmitterNode::onAdd: Invalid packet, bad datablockId(mEmitterDatablock): %d", mEmitterDatablockId);
	}

	if( isClientObject() )
	{
		setEmitterDataBlock( mEmitterDatablock );
	}
	else
	{
		setMaskBits( StateMask | EmitterDBMask );
	}

	mObjBox.minExtents.set(-0.5, -0.5, -0.5);
	mObjBox.maxExtents.set( 0.5,  0.5,  0.5);
	resetWorldBox();
	addToScene();

	return true;
}

//-----------------------------------------------------------------------------
// safeDelete
//-----------------------------------------------------------------------------
//Not used
void GraphEmitterNode::safeDelete()
{
	if(!shuttingDown)
	{
		shuttingDown = true;
		if(isServerObject()){
			Con::printf("ServerSafeDelete");
			//setMaskBits( StateMask );
		}
		else
			Con::printf("ClientSafeDelete");
		//this->safeDeleteObject();
		this->deleteObject();
	}
}

//-----------------------------------------------------------------------------
// onRemove
//-----------------------------------------------------------------------------
void GraphEmitterNode::onRemove()
{
	removeFromScene();
	if( isClientObject() )
	{
		if( mEmitter )
		{
			mEmitter->deleteWhenEmpty();
			mEmitter = NULL;
		}
	}

	Parent::onRemove();
}

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool GraphEmitterNode::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	mDataBlock = dynamic_cast<GraphEmitterNodeData*>( dptr );
	if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	// Synchronize the expression data.
	xFunc = mDataBlock->xFunc;
	yFunc = mDataBlock->yFunc;
	zFunc = mDataBlock->zFunc;

	xfuncParser.SetExpr(xFunc);
	yfuncParser.SetExpr(yFunc);
	zfuncParser.SetExpr(zFunc);

	funcMax = mDataBlock->funcMax;
	funcMin = mDataBlock->funcMin;

	ProgressMode = mDataBlock->ProgressMode;

	Reverse = mDataBlock->Reverse;
	if(Reverse)
		particleProg = funcMax;
	else
		particleProg = funcMin;
	Loop = mDataBlock->Loop;
	Grounded = mDataBlock->Grounded;

	timeScale = mDataBlock->timeScale;
	for(int i = 0; i < attrobjectCount; i++)
	{
		AttractionMode[i] = mDataBlock->AttractionMode[i];
		Amount[i] = mDataBlock->Amount[i];
		Attraction_offset[i] = mDataBlock->Attraction_offset[i];
	}
	attractionrange = mDataBlock->attractionrange;
	sticky = mDataBlock->sticky;
	ParticleCollision = mDataBlock->ParticleCollision;

	standAloneEmitter = mDataBlock->standAloneEmitter;
	sa_ejectionPeriodMS = mDataBlock->sa_ejectionPeriodMS;
	sa_periodVarianceMS = mDataBlock->sa_periodVarianceMS;

	sa_ejectionVelocity = mDataBlock->sa_ejectionVelocity;
	sa_velocityVariance = mDataBlock->sa_velocityVariance;
	sa_ejectionOffset = mDataBlock->sa_ejectionOffset;

	if(mEmitter)
	{
		for(int i = 0; i < attrobjectCount; i++)
		{
			mEmitter->AttractionMode[i] = AttractionMode[i];
			mEmitter->Amount[i] = Amount[i];
			mEmitter->Attraction_offset[i] = Attraction_offset[i];
		}
		mEmitter->sticky = sticky;
		mEmitter->ParticleCollision = ParticleCollision;
		mEmitter->attractionrange = attractionrange;
	}

	for(int i = 0; i < initialValues.size(); i=i+2)
	{
		if(strcmp("xFunc",initialValues[i].c_str()) == 0)
			xFunc = const_cast<char*>(initialValues[i+1].c_str());
		if(strcmp("yFunc",initialValues[i].c_str()) == 0)
			yFunc = const_cast<char*>(initialValues[i+1].c_str());
		if(strcmp("zFunc",initialValues[i].c_str()) == 0)
			zFunc = const_cast<char*>(initialValues[i+1].c_str());
		if(strcmp("funcMax",initialValues[i].c_str()) == 0)
			funcMax = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("funcMin",initialValues[i].c_str()) == 0)
			funcMin = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("timeScale",initialValues[i].c_str()) == 0)
			timeScale = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("ProgressMode",initialValues[i].c_str()) == 0)
			ProgressMode = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Reverse",initialValues[i].c_str()) == 0)
			Reverse = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Loop",initialValues[i].c_str()) == 0)
			Loop = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("attractionrange",initialValues[i].c_str()) == 0)
			attractionrange = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sticky",initialValues[i].c_str()) == 0)
			sticky = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("RayCollision",initialValues[i].c_str()) == 0)
			ParticleCollision = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Grounded",initialValues[i].c_str()) == 0)
			Grounded = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("standAloneEmitter",initialValues[i].c_str()) == 0)
			standAloneEmitter = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_ejectionPeriodMS",initialValues[i].c_str()) == 0)
			sa_ejectionPeriodMS = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_periodVarianceMS",initialValues[i].c_str()) == 0)
			sa_periodVarianceMS = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_ejectionVelocity",initialValues[i].c_str()) == 0)
			sa_ejectionVelocity = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_velocityVariance",initialValues[i].c_str()) == 0)
			sa_velocityVariance = atoi(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("sa_ejectionOffset",initialValues[i].c_str()) == 0)
			sa_ejectionOffset = atof(const_cast<char*>(initialValues[i+1].c_str()));
		if(strcmp("Attraction_offset",initialValues[i].c_str()) == 0)
		{
			Attraction_offset[0] = const_cast<char*>(initialValues[i+1].c_str());
			Attraction_offset[1] = const_cast<char*>(initialValues[i+2].c_str());
			i++;
		}
		if(strcmp("attractedObjectID",initialValues[i].c_str()) == 0)
		{
			attractedObjectID[0] = const_cast<char*>(initialValues[i+1].c_str());
			attractedObjectID[1] = const_cast<char*>(initialValues[i+2].c_str());
			i++;
		}
		if(strcmp("AttractionMode",initialValues[i].c_str()) == 0)
		{
			AttractionMode[0] = atoi(const_cast<char*>(initialValues[i+1].c_str()));
			AttractionMode[1] = atoi(const_cast<char*>(initialValues[i+2].c_str()));
			i++;
		}
		if(strcmp("Amount",initialValues[i].c_str()) == 0)
		{
			Amount[0] = atoi(const_cast<char*>(initialValues[i+1].c_str()));
			Amount[1] = atoi(const_cast<char*>(initialValues[i+2].c_str()));
			i++;
		}
	}

	// Todo: Uncomment if this is a "leaf" class
	scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
void GraphEmitterNode::inspectPostApply()
{
	Parent::inspectPostApply();
	setMaskBits(StateMask | EmitterDBMask);
}

//-----------------------------------------------------------------------------
// advanceTime
//-----------------------------------------------------------------------------
void GraphEmitterNode::processTick(const Move* move)
{
	Parent::processTick(move);
	bool server = false;
	if(isServerObject())
		server = true;
	else
		server = false;

	mObjBox.minExtents = Point3F(xMnDist, yMnDist, zMnDist);
	mObjBox.maxExtents = Point3F(xMxDist, yMxDist, zMxDist);
	resetWorldBox();

	if ( isMounted() )
	{
		MatrixF mat;
		mMount.object->getMountTransform( mMount.node, mMount.xfm, &mat );
		setTransform( mat );
	}
}

void GraphEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);

	if(!mActive || mEmitter.isNull() || !mDataBlock)
		return;

	Point3F emitPoint, emitVelocity;
	Point3F emitAxis(0, 0, 1);
	getTransform().mulV(emitAxis);
	getTransform().getColumn(3, &emitPoint);
	emitVelocity = emitAxis * mVelocity;
	// Here we also send 'this' so the emitter can use the nodes data.
	mEmitter->emitParticles(emitPoint, emitPoint,
		emitAxis,
		emitVelocity, (U32)(dt * mDataBlock->timeMultiple * 1000.0f), this);
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 GraphEmitterNode::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
	U32 retMask = Parent::packUpdate(con, mask, stream);

	if ( stream->writeFlag( mask & InitialUpdateMask ) )
	{
		mathWrite(*stream, getTransform());
		mathWrite(*stream, getScale());
	}

	if ( stream->writeFlag( mask & EmitterDBMask ) )
	{
		if( stream->writeFlag(mEmitterDatablock != NULL) )
		{
			stream->writeRangedU32(mEmitterDatablock->getId(), DataBlockObjectIdFirst,
				DataBlockObjectIdLast);
		}
	}

	if ( stream->writeFlag( mask & StateMask ) )
	{
		stream->writeFlag( mActive );
		stream->writeFloat( mVelocity, 15 );
		stream->writeFlag( standAloneEmitter );
		stream->writeFlag( shuttingDown );
	}

	if( stream->writeFlag( mask & emitterEdited) )
	{
		stream->writeInt(sa_ejectionPeriodMS, 10);
		stream->writeInt(sa_periodVarianceMS, 10);
		stream->writeInt(sa_ejectionVelocity * 1000, 15);
		stream->writeInt(sa_velocityVariance * 1000, 15);
		stream->writeInt(sa_ejectionOffset * 1000, 15);

		stream->writeFlag(sticky);
		stream->writeFlag(ParticleCollision);
		stream->writeInt(attractionrange * 1000, 15);
		for(int i = 0; i < attrobjectCount; i++)
		{
			stream->writeInt(AttractionMode[i], 4);
			stream->writeInt(Amount[i] * 1000, 15);
			stream->writeString(attractedObjectID[i]);
			stream->writeString(Attraction_offset[i]);
		}
	}

	if( stream->writeFlag( mask & exprEdited) )
	{
		stream->writeString(xFunc);
		stream->writeString(yFunc);
		stream->writeString(zFunc);
		stream->writeInt(funcMax,32);
		stream->writeInt(funcMin,32);
		stream->writeInt(timeScale * 1000, 15);
		stream->writeInt(ProgressMode, 1);
		stream->writeFlag(Reverse);
		stream->writeFlag(Loop);
		stream->writeFlag(Grounded);
	}
	if( stream->writeFlag(mask & dynamicMod) )
	{
		int xmodCount = 0;
		for(int i=0;i<100;i++)
		{
			if(xVariables[i].token != NULL)
				xmodCount++;
		}

		int ymodCount = 0;
		for(int i=0;i<100;i++)
		{
			if(yVariables[i].token != NULL)
				ymodCount++;
		}

		int zmodCount = 0;
		for(int i=0;i<100;i++)
		{
			if(zVariables[i].token != NULL)
				zmodCount++;
		}
		stream->writeInt(xmodCount,7);
		stream->writeInt(ymodCount,7);
		stream->writeInt(zmodCount,7);
		for(int i=0;i<100;i++)
		{
			if(xVariables[i].token != NULL)
			{
				stream->writeString(&xVariables[i].token,3);
				stream->write(xVariables[i].value);
			}
		}
		for(int i=0;i<100;i++)
		{
			if(yVariables[i].token != NULL)
			{
				stream->writeString(&yVariables[i].token,3);
				stream->write(yVariables[i].value);
			}
		}
		for(int i=0;i<100;i++)
		{
			if(zVariables[i].token != NULL)
			{
				stream->writeString(&zVariables[i].token,3);
				stream->write(zVariables[i].value);
			}
		}
	}
	return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void GraphEmitterNode::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);

	if ( stream->readFlag() )
	{
		MatrixF temp;
		Point3F tempScale;
		mathRead(*stream, &temp);
		mathRead(*stream, &tempScale);

		setScale(tempScale);
		setTransform(temp);
	}

	if ( stream->readFlag() )
	{
		mEmitterDatablockId = stream->readFlag() ?
			stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast) : 0;

		GraphEmitterData *emitterDB = NULL;
		Sim::findObject( mEmitterDatablockId, emitterDB );
		if ( isProperlyAdded() )
			setEmitterDataBlock( emitterDB );
	}

	if ( stream->readFlag() )
	{
		mActive = stream->readFlag();
		mVelocity = stream->readFloat( 15 );

		standAloneEmitter = stream->readFlag();
		bool startShutdown = false;
		startShutdown = stream->readFlag();
		if(startShutdown)
			safeDelete();
	}
	//EmitterEdited
	if ( stream->readFlag() )
	{
		sa_ejectionPeriodMS = stream->readInt(10);
		sa_periodVarianceMS = stream->readInt(10);
		sa_ejectionVelocity = stream->readInt(15) / 1000.0f;
		sa_velocityVariance = stream->readInt(15) / 1000.0f;
		sa_ejectionOffset = stream->readInt(15) / 1000.0f;

		sticky = stream->readFlag();
		ParticleCollision = stream->readFlag();
		attractionrange = stream->readInt(15) / 1000.0f;
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

		//attractedObjectID = stream->readInt(6);


		if(mEmitter)
		{
			mEmitter->sticky = sticky;
			mEmitter->ParticleCollision = ParticleCollision;
			mEmitter->attractionrange = attractionrange;
			for(int i = 0; i < attrobjectCount; i++)
			{
				mEmitter->AttractionMode[i] = AttractionMode[i];
				mEmitter->Amount[i] = Amount[i];
				mEmitter->attractedObjectID[i] = attractedObjectID[i];
				mEmitter->Attraction_offset[i] = Attraction_offset[i];
			}

		}
	}
	//ExprEditted
	if ( stream->readFlag() )
	{
		char buf[256];

		stream->readString(buf);
		xFunc = dStrdup(buf);

		stream->readString(buf);
		yFunc = dStrdup(buf);

		stream->readString(buf);
		zFunc = dStrdup(buf);

		xfuncParser.SetExpr(UpToLow(xFunc));

		yfuncParser.SetExpr(UpToLow(yFunc));

		zfuncParser.SetExpr(UpToLow(zFunc));

		funcMax = stream->readInt(32);
		funcMin = stream->readInt(32);

		timeScale = stream->readInt(15) / 1000.0f;
		ProgressMode = stream->readInt(1);
		Reverse = stream->readFlag();
		Loop = stream->readFlag();
		Grounded = stream->readFlag();
	}
	if( stream->readFlag() )
	{
		dMemset(xVariables,0,sizeof(xVariables));
		dMemset(yVariables,0,sizeof(yVariables));
		dMemset(zVariables,0,sizeof(zVariables));
		int xi = stream->readInt(7);
		int yi = stream->readInt(7);
		int zi = stream->readInt(7);

		char buf[3];

		for(int i=0;i<xi;i++)
		{
			stream->readString(buf);
			xVariables[i].token = *dStrdup(buf);
			stream->read((F32 *)&xVariables[i].value);

			varmap_type xVars = xfuncParser.GetVar();
			varmap_type::const_iterator item = xVars.begin();
			// We don't want to call the DefineVar unless it is absolutely necessary ( for optimization )
			bool found = false;
			for(; item!= xVars.end(); ++item)
			{
				if(UpToLow(const_cast<char*>(item->first.c_str())) == UpToLow(&xVariables[i].token))
					found = true;
			}
			if(!found)
			{
				xfuncParser.DefineVar(UpToLow(&xVariables[i].token), &xVariables[i].value);
			}
		}
		for(int i=0;i<yi;i++)
		{
			stream->readString(buf);
			yVariables[i].token = *dStrdup(buf);
			stream->read((F32 *)&yVariables[i].value);

			varmap_type yvars = yfuncParser.GetVar();
			varmap_type::const_iterator item = yvars.begin();
			bool found = false;
			for(; item!= yvars.end(); ++item)
			{
				if(UpToLow(const_cast<char*>(item->first.c_str())) == UpToLow(&yVariables[i].token))
					found = true;
			}
			if(!found)
				yfuncParser.DefineVar(UpToLow(&yVariables[i].token), &yVariables[i].value);
		}
		for(int i=0;i<zi;i++)
		{
			stream->readString(buf);
			zVariables[i].token = *dStrdup(buf);
			stream->read((F32 *)&zVariables[i].value);

			varmap_type zVars = zfuncParser.GetVar();
			varmap_type::const_iterator item = zVars.begin();
			bool found = false;
			for(; item!= zVars.end(); ++item)
			{
				if(UpToLow(const_cast<char*>(item->first.c_str())) == UpToLow(&zVariables[i].token))
					found = true;
			}
			if(!found)
				zfuncParser.DefineVar(UpToLow(&zVariables[i].token), &zVariables[i].value);
		}
		updateMaxMinDistances();
	}
}

void GraphEmitterNode::updateMaxMinDistances()
{
	F32 tmpPartProg = particleProg;
	xMxDist = 0;
	xMnDist = 0;
	yMxDist = 0;
	yMnDist = 0;
	zMxDist = 0;
	zMnDist = 0;

	for(int i = funcMin; i <= funcMax; i++)
	{
		F32 xRes;
		F32 yRes;
		F32 zRes;
		particleProg = i;
		try{
			xRes = xfuncParser.Eval() * sa_ejectionOffset;
			yRes = yfuncParser.Eval() * sa_ejectionOffset;
			zRes = zfuncParser.Eval() * sa_ejectionOffset;
		}
		catch(mu::Parser::exception_type &e)
		{
			// Most likely cause is: The expression were updated before the dynamic variables were
			//  Don't want it to throw an error all the time, even tho it should throw an exception
			//  When thing really does go wrong -- Needs fix.
			//std::string expr = e.GetExpr();
			//std::string tok = e.GetToken();
			//size_t pos = e.GetPos();
			//std::string msg = e.GetMsg();
			//Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
			break;
		}

		if(xRes > xMxDist)
			xMxDist = xRes;
		if(xRes < xMnDist)
			xMnDist = xRes;

		if(yRes > yMxDist)
			yMxDist = yRes;
		if(yRes < yMnDist)
			yMnDist = yRes;

		if(zRes > zMxDist)
			zMxDist = zRes;
		if(zRes < zMnDist)
			zMnDist = zRes;
	}
	particleProg = tmpPartProg;
	if(xMxDist == 0)
		xMxDist = 0.5;
	if(xMnDist == 0)
		xMnDist = -0.5;
	if(yMxDist == 0)
		yMxDist = 0.5;
	if(yMnDist == 0)
		yMnDist = -0.5;
	if(zMxDist == 0)
		zMxDist = 0.5;
	if(zMnDist == 0)
		zMnDist = -0.5;
}

//-----------------------------------------------------------------------------
// This function converts uppercase characters to lowercase
//-----------------------------------------------------------------------------
char* GraphEmitterNode::UpToLow(char* c)
{
	for (int i=0; i<strlen(c); i++)
		if(c[i] >= 0x41 && c[i] <= 0x5A)
			c[i] = c[i] + 0x20;
	//tolower(c[i]);
	return c;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void GraphEmitterNode::setEmitterDataBlock(GraphEmitterData* data)
{
	if ( isServerObject() )
	{
		// Update the node's sa values to the new emitter datablocks values.
		/*sa_thetaMax = data->thetaMax;
		sa_thetaMin = data->thetaMin;

		sa_phiReferenceVel = data->phiReferenceVel;
		sa_phiVariance = data->phiVariance;

		sa_ejectionVelocity = data->ejectionVelocity;
		sa_velocityVariance = data->velocityVariance;
		sa_ejectionOffset = data->ejectionOffset;

		sa_ejectionPeriodMS = data->ejectionPeriodMS;
		sa_periodVarianceMS = data->periodVarianceMS;
		| emitterEdited */

		setMaskBits( EmitterDBMask );
	}
	else
	{
		if ( mEmitter && data == mEmitter->getDataBlock() )
			return;
		GraphEmitter* pEmitter = NULL;
		if ( data )
		{
			// Create emitter with new datablock
			pEmitter = new GraphEmitter;
			pEmitter->onNewDataBlock( data, false );
			if( pEmitter->registerObject() == false )
			{
				Con::warnf(ConsoleLogEntry::General, "Could not register base emitter for particle of class: %s", data->getName() ? data->getName() : data->getIdString() );
				delete pEmitter;
				return;
			}
		}

		// Replace emitter
		if ( mEmitter )
			mEmitter->deleteWhenEmpty();

		mEmitter = pEmitter;

		if(mEmitter)
		{
			mEmitter->sticky = sticky;
			mEmitter->ParticleCollision = ParticleCollision;
			mEmitter->attractionrange = attractionrange;
			for(int i = 0; i < attrobjectCount; i++)
			{
				mEmitter->attractedObjectID[i] = attractedObjectID[i];
				mEmitter->AttractionMode[i] = AttractionMode[i];
				mEmitter->Amount[i] = Amount[i];
				mEmitter->Attraction_offset[i] = Attraction_offset[i];
			}
		}
	}

	mEmitterDatablock = data;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void GraphEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	// Was it an sa_value that was edited? Then set the emitterEdited bit.
	if(strcmp(slotName, "sa_ejectionOffset") == 0 ||
		strcmp(slotName, "sa_ejectionPeriodMS") == 0 ||
		strcmp(slotName, "sa_periodVarianceMS") == 0 ||
		strcmp(slotName, "standAloneEmitter") == 0 ||
		strcmp(slotName, "sa_ejectionVelocity") == 0 ||
		strcmp(slotName, "sa_velocityVariance") == 0 ||
		strcmp(slotName, "attracted") == 0 ||
		strcmp(slotName, "attractionrange") == 0 ||
		strcmp(slotName, "Attraction_offset") == 0 ||
		strcmp(slotName, "Amount") == 0 ||
		strcmp(slotName, "sticky") == 0 ||
		strcmp(slotName, "RayCollision") == 0 ||
		strcmp(slotName, "attractedObjectID") == 0 ||
		strcmp(slotName, "AttractionMode") == 0)
		setMaskBits(emitterEdited);

	// Was it an expression related value? Then set the exprEdited bit.
	if(strcmp(slotName, "xFunc") == 0 ||
		strcmp(slotName, "yFunc") == 0 ||
		strcmp(slotName, "zFunc") == 0 ||
		strcmp(slotName, "funcMax") == 0 ||
		strcmp(slotName, "funcMin") == 0 ||
		strcmp(slotName, "ProgressMode") == 0 ||
		strcmp(slotName, "Reverse") == 0 ||
		strcmp(slotName, "Loop") == 0 ||
		strcmp(slotName, "Grounded") == 0 ||
		strcmp(slotName, "timeScale") == 0)
	{
		setMaskBits(exprEdited);
		updateMaxMinDistances();
	}

	if(!isProperlyAdded())
	{
		if(strcmp(slotName, "Attraction_offset") == 0)
		{
			initialValues.push_back(std::string("Attraction_offset"));
			initialValues.push_back(std::string(Attraction_offset[0]));
			initialValues.push_back(std::string(Attraction_offset[1]));
		}
		else if(strcmp(slotName, "attractedObjectID") == 0)
		{
			initialValues.push_back(std::string("attractedObjectID"));
			initialValues.push_back(std::string(attractedObjectID[0]));
			initialValues.push_back(std::string(attractedObjectID[1]));
		}
		else if(strcmp(slotName, "AttractionMode") == 0)
		{
			initialValues.push_back(std::string("AttractionMode"));
			std::stringstream ss;
			ss << AttractionMode[0];
			initialValues.push_back(std::string(ss.str()));
			ss.str("");
			ss << AttractionMode[1];
			initialValues.push_back(std::string(ss.str()));
		}
		else if(strcmp(slotName, "Amount") == 0)
		{
			initialValues.push_back(std::string("Amount"));
			std::stringstream ss;
			ss << Amount[0];
			initialValues.push_back(std::string(ss.str()));
			ss.str("");
			ss << Amount[1];
			initialValues.push_back(std::string(ss.str()));
		}
		else{
			//Save the values and add them onNewDataBlock
			initialValues.push_back(std::string(slotName));
			initialValues.push_back(std::string(newValue));
		}
	}

	// If it was a function, update the parsers 
	// ( we don't want it to be case sensitive hence the UpToLow )
	if(strcmp(slotName, "xFunc") == 0)
		xfuncParser.SetExpr(UpToLow(xFunc));
	if(strcmp(slotName, "yFunc") == 0)
		yfuncParser.SetExpr(UpToLow(yFunc));
	if(strcmp(slotName, "zFunc") == 0)
		zfuncParser.SetExpr(UpToLow(zFunc));

	if(strcmp(slotName, "attractedObjectID") == 0)
	{
		for(int i = 0; i < attrobjectCount; i++)
		{
			attractedObjectID[i] = StringTableEntry(attractedObjectID[i]);
		}
	}

	// If it was the emitter value which was updated, then update the associated emitter
	if(strcmp(slotName, "emitter") == 0)
	{
		if(isServerObject())
		{
			setMaskBits(EmitterDBMask);
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void GraphEmitterNode::onDynamicModified(const char* slotName, const char*newValue)
{
	/**Syntax check the dynamic field to see if it follows the pattern:
	CoorVarIndexToken e.g. xVar1c**/
	if(slotName[0] == 'x' || slotName[0] == 'X' ||
		slotName[0] == 'y' || slotName[0] == 'Y' ||
		slotName[0] == 'z' || slotName[0] == 'Z')
	{
		if(slotName[1] == 'v' || slotName[1] == 'V')
		{
			if(slotName[2] == 'a' || slotName[2] == 'A')
			{
				if(slotName[3] == 'r' || slotName[3] == 'R')
				{
					//Is the 5th bit a digit? If it is not the syntax is wrong
					if(isdigit(slotName[4]))
					{
						U8 index = atoi(&slotName[4]);
						char name;

						int ccnt = 0;
						//If the 6th bit is a digit as well, append the bit to the index as the
						// - least significant bit
						if(isdigit(slotName[5]))
						{
							//U8 second = atoi(&slotName[4]); I don't know why this isn't needed but it isn't
							char combined[3];
							strcpy(combined, &slotName[4]);
							index = atoi(combined);
						}
						else if(!isalpha(slotName[5]))
						{
							// If slotName bit 5 isn't a digit and it isn't an alphabetical character something must be wrong.
							Con::warnf("Dynamic variable specified on graphEmitter %s but no token set", getIdString());
							return;
						}
						else{
							//Else add slotName bit 5 to bit 0 of name.
							//name[ccnt] = slotName[5];
							name = slotName[5];
							ccnt++;
						}
						/*if(isalpha(slotName[6])){
						name[ccnt] = slotName[6];
						ccnt++;
						if(isalpha(slotName[7])){
						name[ccnt] = slotName[7];
						ccnt++;
						}
						}*/
						//If name has been set to something
						if(name != NULL)
						{
							F32 value = atof(newValue);
							if(isdigit(*newValue))
							{
								if(slotName[0] == 'x' || slotName[0] == 'X')
								{
									// If we already have a variable with that name and it is not equal to the current token
									//  - then we want to remove it before we define it again.
									if(xVariables[index].token != NULL && xVariables[index].token != name)
									{
										xfuncParser.RemoveVar(UpToLow(&xVariables[index].token));
									}
									// Add the new variable to the xVariables list
									xVariables[index].value = value;
									xVariables[index].token = name;
									std::string s;
									std::stringstream ss;
									ss << xVariables[index].token;
									ss >> s;
									try{
										xfuncParser.DefineVar(s,&xVariables[index].value);
									}
									catch(mu::Parser::exception_type &e)
									{
										std::string expr = e.GetExpr();
										std::string tok = e.GetToken();
										size_t pos = e.GetPos();
										std::string msg = e.GetMsg();
										Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
									}
									// If we are running this on the server, be sure to update the client aswell.
									if(isServerObject())
										setMaskBits(dynamicMod);
								}
								if(slotName[0] == 'y' || slotName[0] == 'Y')
								{
									if(yVariables[index].token != NULL && yVariables[index].token != name)
									{
										yfuncParser.RemoveVar(UpToLow(&yVariables[index].token));
									}
									yVariables[index].value = value;
									yVariables[index].token = name;
									std::string s;
									std::stringstream ss;
									ss << xVariables[index].token;
									ss >> s;
									try{
										yfuncParser.DefineVar(s,&yVariables[index].value);
									}
									catch(mu::Parser::exception_type &e)
									{
										std::string expr = e.GetExpr();
										std::string tok = e.GetToken();
										size_t pos = e.GetPos();
										std::string msg = e.GetMsg();
										Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
									}

									if(isServerObject())
										setMaskBits(dynamicMod);
								}
								if(slotName[0] == 'z' || slotName[0] == 'Z')
								{
									if(zVariables[index].token != NULL && zVariables[index].token != name)
									{
										zfuncParser.RemoveVar(UpToLow(&zVariables[index].token));
									}
									zVariables[index].value = value;
									zVariables[index].token = name;
									std::string s;
									std::stringstream ss;
									ss << xVariables[index].token;
									ss >> s;
									try{
										zfuncParser.DefineVar(s,&zVariables[index].value);
									}
									catch(mu::Parser::exception_type &e)
									{
										std::string expr = e.GetExpr();
										std::string tok = e.GetToken();
										size_t pos = e.GetPos();
										std::string msg = e.GetMsg();
										Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
									}

									if(isServerObject())
										setMaskBits(dynamicMod);
								}
							}
						}
					}
					else
					{
						Con::warnf("Dynamic variable specified on graphEmitter %s but no index set", getIdString());
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// The onBoundaryLimit callback handler.
//-----------------------------------------------------------------------------
void GraphEmitterNode::onBoundaryLimit(bool Max)
{
	cb_Max = Max;
	if (mDataBlock && !isGhost())
		mDataBlock->onBoundaryLimit_callback( this, Max );
	else
	{
		NetConnection* conn = NetConnection::getConnectionToServer();
		if(conn)
			conn->postNetEvent(new CallbackEvent(conn->getGhostIndex(this), Max));
	}
}

//-----Netevent---
// Document this later.
CallbackEvent::CallbackEvent(S32 id, bool max)
{
	mNode = id;
	Max = max;
}

CallbackEvent::~CallbackEvent()
{
	//dFree(&Max);
	//dFree(&mNode);
}

void CallbackEvent::pack(NetConnection* conn, BitStream *bstream)
{
	bstream->write(Max);
	bstream->write(mNode);
}

void CallbackEvent::unpack(NetConnection *conn, BitStream *bstream)
{
	bstream->read(&Max);
	bstream->read(&mNode);
}

// This just prints the event in the console. You might
// want to do something more clever here -- BJG
void CallbackEvent::process(NetConnection *conn)
{
	try{
		GraphEmitterNode* m_Node = dynamic_cast< GraphEmitterNode* >(conn->resolveObjectFromGhostIndex(mNode));
		if(m_Node)
			m_Node->onBoundaryLimit(Max);
	}
	catch(...){
		Con::printf("Callback error");
	}
}

void CallbackEvent::write(NetConnection*, BitStream *bstream)
{
	bstream->write(Max);
}

//_______________________________
DeleteEvent::DeleteEvent(S32 id)
{
	mNode = id;
}

DeleteEvent::~DeleteEvent()
{
	//dFree(&Max);
	//dFree(&mNode);
}

void DeleteEvent::pack(NetConnection* conn, BitStream *bstream)
{
	bstream->write(mNode);
}

void DeleteEvent::unpack(NetConnection *conn, BitStream *bstream)
{
	bstream->read(&mNode);
}

// This just prints the event in the console. You might
// want to do something more clever here -- BJG
void DeleteEvent::process(NetConnection *conn)
{
	GraphEmitterNode* m_Node = dynamic_cast< GraphEmitterNode* >(conn->resolveGhost(mNode));
	m_Node->safeDelete();
}

void DeleteEvent::write(NetConnection*, BitStream *bstream)
{
}

IMPLEMENT_CO_NETEVENT_V1(DeleteEvent);
IMPLEMENT_CO_NETEVENT_V1(CallbackEvent);

IMPLEMENT_CALLBACK(GraphEmitterNodeData, onBoundaryLimit, void, ( GameBase* obj, bool Max), ( obj, Max ),
	"@brief Informs a graphEmitter that it has hit a boundary and is now resetting the t value.n"
	"@param Max - did it hit the maximum boundary or the minimum boundary?.n"
	);


DefineEngineMethod(GraphEmitterNode, setEmitterDataBlock, void, (GraphEmitterData* emitterDatablock), (0),
	"Assigns the datablock for this emitter node.\n"
	"@param emitterDatablock GraphEmitterData datablock to assign\n"
	"@tsexample\n"
	"// Assign a new emitter datablock\n"
	"%emitter.setEmitterDatablock( %emitterDatablock );\n"
	"@endtsexample\n" )
{
	if ( !emitterDatablock )
	{
		Con::errorf("GraphEmitterData datablock could not be found when calling setEmitterDataBlock in graphEmitterNode.");
		return;
	}

	object->setEmitterDataBlock(emitterDatablock);
}

DefineEngineMethod(GraphEmitterNode, setActive, void, (bool active),,
	"Turns the emitter on or off.\n"
	"@param active New emitter state\n" )
{
	object->setActive( active );
}

DefineEngineMethod(GraphEmitterNode, safeDelete, void, (void),,
	"Delete the emitter.\n")
{
	object->safeDelete();
}

DefineEngineMethod(GraphEmitterNode, setDataBlock, void, (void),,
	"Delete the emitter.\n")
{
	object->safeDelete();
>>>>>>> 2190e5a3bcdb44a5f496b14f6226b46c12fa418b
}