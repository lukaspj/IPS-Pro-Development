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

GraphEmitterNodeData::GraphEmitterNodeData()
{
}

void GraphEmitterNodeData::initPersistFields()
{
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
// SphereEmitterNode
//-----------------------------------------------------------------------------
GraphEmitterNode::GraphEmitterNode()
{
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

	// Todo: Uncomment if this is a "leaf" class
	//scriptOnNewDataBlock();
	return true;
}

void GraphEmitterNode::advanceTime(F32 dt)
{
	Parent::advanceTime(dt);
	GraphEmitterNodeData* DataBlock = getDataBlock();
	if(!mEmitter->isProperlyAdded() || !DataBlock)
		return;
	
	if(mEmitter){
		mEmitter->getObjBox().minExtents;
	}

	mEmitter->emitParticles( (U32)(dt * DataBlock->timeMultiple * 1000.0f), this );
}

void GraphEmitterNode::updateMaxMinDistances()
{
   GraphEmitter* emitter = (GraphEmitter*)mEmitter;
	if(!mEmitter)
		return;
	F32 tmpPartProg = emitter->particleProg;
	xMxDist = 0;
	xMnDist = 0;
	yMxDist = 0;
	yMnDist = 0;
	zMxDist = 0;
	zMnDist = 0;

	for(int i = emitter->funcMin; i <= emitter->funcMax; i++)
	{
		F32 xRes = 0.0f;
		F32 yRes = 0.0f;
		F32 zRes = 0.0f;
		emitter->particleProg = i;
		try{
			xRes = ((GraphEmitter*)mEmitter)->xfuncParser.Eval() * emitter->sa_ejectionOffset;
			yRes = ((GraphEmitter*)mEmitter)->yfuncParser.Eval() * emitter->sa_ejectionOffset;
			zRes = ((GraphEmitter*)mEmitter)->zfuncParser.Eval() * emitter->sa_ejectionOffset;
		}
		catch(mu::Parser::exception_type &e)
		{
			// Most likely cause is: The expression were updated before the dynamic variables were
			//  Don't want it to throw an error all the time, even tho it should throw an exception
			//  When thing really does go wrong -- Needs fix.
			std::string expr = e.GetExpr();
			std::string tok = e.GetToken();
			size_t pos = e.GetPos();
			std::string msg = e.GetMsg();
			Con::errorf("Parsing error! Failed to parse: \n %s\nAt token: %s\nAt position: %u\nMessage: %s",expr.c_str(),tok.c_str(),pos,msg.c_str());
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
	emitter->particleProg = tmpPartProg;
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