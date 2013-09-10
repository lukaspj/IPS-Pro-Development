//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif
#ifndef _GFXTEXTUREHANDLE_H_
#include "gfx/gfxTextureHandle.h"
#endif

#define MaxParticleSize 50.0

class ParticleEmitterData;
class ParticleManager;

//*****************************************************************************
// Particle Data
//*****************************************************************************
class ParticleData : public SimDataBlock
{
   typedef SimDataBlock Parent;

public:
   enum PDConst
   {
      PDC_NUM_KEYS = 4,
      PDC_NUM_BITS = 3,
   };
   enum ParticleVectorPreloadFlags
   {
      ParticleTextures = BIT(0),
      InvAlpha = BIT(1),
   };

   Point3F relPos;

   F32   dragCoefficient;
   F32   windCoefficient;
   F32   gravityCoefficient;

   F32   inheritedVelFactor;
   F32   constantAcceleration;

   S32   lifetimeMS;
   S32   lifetimeVarianceMS;
   
   F32    times[ PDC_NUM_KEYS ];
   F32    sizes[ PDC_NUM_KEYS ];

public:
   ParticleData();
   ~ParticleData();

   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
   virtual bool onAdd();
   virtual bool preload(bool server, String &errorStr);
   static void  initPersistFields();
   virtual void preloadVector(Vector<ParticleData*> particleDataBlocks, U32 mask, ParticleEmitterData* emitterData) = 0;
   
   static bool protectedSetSizes( void *object, const char *index, const char *data );

   bool reload(char errorBuffer[256]);

   virtual ParticleManager* CreateManager() = 0;
};

//*****************************************************************************
// Particle
// 
// This structure should be as small as possible.
//*****************************************************************************
struct Particle
{
   Point3F        vel;     // Velocity
   Point3F        acc;     // Constant acceleration
   F32            size;
   Point3F        orientDir;  // direction particle should go if using oriented particles
   U32            totalLifetime;   // Total ms that this instance should be "live"
   U32            currentAge;
   ParticleData*  dataBlock;       // datablock that contains global parameters for
                                  //  this instance
   Particle*      next;
   Point3F        pos;     // current instantaneous position
   Point3F        relPos;
};


#endif // _PARTICLE_H_