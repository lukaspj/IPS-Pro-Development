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

#ifndef _H_RadiusMesh_EMITTER
#define _H_RadiusMesh_EMITTER

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif
#ifndef _GFXPRIMITIVEBUFFER_H_
#include "gfx/gfxPrimitiveBuffer.h"
#endif
#ifndef _GFXVERTEXBUFFER_H_
#include "gfx/gfxVertexBuffer.h"
#endif
#ifndef _PARTICLE_H_
#include "T3D/fx/particle.h"
#endif

#include "ts\tsMesh.h"

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif

#ifndef _BITSTREAM_H_
#include "core/stream/bitStream.h"
#endif

#if defined(TORQUE_OS_XENON)
#include "gfx/D3D9/360/gfx360MemVertexBuffer.h"
#endif

#include "IPSCore.h"
#include "IPSEmitterHelper.h"

#include <vector>
#include <string>
#include <iosfwd>
#include <sstream>

class RenderPassManager;
class ParticleData;

#ifndef attrobjectCount
#define attrobjectCount (U8)2
#endif

//*****************************************************************************
// Particle Emitter Data
//*****************************************************************************
class RadiusMeshEmitterData : public GameBaseData
{
	typedef GameBaseData Parent;

	static bool _setAlignDirection( void *object, const char *index, const char *data );
public:

	RadiusMeshEmitterData();
	DECLARE_CONOBJECT(RadiusMeshEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool preload(bool server, String &errorStr);
	bool onAdd();
	void allocPrimBuffer( S32 overrideSize = -1 );

public:
	S32   ejectionPeriodMS;					///< Time, in Milliseconds, between particle ejection
	S32   periodVarianceMS;					///< Variance in ejection peroid between 0 and n

	F32   ejectionVelocity;					///< Ejection velocity
	F32   velocityVariance;					///< Variance for velocity between 0 and n
	F32   ejectionOffset;					///< Z offset from emitter point to eject from

	F32   softnessDistance;					///< For soft particles, the distance (in meters) where particles will be faded
	///< based on the difference in depth between the particle and the scene geometry.

	/// A scalar value used to influence the effect 
	/// of the ambient color on the particle.
	F32 ambientFactor;

	U32   lifetimeMS;                         ///< Lifetime of particles
	U32   lifetimeVarianceMS;                 ///< Varience in lifetime from 0 to n

	bool  overrideAdvance;                    ///<
	bool  orientParticles;                    ///< Particles always face the screen
	bool  orientOnVelocity;                   ///< Particles face the screen at the start
	bool  useEmitterSizes;                    ///< Use emitter specified sizes instead of datablock sizes
	bool  useEmitterColors;                   ///< Use emitter specified colors instead of datablock colors
	bool  alignParticles;                     ///< Particles always face along a particular axis
	Point3F alignDirection;                   ///< The direction aligned particles should face

	StringTableEntry      particleString;     ///< Used to load particle data directly from a string

	Vector<ParticleData*> particleDataBlocks; ///< Particle Datablocks 
	Vector<U32>           dataBlockIds;       ///< Datablock IDs (parellel array to particleDataBlocks)

	U32                   partListInitSize;   /// initial size of particle list calc'd from datablock info

	GFXPrimitiveBufferHandle   primBuff;

	S32                   blendStyle;         ///< Pre-define blend factor setting
	bool                  sortParticles;      ///< Particles are sorted back-to-front
	bool                  reverseOrder;       ///< reverses draw order
	StringTableEntry      textureName;        ///< Emitter texture file to override particle textures
	GFXTexHandle          textureHandle;      ///< Emitter texture handle from txrName
	bool                  highResOnly;        ///< This particle system should not use the mixed-resolution particle rendering
	bool                  renderReflection;   ///< Enables this emitter to render into reflection passes.

	bool reload();
};

//*****************************************************************************
// Particle Emitter
//*****************************************************************************
class RadiusMeshEmitter : public GameBase
{
	typedef GameBase Parent;

	U32	oldTime;
	Point3F* rotate(MatrixF trans, Point3F p);
	Point3F* TSrotate(MatrixF trans, Point3F p);

	Point3F parentNodePos;

	enum MaskBits
	{
		StateMask		= Parent::NextFreeMask << 0,
		RadiusMeshEmitterMask	= Parent::NextFreeMask << 1,
		particleMask	= Parent::NextFreeMask << 2,
		physicsMask		= Parent::NextFreeMask << 3,
		NextFreeMask	= Parent::NextFreeMask << 4
	};
public:

	// Particle settings ----------------------------------------------------------------
	S32   ejectionPeriodMS;					///< Time, in Milliseconds, between particle ejection
	S32   periodVarianceMS;					///< Variance in ejection peroid between 0 and n

	F32   ejectionVelocity;					///< Ejection velocity
	F32   velocityVariance;					///< Variance for velocity between 0 and n
	F32   ejectionOffset;					///< Z offset from emitter point to eject from

	U32   lifetimeMS;                         ///< Lifetime of particles
	U32   lifetimeVarianceMS;                 ///< Varience in lifetime from 0 to n

	F32   softnessDistance;					///< For soft particles, the distance (in meters) where particles will be faded
	///< based on the difference in depth between the particle and the scene geometry.

	/// A scalar value used to influence the effect 
	/// of the ambient color on the particle.
	F32 ambientFactor;

	bool  overrideAdvance;                    ///<
	bool  orientParticles;                    ///< Particles always face the screen
	bool  orientOnVelocity;                   ///< Particles face the screen at the start
	bool  useEmitterSizes;                    ///< Use emitter specified sizes instead of datablock sizes
	bool  useEmitterColors;                   ///< Use emitter specified colors instead of datablock colors
	bool  alignParticles;                     ///< Particles always face along a particular axis
	Point3F alignDirection;                   ///< The direction aligned particles should face

	static bool _setAlignDirection( void *object, const char *index, const char *data );

	S32                   blendStyle;         ///< Pre-define blend factor setting
	bool                  sortParticles;      ///< Particles are sorted back-to-front
	bool                  reverseOrder;       ///< reverses draw order
	StringTableEntry      textureName;        ///< Emitter texture file to override particle textures
	GFXTexHandle          textureHandle;      ///< Emitter texture handle from txrName
	bool                  highResOnly;        ///< This particle system should not use the mixed-resolution particle rendering
	bool                  renderReflection;   ///< Enables this emitter to render into reflection passes.
	// Particle settings ----------------------------------------------------------------

	// RadiusMesh variables ----------------------------------------------------------------
	Point3F center;
	F32 radius;
	bool active;
	void tickToDeath(bool boolean) { mDeleteWhenEmpty = boolean; 
	if(isServerObject())
		setMaskBits(RadiusMeshEmitterMask);};

	Vector<IPSCore::face> emitfaces;

	std::vector<std::string> initialValues;
	std::vector<std::string> anotherValues;
	//--- Debug
	bool renderIntersections;
	bool renderEmittingFaces;
	bool shadeByCoverage;
	bool enableDebugRender;
	bool renderSphere;
	//--- Debug
	// RadiusMesh variables ----------------------------------------------------------------

	void loadFaces();
	bool isObjectCulled();
	void debugRender(ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance* overrideMat);

	void onStaticModified(const char* slotName, const char*newValue);
	//void onDynamicModified(const char* slotName, const char*newValue);

	U32 vertexCount;
	Vector<int[3]> triList;
	Vector<int[2]> edgeList;

#if defined(TORQUE_OS_XENON)
	typedef GFXVertexPCTT ParticleVertexType;
#else
	typedef GFXVertexPCT ParticleVertexType;
#endif

	RadiusMeshEmitter();
	~RadiusMeshEmitter();

	DECLARE_CONOBJECT(RadiusMeshEmitter);

	static Point3F mWindVelocity;
	static void setWindVelocity( const Point3F &vel ){ mWindVelocity = vel; }

	ColorF getCollectiveColor();

	/// Sets sizes of particles based on sizelist provided
	/// @param   sizeList   List of sizes
	void setSizes( F32 *sizeList );

	/// Sets colors for particles based on color list provided
	/// @param   colorList   List of colors
	void setColors( ColorF *colorList );

	RadiusMeshEmitterData *getDataBlock(){ return mDataBlock; }
	bool onNewDataBlock( GameBaseData *dptr, bool reload );

	/// By default, a particle renderer will wait for it's owner to delete it.  When this
	/// is turned on, it will delete itself as soon as it's particle count drops to zero.
	void deleteWhenEmpty();

	StringTableEntry emitMesh;
	bool	evenEmission;
	bool	emitOnFaces;
	bool		sticky;
	F32		attractionrange;
	S32		AttractionMode[attrobjectCount];
	F32		Amount[attrobjectCount];
	StringTableEntry	Attraction_offset[attrobjectCount];

	S32		mainTime;

	StringTableEntry attractedObjectID[attrobjectCount];

	/// @name Particle Emission
	/// Main interface for creating particles.  The emitter does _not_ track changes
	///  in axis or velocity over the course of a single update, so this should be called
	///  at a fairly fine grain.  The emitter will potentially track the last particle
	///  to be created into the next call to this function in order to create a uniformly
	///  random time distribution of the particles.  If the object to which the emitter is
	///  attached is in motion, it should try to ensure that for call (n+1) to this
	///  function, start is equal to the end from call (n).  This will ensure a uniform
	///  spatial distribution.
	/// @{

	// Added the RadiusMeshEmitterNode here
	void emitParticles(
		const F32 velocity,
		const U32      numMilliseconds);

	/// @}

	bool mDead;

protected:
	/// @name Internal interface
	/// @{

	/// Adds a particle
	/// @param   pos   Initial position of particle
	/// @param   axis
	/// @param   vel   Initial velocity
	/// @param   axisx

	// Added the RadiusMeshEmitterNode here
	bool addParticle(const F32 &vel);


	inline void setupBillboard( Particle *part,
		Point3F *basePts,
		const MatrixF &camView,
		const ColorF &ambientColor,
		ParticleVertexType *lVerts );

	inline void setupOriented( Particle *part,
		const Point3F &camPos,
		const ColorF &ambientColor,
		ParticleVertexType *lVerts );

	inline void setupAligned(  const Particle *part, 
		const ColorF &ambientColor,
		ParticleVertexType *lVerts );

	/// Updates the bounding box for the particle system
	void updateBBox();

	/// @}
protected:
	bool onAdd();
	void onRemove();

	void processTick(const Move *move);
	void advanceTime(F32 dt);

	// Rendering
protected:
	void prepRenderImage( SceneRenderState *state );
	void copyToVB( const Point3F &camPos, const ColorF &ambientColor );

	// PEngine interface
private:

	void update( U32 ms );
	inline void updateKeyData( Particle *part );


private:

	/// Constant used to calculate particle 
	/// rotation from spin and age.
	static const F32 AgedSpinToRadians;

	RadiusMeshEmitterData* mDataBlock;

	U32       mInternalClock;

	U32       mNextParticleTime;

	Point3F   mLastPosition;
	bool      mHasLastPosition;
	MatrixF   mBBObjToWorld;

	bool      mDeleteWhenEmpty;
	bool      mDeleteOnTick;

	S32       mLifetimeMS;
	S32       mElapsedTimeMS;

	F32       sizes[ ParticleData::PDC_NUM_KEYS ];
	ColorF    colors[ ParticleData::PDC_NUM_KEYS ];

#if defined(TORQUE_OS_XENON)
	GFX360MemVertexBufferHandle<ParticleVertexType> mVertBuff;
#else
	GFXVertexBufferHandle<ParticleVertexType> mVertBuff;
#endif

	//   These members are for implementing a link-list of the active emitter 
	//   particles. Member part_store contains blocks of particles that can be
	//   chained in a link-list. Usually the first part_store block is large
	//   enough to contain all the particles but it can be expanded in emergency
	//   circumstances.
	Vector <Particle*> part_store;
	Particle*  part_freelist;
	Particle   part_list_head;
	S32        n_part_capacity;
	S32        n_parts;
	S32       mCurBuffSize;


public:

	enum EnumProgressMode {
		byParticleCount = 0,
		byTime,
	};

	enum EnumAttractionMode{
		none = 0,
		attract = 1,
		repulse = 2,
	};

	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

	void inspectPostApply();
	static void initPersistFields();
};

//*****************************************************************************
// GraphEmitterNetEvemt
//*****************************************************************************
// A simple NetEvent to transmit a string over the network.
// This is based on the code in netTest.cc

class rm_DeleteEvent : public NetEvent
{
	typedef NetEvent Parent;
	S32 mNode;
public:
	rm_DeleteEvent(S32 node = -1);
	virtual ~rm_DeleteEvent();

	virtual void pack   (NetConnection *conn, BitStream *bstream);
	virtual void write  (NetConnection *conn, BitStream *bstream);
	virtual void unpack (NetConnection *conn, BitStream *bstream);
	virtual void process(NetConnection *conn);

	DECLARE_CONOBJECT(rm_DeleteEvent);
};

#endif // _H_RadiusMesh_EMITTER
