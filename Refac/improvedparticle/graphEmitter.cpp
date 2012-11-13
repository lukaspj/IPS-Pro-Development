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

#include "graphEmitter.h"
#include "graphEmitterNode.h"
#include "IPSCore.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(GraphEmitterData);
IMPLEMENT_CONOBJECT(GraphEmitter);

ConsoleDocClass( GraphEmitter,
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

ConsoleDocClass( GraphEmitterData,
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

typedef GraphEmitter::EnumProgressMode gProgressMode;
DefineEnumType( gProgressMode );

ImplementEnumType( gProgressMode,
	"The way the t value of the graphEmitter is increased.\n"
	"@ingroup FX\n\n")
{ GraphEmitter::byParticleCount,				"ParticleCount",	"Increase t value when a new particle is emitted.\n" },
{ GraphEmitter::byTime,							"ByTime",			"Increase t value with time.\n" },
EndImplementEnumType;

//-----------------------------------------------------------------------------
// GraphEmitterData
//-----------------------------------------------------------------------------
GraphEmitterData::GraphEmitterData()
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

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GraphEmitterData::initPersistFields()
{
	addGroup( "Expression");

	addField( "Grounded", TYPEID< bool >(), Offset(Grounded, GraphEmitterData),
		"Ground the graphEmitter." );

	addField( "xFunc", TYPEID< StringTableEntry >(), Offset(xFunc, GraphEmitterData),
		"The expression specifying the emitted particles X coordinate." );

	addField( "yFunc", TYPEID< StringTableEntry >(), Offset(yFunc, GraphEmitterData),
		"The expression specifying the emitted particles Y coordinate." );

	addField( "zFunc", TYPEID< StringTableEntry >(), Offset(zFunc, GraphEmitterData),
		"The expression specifying the emitted particles Z coordinate." );

	addField( "funcMax", TYPEID< S32 >(), Offset(funcMax, GraphEmitterData),
		"TThe expressions interval, maximum." );

	addField( "funcMin", TYPEID< S32 >(), Offset(funcMin, GraphEmitterData),
		"The expressions interval, minimum." );

	addField( "timeScale", TYPEID< F32 >(), Offset(timeScale, GraphEmitterData),
		"The amount to scale the t value with." );

	addField( "ProgressMode", TYPEID< GraphEmitter::EnumProgressMode >(), Offset(ProgressMode, GraphEmitterData),
		"String value that controls how the t value is increased." );

	addField( "Reverse", TYPEID< bool >(), Offset(Reverse, GraphEmitterData),
		"Reverse the graphEmitter." );

	addField( "Loop", TYPEID< bool >(), Offset(Loop, GraphEmitterData),
		"Loop the graphEmitter." );

	endGroup( "Expression" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void GraphEmitterData::packData(BitStream* stream)
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
void GraphEmitterData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	char buf[256];

	stream->readString(buf);
	
	dStrdup(buf);

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
// onAdd
//-----------------------------------------------------------------------------
bool GraphEmitterData::onAdd()
{
	if( Parent::onAdd() == false )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// SphereEmitter
//-----------------------------------------------------------------------------
GraphEmitter::GraphEmitter()
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
void GraphEmitter::initPersistFields()
{
	addGroup( "Expression");

	addField( "Grounded", TYPEID< bool >(), Offset(Grounded, GraphEmitter),
		"Ground the graphEmitter." );

	addField( "xFunc", TYPEID< StringTableEntry >(), Offset(xFunc, GraphEmitter),
		"The expression specifying the emitted particles X coordinate." );

	addField( "yFunc", TYPEID< StringTableEntry >(), Offset(yFunc, GraphEmitter),
		"The expression specifying the emitted particles Y coordinate." );

	addField( "zFunc", TYPEID< StringTableEntry >(), Offset(zFunc, GraphEmitter),
		"The expression specifying the emitted particles Z coordinate." );

	addField( "funcMax", TYPEID< S32 >(), Offset(funcMax, GraphEmitter),
		"TThe expressions interval, maximum." );

	addField( "funcMin", TYPEID< S32 >(), Offset(funcMin, GraphEmitter),
		"The expressions interval, minimum." );

	addField( "timeScale", TYPEID< F32 >(), Offset(timeScale, GraphEmitter),
		"The amount to scale the t value with." );

	addField( "ProgressMode", TYPEID< gProgressMode >(), Offset(ProgressMode, GraphEmitter),
		"String value that controls how the t value is increased." );

	addField( "Reverse", TYPEID< bool >(), Offset(Reverse, GraphEmitter),
		"Reverse the graphEmitter." );

	addField( "Loop", TYPEID< bool >(), Offset(Loop, GraphEmitter),
		"Loop the graphEmitter." );

	endGroup( "Expression" );

	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U32 GraphEmitter::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
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
void GraphEmitter::unpackUpdate(NetConnection* con, BitStream* stream)
{
	Parent::unpackUpdate(con, stream);

	//ExprEditted
	if ( stream->readFlag() )
	{
		char buf[256];

		stream->readString(buf);
		*xFunc = *IPSCore::UpToLow(dStrdup(buf));

		stream->readString(buf);
		*yFunc = *IPSCore::UpToLow(dStrdup(buf));
		//yFunc = dStrdup(buf);

		stream->readString(buf);
		*zFunc = *IPSCore::UpToLow(dStrdup(buf));
		//zFunc = dStrdup(buf);

		xfuncParser.SetExpr(IPSCore::UpToLow(xFunc));

		yfuncParser.SetExpr(IPSCore::UpToLow(yFunc));

		zfuncParser.SetExpr(IPSCore::UpToLow(zFunc));

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

//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool GraphEmitter::onNewDataBlock( GameBaseData *dptr, bool reload )
{
	Parent::onNewDataBlock(dptr, reload);
	GraphEmitterData* DataBlock = getDataBlock();
	if ( !DataBlock || !Parent::onNewDataBlock( dptr, reload ) )
		return false;

	// Synchronize the expression data.
	xFunc = DataBlock->xFunc;
	yFunc = DataBlock->yFunc;
	zFunc = DataBlock->zFunc;

	xfuncParser.SetExpr(xFunc);
	yfuncParser.SetExpr(yFunc);
	zfuncParser.SetExpr(zFunc);

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

	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

//-----------------------------------------------------------------------------
// addParticle
//-----------------------------------------------------------------------------
bool GraphEmitter::addParticle(const Point3F& pos,
	const Point3F& axis,
	const Point3F& vel,
	const Point3F& axisx)
{
	GraphEmitterData* DataBlock = getDataBlock();
	n_parts++;
	if (n_parts > n_part_capacity || n_parts > DataBlock->partListInitSize)
	{
		// In an emergency we allocate additional particles in blocks of 16.
		// This should happen rarely.
		Particle* store_block = new Particle[16];
		part_store.push_back(store_block);
		n_part_capacity += 16;
		for (S32 i = 0; i < 16; i++)
		{
			store_block[i].next = part_freelist;
			part_freelist = &store_block[i];
		}
		DataBlock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
	}
	Particle* pNew = part_freelist;
	part_freelist = pNew->next;
	pNew->next = part_list_head.next;
	part_list_head.next = pNew;

	Point3F ejectionAxis = axis;

	F32 initialVel = DataBlock->ejectionVelocity;
	initialVel    += (DataBlock->velocityVariance * 2.0f * gRandGen.randF()) - DataBlock->velocityVariance;

	pNew->pos = pos + (ejectionAxis * DataBlock->ejectionOffset);
	pNew->vel = ejectionAxis * initialVel;
	pNew->orientDir = ejectionAxis;
	pNew->acc.set(0, 0, 0);
	pNew->currentAge = 0;

	// Choose a new particle datablack randomly from the list
	U32 dBlockIndex = gRandGen.randI() % DataBlock->particleDataBlocks.size();
	DataBlock->particleDataBlocks[dBlockIndex]->initializeParticle(pNew, vel);
	updateKeyData( pNew );
	return true;
}

//-----------------------------------------------------------------------------
// addParticle - Node
//-----------------------------------------------------------------------------
bool GraphEmitter::addParticle(const Point3F& pos,
	const Point3F& axis,
	const Point3F& vel,
	const Point3F& axisx,
	ParticleEmitterNode* pnodeDat)
{
	PROFILE_SCOPE(GraphEmitAddPart);
	IPSBenchmarkBegin;
	GraphEmitterNode* nodeDat = static_cast<GraphEmitterNode*>(pnodeDat);
	GraphEmitterData* DataBlock = getDataBlock();
	n_parts++;
	if (n_parts > n_part_capacity || n_parts > DataBlock->partListInitSize)
	{
		// In an emergency we allocate additional particles in blocks of 16.
		// This should happen rarely.
		Particle* store_block = new Particle[16];
		part_store.push_back(store_block);
		n_part_capacity += 16;
		for (S32 i = 0; i < 16; i++)
		{
			store_block[i].next = part_freelist;
			part_freelist = &store_block[i];
		}
		mDataBlock->allocPrimBuffer(n_part_capacity); // allocate larger primitive buffer or will crash 
	}
	Particle* pNew = part_freelist;
	part_freelist = pNew->next;
	pNew->next = part_list_head.next;
	part_list_head.next = pNew;

	F32 initialVel;
	// If it is a standAloneEmitter, then we want it to use the sa values from the node
	if(nodeDat->standAloneEmitter)
	{
		initialVel = nodeDat->sa_ejectionVelocity;
		initialVel    += (nodeDat->sa_velocityVariance * 2.0f * gRandGen.randF()) - nodeDat->sa_velocityVariance;
	}
	else
	{
		initialVel = DataBlock->ejectionVelocity;
		initialVel    += (DataBlock->velocityVariance * 2.0f * gRandGen.randF()) - DataBlock->velocityVariance;
	}

	if(nodeDat)
	{
		// Set the time since this code was last run
		U32 dt = mInternalClock - oldTime;
		oldTime = mInternalClock;

		// Did we hit the upper limit?
		if(nodeDat->particleProg > nodeDat->funcMax)
		{
			if(nodeDat->Loop)
				nodeDat->particleProg = nodeDat->funcMin;
			nodeDat->onBoundaryLimit(true);
		}
		// Did we hit the lower limit?
		if(nodeDat->particleProg < nodeDat->funcMin)
		{
			if(nodeDat->Loop)
				nodeDat->particleProg = nodeDat->funcMax;
			nodeDat->onBoundaryLimit(false);
		}
		// We don't want to risk dividing by zero.
		//  - We don't care too much about accuracy, so whatever is close to zero is fine.
		if(nodeDat->particleProg == 0)
		{
			nodeDat->particleProg = F32_MIN;
		}

		F32 resultx = 0;
		F32 resulty = 0;
		F32 resultz = 0;
		// Evaluate the expressions and get the results.
		try{
			resultx = nodeDat->xfuncParser.Eval();
			resulty = nodeDat->yfuncParser.Eval();
			resultz = nodeDat->zfuncParser.Eval();
		}
		catch(mu::Parser::exception_type &e)
		{
			std::string expr = e.GetExpr();
			std::string tok = e.GetToken();
			size_t pos = e.GetPos();
			std::string msg = e.GetMsg();
			Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
		}

		if(nodeDat->Grounded)
		{
			F32 theHeight;
			Point3F TerNorm;
			bool foundTerrain = GetTerrainHeightAndNormal(pos.x+resultx, pos.y+resulty, theHeight, TerNorm);
			resultz += theHeight;
			// Construct a vector from the 3 results
			Point3F funcPos = Point3F(resultx, resulty, resultz - pos.z);
			// Get the transform of the node to get the rotation matrix
			MatrixF trans = nodeDat->getTransform();
			// Rotate our point by the rotation matrix
			trans.mulV(funcPos);

			// Add the position of the node to get coordinates in object space
			//  - and set the position of the new particle.
			if(nodeDat->standAloneEmitter){
				pNew->pos = pos + funcPos + (TerNorm * nodeDat->sa_ejectionOffset);

				pNew->relPos = funcPos + TerNorm * nodeDat->sa_ejectionOffset;
			}
			else{
				pNew->pos = pos + funcPos + (TerNorm * mDataBlock->ejectionOffset);

				pNew->relPos = funcPos + TerNorm * mDataBlock->ejectionOffset;
			}
		}
		else{
			// Construct a vector from the 3 results
			const Point3F *funcPos = new const Point3F(resultx, resulty, resultz);

			// Get the transform of the node to get the rotation matrix
			MatrixF trans = nodeDat->getTransform();
			// Rotate our point by the rotation matrix
			Point3F p;
			trans.mulV(*funcPos, &p);

			// Add the position of the node to get coordinates in object space
			//  - and set the position of the new particle.
			if(nodeDat->standAloneEmitter){
				pNew->pos = pos + (p * nodeDat->sa_ejectionOffset);

				pNew->relPos = p * nodeDat->sa_ejectionOffset;
			}
			else{
				pNew->pos = pos + (p * mDataBlock->ejectionOffset);

				pNew->relPos = p * mDataBlock->ejectionOffset;
			}

			delete(funcPos);
		}

		//pNew->parent = *nodeDat;


		// Increment the t value based on the progressmode
		if(nodeDat->ProgressMode == gProgressMode::byParticleCount){
			if(Reverse)
				nodeDat->particleProg = (nodeDat->particleProg - (1 * nodeDat->timeScale));
			else
				nodeDat->particleProg = (nodeDat->particleProg + (1 * nodeDat->timeScale));
		}
		if(nodeDat->ProgressMode == gProgressMode::byTime){
			if(Reverse)
				nodeDat->particleProg = (nodeDat->particleProg - (dt * nodeDat->timeScale));
			else
				nodeDat->particleProg = (nodeDat->particleProg + (dt * nodeDat->timeScale));
		}
		parentNodePos = pos;
		Point3F relNorm = pNew->relPos;
		relNorm.normalize();
		pNew->vel = axis * initialVel;
		pNew->orientDir = axis;
		pNew->acc.set(0, 0, 0);
		pNew->currentAge = 0;

		// Choose a new particle datablack randomly from the list
		U32 dBlockIndex = gRandGen.randI() % mDataBlock->particleDataBlocks.size();
		mDataBlock->particleDataBlocks[dBlockIndex]->initializeParticle(pNew, vel);
		updateKeyData( pNew );
	}
	IPSBenchmarkEnd("----Graph---- addPart");
	return true;
}

void GraphEmitter::onStaticModified(const char* slotName, const char*newValue)
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

void GraphEmitter::onDynamicModified(const char* slotName, const char*newValue)
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

//-----------------------------------------------------------------------------
// The onBoundaryLimit callback handler.
//-----------------------------------------------------------------------------
void GraphEmitter::onBoundaryLimit(bool Max, ParticleEmitterNode* node)
{
	cb_Max = Max;
	GraphEmitterData* DataBlock = getDataBlock();
	if (DataBlock && !isGhost())
		DataBlock->onBoundaryLimit_callback( node, Max );
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
			m_Node->getEmitter()->onBoundaryLimit(Max, m_Node);
	}
	catch(...){
		Con::printf("Callback error");
	}
}

void CallbackEvent::write(NetConnection*, BitStream *bstream)
{
	bstream->write(Max);
}

IMPLEMENT_CO_NETEVENT_V1(CallbackEvent);

IMPLEMENT_CALLBACK(GraphEmitterData, onBoundaryLimit, void, ( GameBase* obj, bool Max), ( obj, Max ),
	"@brief Informs a graphEmitter that it has hit a boundary and is now resetting the t value.n"
	"@param Max - did it hit the maximum boundary or the minimum boundary?.n"
	);

ParticleEmitter* GraphEmitterData::createEmitter() { return new GraphEmitter; }


bool GraphEmitter::GetTerrainHeightAndNormal(const F32 x, const F32 y, F32 &height, Point3F &normal)
{
	Point3F startPnt( x, y, 10000.0f );
	Point3F endPnt( x, y, -10000.0f );

	RayInfo ri;
	bool hit;         

	hit = gClientContainer.castRay(startPnt, endPnt, TerrainObjectType, &ri);   

	if ( hit ){
		height = ri.point.z;
		normal = ri.normal;
	}

	return hit;
}