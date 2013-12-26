#ifndef _H_PARTICLE_EMITTER
#define _H_PARTICLE_EMITTER

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

#ifndef _PARTICLEEMITTERDUMMY_H_
#include "particleEmitterNode.h"
#endif

#if defined(TORQUE_OS_XENON)
#include "gfx/D3D9/360/gfx360MemVertexBuffer.h"
#endif

#ifndef PARTICLE_BEHAVIOUR_H
#include "ImprovedParticle\ParticleBehaviours\particleBehaviour.h"
#endif

#include "ImprovedParticle\IPSCore.h"
#include "ImprovedParticle\ParticleManagers\particleManager.h"

#ifndef ParticleBehaviourCount
#define ParticleBehaviourCount (U8)8
#endif

class RenderPassManager;
class ParticleData;
class IParticleBehaviour;

//*****************************************************************************
// Particle Emitter Data
//*****************************************************************************
class ParticleEmitterData : public GameBaseData
{
	typedef GameBaseData Parent;

protected:
	static bool _setAlignDirection( void *object, const char *index, const char *data );

public:

	//------- Functions -------
	//--------------------------------------------
	// ParticleEmitterData
	//--------------------------------------------
	ParticleEmitterData();
	//DECLARE_CONOBJECT(ParticleEmitterData);
	static void initPersistFields();
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool preload(bool server, String &errorStr);
	bool onAdd();
	void allocPrimBuffer( S32 overrideSize = -1 );
	virtual ParticleEmitter* createEmitter() = 0;
	bool reload();

   
	//------- Fields -------
	// Common variables

	S32   ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32   periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n

	F32   ejectionVelocity;                   ///< Ejection velocity
	F32   velocityVariance;                   ///< Variance for velocity between 0 and n
	F32   ejectionOffset;                     ///< Z offset from emitter point to eject from

	F32	  softnessDistance;                   ///< For soft particles, the distance (in meters) where particles will be faded
	///< based on the difference in depth between the particle and the scene geometry.
	// LOD Variables
	F32   mediumResDistance;                  ///< When to switch to the medium resolution texture
	F32   lowResDistance;                     ///< When to switch to the low resolution texture
	F32   ejectionLODStartDistance;           ///< When to start increasing the ejection period
	F32   ejectionLODEndDistance;             ///< When to stop increasing the ejection period, and stop emitting particles
	S32   lodEjectionPeriod;                  ///< What to level the ejection period to
	F32   SimulationLODBegin;
	F32   SimulationLODEnd;

	/// A scalar value used to influence the effect 
	/// of the ambient color on the particle.
	F32 ambientFactor;

	U32   lifetimeMS;                         ///< Lifetime of particles
	U32   lifetimeVarianceMS;                 ///< Varience in lifetime from 0 to n

	bool  overrideAdvance;                    ///<
	bool  orientParticles;                    ///< Particles always face the screen
	bool  orientOnVelocity;                   ///< Particles face the screen at the start
	bool  mUseEmitterSizes;                    ///< Use emitter specified sizes instead of datablock sizes
	bool  mUseEmitterColors;                   ///< Use emitter specified colors instead of datablock colors
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

	//------------------------- Stand alone variables
	bool				standAloneEmitter;
	/*S32					sa_ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32					sa_periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n
	
	F32					sa_ejectionVelocity;                   ///< Ejection velocity
	F32					sa_velocityVariance;                   ///< Variance for velocity between 0 and n
	F32					sa_ejectionOffset;                     ///< Z offset from emitter point to eject from*/

	SimObjectPtr<IParticleBehaviour> ParticleBHVs[ParticleBehaviourCount];

};

//*****************************************************************************
// Particle Emitter
//*****************************************************************************
class ParticleEmitter : public GameBase
{
	typedef GameBase Parent;
public:
	//------- Enums -------
	enum MaskBits
	{
		sa_Mask		 =	Parent::NextFreeMask << 0,
		NextFreeMask =	Parent::NextFreeMask << 1,
	};

	//------------------------- Stand alone variables
	bool				standAloneEmitter;
	S32					sa_ejectionPeriodMS;                   ///< Time, in Milliseconds, between particle ejection
	S32					sa_periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n
	
	F32					sa_ejectionVelocity;                   ///< Ejection velocity
	F32					sa_velocityVariance;                   ///< Variance for velocity between 0 and n
	F32					sa_ejectionOffset;                     ///< Z offset from emitter point to eject from
	S32					RenderEjectionPeriodMS;
	U32					CurrentUpdateIndex;
	U32					MaxUpdateIndex;
	U32					TSU;

	bool				grounded;
	bool				mActive;
	SimObjectPtr<IParticleBehaviour> ParticleBHVs[ParticleBehaviourCount];

	U32					oldTime;
	Point3F				parentNodePos;
	Box3F				parentNodeBox;
	bool				Dirty;

   ParticleManager* mParticleManager;
	
	U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);

	virtual void onStaticModified(const char* slotName, const char*newValue);
   virtual void onDynamicModified(const char* slotName, const char*newValue) {};

	ParticleEmitter();
	~ParticleEmitter();
	//DECLARE_CONOBJECT(ParticleEmitter);

	static Point3F mWindVelocity;
	static void setWindVelocity( const Point3F &vel ){ mWindVelocity = vel; }

	ColorF getCollectiveColor();

	/// Sets sizes of particles based on sizelist provided
	/// @param   sizeList   List of sizes
	void setSizes( F32 *sizeList );

	/// Sets colors for particles based on color list provided
	/// @param   colorList   List of colors
	void setColors( ColorF *colorList );

	ParticleEmitterData *getDataBlock(){ return mDataBlock; }
	bool onNewDataBlock( GameBaseData *dptr, bool reload );

	/// By default, a particle renderer will wait for it's owner to delete it.  When this
	/// is turned on, it will delete itself as soon as it's particle count drops to zero.
	void deleteWhenEmpty();

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

	// For nodes
	void emitParticles( const U32 numMilliseconds, ParticleEmitterNode* node );
	// For projectiles
	void emitParticles(const Point3F& start,
		const Point3F& end,
		const Point3F& axis,
		const Point3F& velocity,
		const U32      numMilliseconds);
	// For...
	void emitParticles(const Point3F& point,
		const bool     useLastPosition,
		const Point3F& axis,
		const Point3F& velocity,
		const U32      numMilliseconds);
	// For...
	void emitParticles(const Point3F& rCenter,
		const Point3F& rNormal,
		const F32      radius,
		const Point3F& velocity,
		S32 count);

	/// @}

	bool mDead;
	
	/// @name Internal interface
	/// @{

	/// Adds a particle
	/// @param   pos   Initial position of particle
	/// @param   axis
	/// @param   vel   Initial velocity
	/// @param   axisx
	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, const MatrixF& trans) = 0;

	virtual bool addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, ParticleEmitterNode* node) = 0;

	/// Updates the bounding box for the particle system
	void updateBBox();

	/// @}
	bool onAdd();
	void onRemove();

	void processTick(const Move *move);
	void advanceTime(F32 dt);

	// PEngine interface
	protected:
	void update( U32 ms );
	inline void updateKeyData( Particle *part, U32 numMilliseconds = 0 );

	// Rendering
	void prepRenderImage( SceneRenderState *state );

	ParticleEmitterData* mDataBlock;

	U32       mInternalClock;

	U32       mNextParticleTime;

	bool      mDeleteWhenEmpty;
	bool      mDeleteOnTick;

	S32       mLifetimeMS;
	S32       mElapsedTimeMS;

public:
	/// Constant used to calculate particle 
	/// rotation from spin and age.
	static const F32 AgedSpinToRadians;

	Point3F   mLastPosition;
	bool      mHasLastPosition;
	MatrixF   mBBObjToWorld;

	F32       sizes[ ParticleData::PDC_NUM_KEYS ];
	ColorF    colors[ ParticleData::PDC_NUM_KEYS ];

};

class ParticleEmitterWrapper : public IParticleEmitter
{
public:
   ParticleEmitter* object;
   ParticleEmitterWrapper(ParticleEmitter* obj) { object = obj; };
	//------- Functions -------
	//--------------------------------------------
	// IParticleEmitter
	//--------------------------------------------
   bool isDead() { return object->mDead; };
   const Box3F& getRenderWorldBox() { return object->getRenderWorldBox(); };
   Box3F getParentNodeBox() { return object->parentNodeBox; };
   ColorF* getColors() { return object->colors; };
   F32* getSizes() { return object->sizes; };
   MatrixF getBBObjToWorld() { return object->mBBObjToWorld; };
   Point3F getLastPosition() { return object->mLastPosition; };
   Point3F getParentNodePos() { return object->parentNodePos; };
   const F32 getAgedSpinToRadians() { return object->AgedSpinToRadians; };
   ParticleEmitter* getPointer() { return object; };
};

class ParticleEmitterDataWrapper : public IParticleEmitterData
{
public:
   ParticleEmitterData* object;
   ParticleEmitterDataWrapper(ParticleEmitterData* obj) { object = obj; };
	//------- Functions -------
	//--------------------------------------------
	// IParticleEmitterData
	//--------------------------------------------
   bool isRenderingReflections() { return object->renderReflection; };
   bool isHighResOnly() { return object->highResOnly; };
   bool isOrientedOnVelocity() { return object->orientOnVelocity; };
   bool useEmitterColors() { return object->mUseEmitterColors; };
   bool useEmitterSizes() { return object->mUseEmitterSizes; };
   bool useReversedOrder() { return object->reverseOrder; };
   bool useSortedParticles() { return object->sortParticles; };
   bool useAlignedParticles() { return object->alignParticles; };
   bool useOrientedParticles() { return object->orientParticles; };
   U32 getPartListInitSize() { return object->partListInitSize; };
   S32 getBlendStyle() { return object->blendStyle; };
   F32 getMediumResDistance() { return object->mediumResDistance; };
   F32 getLowResDistance() { return object->lowResDistance; };
   F32 getSoftnessDistance() { return object->softnessDistance; };
   F32 getAmbientFactor() { return object->ambientFactor; };
   VectorF getAlignDirection() { return object->alignDirection; };
   GFXTexHandle getTextureHandle() { return object->textureHandle; };
   GFXPrimitiveBufferHandle getPrimBuff() { return object->primBuff; };
   GFXPrimitiveBufferHandle* getPrimBuffRef() { return &object->primBuff; };
   void allocPrimBuffer( S32 overrideSize = -1 ) { object->allocPrimBuffer(overrideSize); };
   ParticleEmitterData* getPointer() { return object; };
};

#endif // _H_PARTICLE_EMITTER