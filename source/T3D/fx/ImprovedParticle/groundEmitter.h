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

#ifndef GROUND_EMITTER_H_
#define GROUND_EMITTER_H_

#include "T3D\fx\particleEmitter.h"

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif

class GroundEmitterData : public ParticleEmitterData
{
	typedef ParticleEmitterData Parent;

	//------- Functions -------
public:
	GroundEmitterData();
	DECLARE_CONOBJECT(GroundEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	virtual ParticleEmitter* createEmitter();

	//------- Variables -------
public:
	F32   radius;
};

class GroundEmitter : public ParticleEmitter
{
	typedef ParticleEmitter Parent;

	//------- Enums -------
	enum MaskBits
	{
		exprEdited		= Parent::NextFreeMask << 0,
		dynamicMod		= Parent::NextFreeMask << 1,
		NextFreeMask	= Parent::NextFreeMask << 2,
	};

public:
	enum EnumProgressMode {
		byParticleCount = 0,
		byTime,
	};

	//------- Functions -------
public:
	GroundEmitter();
	DECLARE_CONOBJECT(GroundEmitter);

	bool onNewDataBlock( GameBaseData *dptr, bool reload );
	
	static void initPersistFields();
	
	bool	GetTerrainHeightAndMat(const F32 &x, const F32 &y, F32 &height, StringTableEntry &mat, Point3F &normal);

protected:
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx);
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node);

	//------- Variables -------
public:

private:
	Point3F parentNodePos;

	//------- Callbacks -------
public:

private:
	virtual GroundEmitterData* getDataBlock() { return static_cast<GroundEmitterData*>(Parent::getDataBlock()); }
};

#endif // GROUND_EMITTER_H_
=======
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

#ifndef _H_Ground_EMITTER
#define _H_Ground_EMITTER

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
#ifndef _Ground_EMITTERNODE_H_
#include "GroundEmitterNode.h"
#endif

#if defined(TORQUE_OS_XENON)
#include "gfx/D3D9/360/gfx360MemVertexBuffer.h"
#endif

class RenderPassManager;
class ParticleData;
class BaseMatInstance;
class TerrainBlock;

//*****************************************************************************
// Particle Emitter Data
//*****************************************************************************
class GroundEmitterData : public GameBaseData
{
	typedef GameBaseData Parent;

	static bool _setAlignDirection( void *object, const char *index, const char *data );
public:

	GroundEmitterData();
	DECLARE_CONOBJECT(GroundEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool preload(bool server, String &errorStr);
	bool onAdd();
	void allocPrimBuffer( S32 overrideSize = -1 );

public:
	S32   ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32   periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n

	F32   ejectionVelocity;                   ///< Ejection velocity
	F32   velocityVariance;                   ///< Variance for velocity between 0 and n
	F32   ejectionOffset;                     ///< Z offset from emitter point to eject from

	F32   thetaMin;                           ///< Minimum angle, from the horizontal plane, to eject from
	F32   thetaMax;                           ///< Maximum angle, from the horizontal plane, to eject from

	F32   softnessDistance;                   ///< For soft particles, the distance (in meters) where particles will be faded
	///< based on the difference in depth between the particle and the scene geometry.

	F32   radius;

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
class GroundEmitter : public GameBase
{
	typedef GameBase Parent;

	U32	oldTime;
	Point3F* rotate(MatrixF trans, Point3F p);
	Point3F parentNodePos;

public:

#if defined(TORQUE_OS_XENON)
	typedef GFXVertexPCTT ParticleVertexType;
#else
	typedef GFXVertexPCT ParticleVertexType;
#endif

	GroundEmitter();
	~GroundEmitter();

	DECLARE_CONOBJECT(GroundEmitter);

	static Point3F mWindVelocity;
	static void setWindVelocity( const Point3F &vel ){ mWindVelocity = vel; }

	ColorF getCollectiveColor();

	/// Sets sizes of particles based on sizelist provided
	/// @param   sizeList   List of sizes
	void setSizes( F32 *sizeList );

	/// Sets colors for particles based on color list provided
	/// @param   colorList   List of colors
	void setColors( ColorF *colorList );

	GroundEmitterData *getDataBlock(){ return mDataBlock; }
	bool onNewDataBlock( GameBaseData *dptr, bool reload );

	/// By default, a particle renderer will wait for it's owner to delete it.  When this
	/// is turned on, it will delete itself as soon as it's particle count drops to zero.
	void deleteWhenEmpty();


	bool		sticky;
	F32		attractionrange;
	S32		AttractionMode[attrobjectCount];
	F32		Amount[attrobjectCount];
	StringTableEntry	Attraction_offset[attrobjectCount];

	StringTableEntry attractedObjectID[attrobjectCount];

	bool	GetTerrainHeightAndMat(const F32 &x, const F32 &y, F32 &height, StringTableEntry &mat, Point3F &normal);

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

	// Added the GroundEmitterNode here
	void emitParticles(	const U32 numMilliseconds, GroundEmitterNode* node);

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

	// Added the GroundEmitterNode here
	bool addParticle(GroundEmitterNode* node);


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

	GroundEmitterData* mDataBlock;

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

};

#endif // _H_Ground_EMITTER

