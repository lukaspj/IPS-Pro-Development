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

#include "graphEmitter.h"
#include "../IPSCore.h"

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

	timeScale = 1.0;
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void GraphEmitterData::initPersistFields()
{
	addGroup( "Expression");

	addField( "Grounded", TYPEID< bool >(), Offset(Grounded, GraphEmitterData),
		"Emit particles along the terrain rather than at the nodes position." );

	addField( "xFunc", TYPEID< StringTableEntry >(), Offset(xFunc, GraphEmitterData),
		"The expression specifying the emitted particles X coordinate." );

	addField( "yFunc", TYPEID< StringTableEntry >(), Offset(yFunc, GraphEmitterData),
		"The expression specifying the emitted particles Y coordinate." );

	addField( "zFunc", TYPEID< StringTableEntry >(), Offset(zFunc, GraphEmitterData),
		"The expression specifying the emitted particles Z coordinate." );

	addField( "funcMax", TYPEID< S32 >(), Offset(funcMax, GraphEmitterData),
		"The expressions interval, maximum." );

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
// GraphEmitter
//-----------------------------------------------------------------------------
GraphEmitter::GraphEmitter()
{
	particleProg = 0.0;

	lastErrorTime = 0;

	xFunc = "0";
	yFunc = "0";
	zFunc = "0";

	funcMax = 2000;
	funcMin = 0;

	particleProg = 0;

	ProgressMode = 0;

	Reverse = false;
	Loop = true;

	timeScale = 1.0;

	for(int i=0;i<100;i++)
	{
		xVariables[i].token = NULL;
		yVariables[i].token = NULL;
		zVariables[i].token = NULL;
		xVariables[i].value = NULL;
		yVariables[i].value = NULL;
		zVariables[i].value = NULL;
	}

	// --- x-function parser ---
	// Variables
	xfuncParser.DefineVar("t", &particleProg);
	// Constants
	xfuncParser.DefineConst("pi", M_PI_F);
	// Expression
	xfuncParser.SetExpr(xFunc);
	// Custom functions
	xfuncParser.DefineFun("randi", mu_RandomInteger, false);
	xfuncParser.DefineFun("srandi", mu_SeededRandomInteger, false);
	xfuncParser.DefineFun("randf", mu_RandomFloat, false);
	xfuncParser.DefineFun("srandf", mu_SeededRandomFloat, false);
	// Custom operators
	xfuncParser.DefineOprt("%", mu_ModulusOprt, 
		mu::EOprtPrecedence::prMUL_DIV, 
		mu::EOprtAssociativity::oaLEFT, 
		true);
	
	// --- y-function parser ---
	// Variables
	yfuncParser.DefineVar("t", &particleProg);
	// Constants
	yfuncParser.DefineConst("pi", M_PI_F);
	// Expression
	yfuncParser.SetExpr(yFunc);
	// Custom functions
	yfuncParser.DefineFun("randi", mu_RandomInteger, false);
	yfuncParser.DefineFun("srandi", mu_SeededRandomInteger, false);
	yfuncParser.DefineFun("randf", mu_RandomFloat, false);
	yfuncParser.DefineFun("srandf", mu_SeededRandomFloat, false);
	// Custom operators
	yfuncParser.DefineOprt("%", mu_ModulusOprt, 
		mu::EOprtPrecedence::prMUL_DIV, 
		mu::EOprtAssociativity::oaLEFT, 
		true);
	
	// --- z-function parser ---
	// Variables
	zfuncParser.DefineVar("t", &particleProg);
	// Constants
	zfuncParser.DefineConst("pi", M_PI_F);
	// Expression
	zfuncParser.SetExpr(zFunc);
	// Custom functions
	zfuncParser.DefineFun("randi", mu_RandomInteger, false);
	zfuncParser.DefineFun("srandi", mu_SeededRandomInteger, false);
	zfuncParser.DefineFun("randf", mu_RandomFloat, false);
	zfuncParser.DefineFun("srandf", mu_SeededRandomFloat, false);
	// Custom operators
	zfuncParser.DefineOprt("%", mu_ModulusOprt, 
		mu::EOprtPrecedence::prMUL_DIV, 
		mu::EOprtAssociativity::oaLEFT, 
		true);

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

		stream->readString(buf);
		*zFunc = *IPSCore::UpToLow(dStrdup(buf));

		xfuncParser.SetExpr(IPSCore::UpToLow(xFunc));

		yfuncParser.SetExpr(IPSCore::UpToLow(yFunc));

		zfuncParser.SetExpr(IPSCore::UpToLow(zFunc));

		funcMax = stream->readInt(32);
		funcMin = stream->readInt(32);

		timeScale = stream->readInt(15) / 1000.0f;
		ProgressMode = stream->readInt(1);
		Reverse = stream->readFlag();
		Loop = stream->readFlag();
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

	xfuncParser.SetExpr(IPSCore::UpToLow(xFunc));
	yfuncParser.SetExpr(IPSCore::UpToLow(yFunc));
	zfuncParser.SetExpr(IPSCore::UpToLow(zFunc));

	funcMax = DataBlock->funcMax;
	funcMin = DataBlock->funcMin;

	ProgressMode = DataBlock->ProgressMode;

	Reverse = DataBlock->Reverse;
	if(Reverse)
		particleProg = funcMax;
	else
		particleProg = funcMin;
	Loop = DataBlock->Loop;

	timeScale = DataBlock->timeScale;
   grounded = DataBlock->Grounded;

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
							   const Point3F& axisx,
							   const MatrixF& trans)
{
   if(!isClientObject())
      return false;
	PROFILE_SCOPE(GraphEmitAddPart);
	IPSBenchmarkBegin;
	GraphEmitterData* DataBlock = getDataBlock();
   Particle* pNew = mParticleManager->AddParticle();

	F32 initialVel;
	// If it is a standAloneEmitter, then we want it to use the sa values from the node
	initialVel = DataBlock->ejectionVelocity;
	initialVel    += (DataBlock->velocityVariance * 2.0f * gRandGen.randF()) - DataBlock->velocityVariance;
	// Set the time since this code was last run
	U32 dt = mInternalClock - oldTime;
	oldTime = mInternalClock;

	// Did we hit the upper limit?
	if(particleProg > funcMax)
	{
		if(Loop)
			particleProg = funcMin;
	}
	// Did we hit the lower limit?
	if(particleProg < funcMin)
	{
		if(Loop)
			particleProg = funcMax;
	}
	// We don't want to risk dividing by zero.
	//  - We don't care too much about accuracy, so whatever is close to zero is fine.
	if(particleProg == 0)
	{
		particleProg = F32_MIN;
	}

	F32 resultx = 0;
	F32 resulty = 0;
	F32 resultz = 0;
	// Evaluate the expressions and get the results.
	// Limit the amount of error messages to avoid lag
	if(mInternalClock - lastErrorTime > 5000)
	{
	try{
		resultx = xfuncParser.Eval();
		resulty = yfuncParser.Eval();
		resultz = zfuncParser.Eval();
	}
	catch(mu::Parser::exception_type &e)
	{
				lastErrorTime = mInternalClock;
		std::string expr = e.GetExpr();
		std::string tok = e.GetToken();
		size_t pos = e.GetPos();
		std::string msg = e.GetMsg();
		Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
	}
   }

   if(grounded)
	{
		F32 theHeight;
		Point3F TerNorm;
		if(!IPSCore::GetTerrainInfoAtPosition(pos.x+resultx, pos.y+resulty, theHeight, TerNorm))
			return false;
		resultz += theHeight;
		// Construct a vector from the 3 results
		Point3F funcPos = Point3F(resultx, resulty, resultz - pos.z);
		// Rotate our point by the rotation matrix
		trans.mulV(funcPos);

		// Add the position of the node to get coordinates in object space
		//  - and set the position of the new particle.
		pNew->pos = pos + funcPos + (TerNorm * mDataBlock->ejectionOffset);

		pNew->relPos = funcPos + TerNorm * mDataBlock->ejectionOffset;
	}
	else{
		// Construct a vector from the 3 results
	   Point3F funcPos = Point3F(resultx, resulty, resultz);

		// Rotate our point by the rotation matrix
		Point3F p;

		Point3F axisZ(axis.x,axis.y,0.0f);  
		if(axisZ.isZero())  
			axisZ.set(1.0,0.0,0.0);  
		else  
			axisZ.normalize();  

		mCross(axis,axisx,&axisZ);  

		MatrixF rotMat;
		rotMat.setColumn(0,axisx);  
		rotMat.setColumn(1,axis);  
		rotMat.setColumn(2,axisZ);

		rotMat.mulV(funcPos, &p);

		// Add the position of the node to get coordinates in object space
		//  - and set the position of the new particle.
		pNew->pos = pos + (p * mDataBlock->ejectionOffset);

		pNew->relPos = p * mDataBlock->ejectionOffset;
	}
		// Increment the t value based on the progressmode
		if(ProgressMode == gProgressMode::byParticleCount){
			if(Reverse)
				particleProg = (particleProg - (1 * timeScale));
			else
				particleProg = (particleProg + (1 * timeScale));
		}
		if(ProgressMode == gProgressMode::byTime){
			if(Reverse)
				particleProg = (particleProg - (dt * timeScale));
			else
				particleProg = (particleProg + (dt * timeScale));
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
   mParticleManager->initializeParticle(pNew, vel, mDataBlock->particleDataBlocks[dBlockIndex]);
		updateKeyData( pNew );
	IPSBenchmarkEnd("----Graph---- addPart");
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
   if(!isClientObject())
      return false;
	PROFILE_SCOPE(GraphEmitAddPartNode);
	IPSBenchmarkBegin;
	GraphEmitterData* DataBlock = getDataBlock();
   Particle* pNew = mParticleManager->AddParticle();

	F32 initialVel;
	// If it is a standAloneEmitter, then we want it to use the sa values from the node
	if(standAloneEmitter)
	{
		initialVel = sa_ejectionVelocity;
		initialVel    += (sa_velocityVariance * 2.0f * gRandGen.randF()) - sa_velocityVariance;
	}
	else
	{
		initialVel = DataBlock->ejectionVelocity;
		initialVel    += (DataBlock->velocityVariance * 2.0f * gRandGen.randF()) - DataBlock->velocityVariance;
	}

	if(pnodeDat)
	{
		// Set the time since this code was last run
		U32 dt = mInternalClock - oldTime;
		oldTime = mInternalClock;

		// Did we hit the upper limit?
		if(particleProg > funcMax)
		{
			if(Loop)
				particleProg = funcMin;
			onBoundaryLimit(true, pnodeDat);
		}
		// Did we hit the lower limit?
		if(particleProg < funcMin)
		{
			if(Loop)
				particleProg = funcMax;
			onBoundaryLimit(false, pnodeDat);
		}
		// We don't want to risk dividing by zero.
		//  - We don't care too much about accuracy, so whatever is close to zero is fine.
		if(particleProg == 0)
		{
			particleProg = F32_MIN;
		}

		F32 resultx = 0;
		F32 resulty = 0;
		F32 resultz = 0;
		// Evaluate the expressions and get the results.
		// Limit the amount of error messages to avoid lag
		if(mInternalClock - lastErrorTime > 5000)
		{
		try{
			resultx = xfuncParser.Eval();
			resulty = yfuncParser.Eval();
			resultz = zfuncParser.Eval();
		}
		catch(mu::Parser::exception_type &e)
		{
				lastErrorTime = mInternalClock;
				std::string expr = e.GetExpr();
				std::string tok = e.GetToken();
				size_t pos = e.GetPos();
				std::string msg = e.GetMsg();
				Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
			}
		}
      bool isGrounded = standAloneEmitter ? grounded : DataBlock->Grounded;
      if(isGrounded)
		{
			F32 theHeight;
			Point3F TerNorm;
			if(!IPSCore::GetTerrainInfoAtPosition(pos.x+resultx, pos.y+resulty, theHeight, TerNorm))
				return false;
			resultz += theHeight;
			// Construct a vector from the 3 results
			Point3F funcPos = Point3F(resultx, resulty, resultz - pos.z);
			// Get the transform of the node to get the rotation matrix
			MatrixF trans = getTransform();
			// Rotate our point by the rotation matrix
			trans.mulV(funcPos);

			// Add the position of the node to get coordinates in object space
			//  - and set the position of the new particle.
			if(standAloneEmitter){
				pNew->pos = pos + funcPos + (TerNorm * sa_ejectionOffset);

				pNew->relPos = funcPos + TerNorm * sa_ejectionOffset;
			}
			else{
				pNew->pos = pos + funcPos + (TerNorm * mDataBlock->ejectionOffset);

				pNew->relPos = funcPos + TerNorm * mDataBlock->ejectionOffset;
			}
		}
		else{
			// Construct a vector from the 3 results
			Point3F funcPos = Point3F(resultx, resulty, resultz);

			// Get the transform of the node to get the rotation matrix
			MatrixF trans = pnodeDat->getTransform();
			// Rotate our point by the rotation matrix
			Point3F p;
			trans.mulV(funcPos, &p);

			// Add the position of the node to get coordinates in object space
			//  - and set the position of the new particle.
			if(standAloneEmitter){
				pNew->pos = pos + (p * sa_ejectionOffset);

				pNew->relPos = p * sa_ejectionOffset;
			}
			else{
				pNew->pos = pos + (p * mDataBlock->ejectionOffset);

				pNew->relPos = p * mDataBlock->ejectionOffset;
			}
		}

		// Increment the t value based on the progressmode
		if(ProgressMode == gProgressMode::byParticleCount){
			if(Reverse)
				particleProg = (particleProg - (1 * timeScale));
			else
				particleProg = (particleProg + (1 * timeScale));
		}
		if(ProgressMode == gProgressMode::byTime){
			if(Reverse)
				particleProg = (particleProg - (dt * timeScale));
			else
				particleProg = (particleProg + (dt * timeScale));
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
      mParticleManager->initializeParticle(pNew, vel, mDataBlock->particleDataBlocks[dBlockIndex]);
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
						char name = NULL;

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
      ParticleEmitterNode* m_Node = dynamic_cast< ParticleEmitterNode* >(conn->resolveObjectFromGhostIndex(mNode));
		if(m_Node)
         ((GraphEmitter*)m_Node->getEmitter())->onBoundaryLimit(Max, m_Node);
	}
	catch(...){
		Con::printf("Callback error");
	}
}

IMPLEMENT_CO_NETEVENT_V1(CallbackEvent);

IMPLEMENT_CALLBACK(GraphEmitterData, onBoundaryLimit, void, ( GameBase* obj, bool Max), ( obj, Max ),
				   "@brief Informs a graphEmitter that it has hit a boundary and is now resetting the t value.n"
				   "@param Max - did it hit the maximum boundary or the minimum boundary?.n"
				   );

ParticleEmitter* GraphEmitterData::createEmitter() { return new GraphEmitter; }

DefineEngineMethod(GraphEmitterData, reload, void,(),,
				   "Reloads the ParticleData datablocks and other fields used by this emitter.\n"
				   "@tsexample\n"
				   "// Get the editor's current particle emitter\n"
				   "%emitter = PE_EmitterEditor.currEmitter\n\n"
				   "// Change a field value\n"
				   "%emitter.setFieldValue( %propertyField, %value );\n\n"
				   "// Reload this emitter\n"
				   "%emitter.reload();\n"
				   "@endtsexample\n")
{
	object->reload();
}