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

#ifndef _H_MESH_EMITTER
#define _H_MESH_EMITTER

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif

#ifndef _PARTICLE_H_
#include "T3D/fx/particle.h"
#endif

#ifndef _GFXPRIMITIVEBUFFER_H_
#include "gfx/gfxPrimitiveBuffer.h"
#endif

#ifndef _GFXVERTEXBUFFER_H_
#include "gfx/gfxVertexBuffer.h"
#endif

#ifndef PARTICLE_BEHAVIOUR_H
#include "ParticleBehaviours\particleBehaviour.h"
#endif

#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "IPSCore.h"

#include "ParticleManagers\particleManager.h"

//--------------------------------------------
// MeshEmitterData
//--------------------------------------------
class MeshEmitterData : public GameBaseData
{
	typedef GameBaseData Parent;

public:

	//------- Functions -------

	//--------------------------------------------
	// SimObject
	//--------------------------------------------
	static void initPersistFields();
	virtual bool preload(bool server, String &errorStr);
	virtual bool onAdd();

	//--------------------------------------------
	// SimDatablock
	//--------------------------------------------
	virtual void packData(BitStream* stream);
	virtual void unpackData(BitStream* stream);

	//--------------------------------------------
	// MeshEmitterData
	//--------------------------------------------
	static bool _setAlignDirection( void *object, const char *index, const char *data );
	void allocPrimBuffer( S32 overrideSize = -1);
	bool reload();
	MeshEmitterData();


	//------- Fields -------

	//--------------------------------------------
	// ParticleEmitter standard fields
	//--------------------------------------------

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

	//--------------------------------------------
	// LOD fields
	//--------------------------------------------
	F32   mediumResDistance;                  ///< When to switch to the medium resolution texture
	F32   lowResDistance;                     ///< When to switch to the low resolution texture
	F32   ejectionLODStartDistance;           ///< When to start increasing the ejection period
	F32   ejectionLODEndDistance;             ///< When to stop increasing the ejection period, and stop emitting particles
	S32   lodEjectionPeriod;                  ///< What to level the ejection period to
	F32   SimulationLODBegin;
	F32   SimulationLODEnd;
	
	SimDataBlock* ParticleBHVs[ParticleBehaviourCount];


	DECLARE_CONOBJECT(MeshEmitterData);
};

//--------------------------------------------
// MeshEmitter
//--------------------------------------------
class MeshEmitter : public GameBase
{
	typedef GameBase Parent;

	//------- typedefs -------
#if defined(TORQUE_OS_XENON)
	typedef GFXVertexPCTT ParticleVertexType;
#else
	typedef GFXVertexPCT ParticleVertexType;
#endif

#if defined(TORQUE_OS_XENON)
	GFX360MemVertexBufferHandle<ParticleVertexType> mVertBuff;
#else
	GFXVertexBufferHandle<ParticleVertexType> mVertBuff;
#endif

	//------- Enums -------
protected:
	enum MaskBits
	{
		StateMask		 = Parent::NextFreeMask << 0,
		GeometryMask	 = Parent::NextFreeMask << 1,
      MeshEmitterMask = Parent::NextFreeMask << 2,
		particleMask	 = Parent::NextFreeMask << 3,
      SkipReloadMask  = Parent::NextFreeMask << 4,
		NextFreeMask	 = Parent::NextFreeMask << 5
	};

public:
	//------- Functions -------
	//--------------------------------------------
	// MeshEmitter
	//--------------------------------------------
	MeshEmitter();
	~MeshEmitter();
	virtual void addParticle(const F32 &vel);
	virtual bool getPointOnVertex(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew);
	virtual bool getPointOnFace(SimObject *SB, ShapeBase *SS, TSStatic* TS, Particle *pNew);
	virtual void loadFaces(SimObject *SB, ShapeBase *SS, TSStatic* TS);
	virtual void loadFaces();
	virtual void debugRenderDelegate(ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance* overrideMat) { };
	virtual void bindDelegate(ObjectRenderInst *ori) { ori->renderDelegate.bind( this,  &MeshEmitter::debugRenderDelegate ); };

	static bool _setAlignDirection( void *object, const char *index, const char *data);
	static void setWindVelocity( const Point3F &vel ) { mWindVelocity = vel; }
	ColorF getCollectiveColor();
	/// Sets sizes of particles based on sizelist provided
	/// @param   sizeList   List of sizes
	void setSizes( F32 *sizeList );

	/// Sets colors for particles based on color list provided
	/// @param   colorList   List of colors
	void setColors( ColorF *colorList );
	MeshEmitterData *getDataBlock(){ return mDataBlock; }

	void deleteWhenEmpty();
	void emitParticles(
		const F32 velocity,
		const U32      numMilliseconds);

	/// Updates the bounding box for the particle system
	void updateBBox();

	inline void updateKeyData( Particle *part, U32 numMilliseconds = 0 );

	//--------------------------------------------
	// SimObject
	//--------------------------------------------
	virtual void onStaticModified(const char *slotName, const char *newValue);
	virtual bool onAdd();
	virtual void onRemove();
	virtual void inspectPostApply();
	static void initPersistFields();

	//--------------------------------------------
	// GameBase
	//--------------------------------------------
	virtual bool onNewDataBlock( GameBaseData *dptr, bool reload );
	virtual U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
	virtual void unpackUpdate(NetConnection *conn,           BitStream* stream);

	//--------------------------------------------
	// ITickable
	//--------------------------------------------
	virtual void processTick(const Move *move);
	virtual void advanceTime(F32 dt);
	virtual void update( U32 ms );

	//--------------------------------------------
	// SceneObject
	//--------------------------------------------
	virtual void prepRenderImage( SceneRenderState *state );

	//------- Fields -------
	//--------------------------------------------
	// ParticleEmitter standard fields
	//--------------------------------------------
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

	S32                   blendStyle;         ///< Pre-define blend factor setting
	bool                  sortParticles;      ///< Particles are sorted back-to-front
	bool                  reverseOrder;       ///< reverses draw order
	StringTableEntry      textureName;        ///< Emitter texture file to override particle textures
	GFXTexHandle          textureHandle;      ///< Emitter texture handle from txrName
	bool                  highResOnly;        ///< This particle system should not use the mixed-resolution particle rendering
	bool                  renderReflection;   ///< Enables this emitter to render into reflection passes.
	static Point3F mWindVelocity;
	bool mDead;

	/// Constant used to calculate particle 
	/// rotation from spin and age.
	static const F32 AgedSpinToRadians;

	MeshEmitterData* mDataBlock;

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
   
   ParticleManager* mParticleManager;

	//--------------------------------------------
	// MeshEmitter fields
	//--------------------------------------------
	Vector<IPSCore::face>	emitfaces;				///< Faces to emit particles on
	U32						vertexCount;			///< Amount of vertices in the mesh
	StringTableEntry		emitMesh;				///< Id of the object that has a mesh that we want to emit particles on
	bool					evenEmission;			///< Even the emission
	bool					emitOnFaces;			///< If true, emits particles on faces rather than vertices
	bool					mActive;				///< If true, emits particles

	U32						oldTime;				
	S32						mainTime;

	bool					isObjectCulled;

	Vector<String>			initialValues;

	Point3F					parentNodePos;
	
	//--------------------------------------------
	// LOD fields
	//--------------------------------------------
	S32						RenderEjectionPeriodMS;	///< True ejectionperiod
	U32						CurrentUpdateIndex;		///< Current update count
	U32						MaxUpdateIndex;			///< Maximum number of updates
	U32						TSU;					///< Time Since Update

	//--------------------------------------------
	// Debug fields
	//--------------------------------------------
	bool					enableDebugRender;

	DECLARE_CONOBJECT(MeshEmitter);
};

class MeshEmitterWrapper : public IParticleEmitter
{
public:
   MeshEmitter* object;
   MeshEmitterWrapper(MeshEmitter* obj) { object = obj; };
	//------- Functions -------
	//--------------------------------------------
	// IParticleEmitter
	//--------------------------------------------
   bool isDead() { return object->mDead; };
   const Box3F& getRenderWorldBox() { return object->getRenderWorldBox(); };
   Box3F getParentNodeBox() { return object->getWorldBox(); };
   ColorF* getColors() { return object->colors; };
   F32* getSizes() { return object->sizes; };
   MatrixF getBBObjToWorld() { return object->mBBObjToWorld; };
   Point3F getLastPosition() { return object->mLastPosition; };
   Point3F getParentNodePos() { return object->parentNodePos; };
   const F32 getAgedSpinToRadians() { return object->AgedSpinToRadians; };
   SimObject* getPointer() { return object; };
};

class MeshEmitterDataWrapper : public IParticleEmitterData
{
public:
   MeshEmitterData* object;
   MeshEmitterDataWrapper(MeshEmitterData* obj) { object = obj; };
	//------- Functions -------
	//--------------------------------------------
	// IParticleEmitterData
	//--------------------------------------------
   bool isRenderingReflections() { return object->renderReflection; };
   bool isHighResOnly() { return object->highResOnly; };
   bool isOrientedOnVelocity() { return object->orientOnVelocity; };
   bool useEmitterColors() { return object->useEmitterColors; };
   bool useEmitterSizes() { return object->useEmitterSizes; };
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
   SimObject* getPointer() { return object; };
};

#endif //_H_MESH_EMITTER