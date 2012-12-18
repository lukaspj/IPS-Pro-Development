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
	addField( "emitter",  TYPEID< GraphEmitterData >(), Offset(mEmitterDatablock, GraphEmitterNode),
		"Datablock to use when emitting particles." );

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

	UpdateEmitterValues();

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

		funcMax = stream->readInt(32);
		funcMin = stream->readInt(32);

		timeScale = stream->readInt(15) / 1000.0f;
		ProgressMode = stream->readInt(1);
		Reverse = stream->readFlag();
		Loop = stream->readFlag();
		Grounded = stream->readFlag();
	}
	UpdateEmitterValues();
}

void GraphEmitterNode::onStaticModified(const char* slotName, const char*newValue)
{
	if( strcmp(slotName, "xFunc") == 0 ){
		saUpdateBits |= saXFunc;
		setMaskBits(exprEdited);
	}
	else if( strcmp(slotName, "yFunc") == 0 ){
		saUpdateBits |= saYFunc;
		setMaskBits(exprEdited);
	}
	else if( strcmp(slotName, "zFunc") == 0 ){
		saUpdateBits |= saZFunc;
		setMaskBits(exprEdited);
	}
	else if( strcmp(slotName, "funcMax") == 0 ){
		saUpdateBits |= safuncMax;
		setMaskBits(exprEdited);
	}
	else if( strcmp(slotName, "funcMin") == 0 ){
		saUpdateBits |= safuncMin;
		setMaskBits(exprEdited);
	}
	else if( strcmp(slotName, "ProgressMode") == 0 ){
		saUpdateBits |= saProgMode;
		setMaskBits(exprEdited);
	}
	else if( strcmp(slotName, "Reverse") == 0 ){
		saUpdateBits |= saReverse;
		setMaskBits(exprEdited);
	}
	else if( strcmp(slotName, "Loop") == 0 ){
		saUpdateBits |= saLoop;
		setMaskBits(exprEdited);
	}
	else if( strcmp(slotName, "Grounded") == 0 ){
		saUpdateBits |= saGrounded;
		setMaskBits(exprEdited);
	}
	else if( strcmp(slotName, "timeScale") == 0 ){
		saUpdateBits |= saTimeScale;
		setMaskBits(exprEdited);
	}

	Parent::onStaticModified(slotName, newValue);
}

void GraphEmitterNode::onDynamicModified(const char* slotName, const char*newValue)
{
	
	mEmitter->onDynamicModified(slotName, newValue);
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
	if(!mEmitter)
		return;
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
			xRes = ((GraphEmitter*)mEmitter)->xfuncParser.Eval() * sa_ejectionOffset;
			yRes = ((GraphEmitter*)mEmitter)->yfuncParser.Eval() * sa_ejectionOffset;
			zRes = ((GraphEmitter*)mEmitter)->zfuncParser.Eval() * sa_ejectionOffset;
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
	UpdateEmitterValues();
}

void GraphEmitterNode::UpdateEmitterValues()
{
	if(!mEmitter)
		return;
	GraphEmitter* emitter = (GraphEmitter*)mEmitter;
	if(saGrounded & saUpdateBits)
		emitter->Grounded = Grounded;
	if(saReverse & saUpdateBits)
		emitter->Reverse = Reverse;
	if(saLoop & saUpdateBits)
		emitter->Loop = Loop;
	if(saTimeScale & saUpdateBits)
		emitter->timeScale = timeScale;
	if(safuncMax & saUpdateBits)
		emitter->funcMax = funcMax;
	if(safuncMin & saUpdateBits)
		emitter->funcMin = funcMin;
	if(saProgMode & saUpdateBits)
		emitter->ProgressMode = ProgressMode;
	if(saXFunc & saUpdateBits){
		emitter->xFunc = xFunc;
		((GraphEmitter*)mEmitter)->xfuncParser.SetExpr(IPSCore::UpToLow(xFunc));
	}
	if(saYFunc & saUpdateBits){
		emitter->yFunc = yFunc;
		((GraphEmitter*)mEmitter)->yfuncParser.SetExpr(IPSCore::UpToLow(yFunc));
	}
	if(saZFunc & saUpdateBits){
		emitter->zFunc = zFunc;
		((GraphEmitter*)mEmitter)->zfuncParser.SetExpr(IPSCore::UpToLow(zFunc));
	}
	Parent::UpdateEmitterValues();
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
}