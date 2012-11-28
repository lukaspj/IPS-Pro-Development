datablock ParticleData(ProjectileParticles)
{
   textureName          = "art/shapes/particles/spark";
   dragCoeffiecient     = 0.0;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 100;
   lifetimeVarianceMS   = 25;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;
   thetaMax = "180";
   

   colors[0]     = "1 0.0 0.0 1.0";
   colors[1]     = "1 0.2 0.2 1.0";
   colors[2]     = "1 0.3 0.3 0";

   sizes[0]      = 0.25;
   sizes[1]      = 0.35;
   sizes[2]      = 1.0;

   times[0]      = 0.0;
   times[1]      = 0.3;
   times[2]      = 1.0;
};
datablock ParticleEmitterData(ProjectileEmitter)
{
   ejectionPeriodMS = 1;
  // periodVarianceMS = 1;
   period = 0;
   ejectionVelocity = 0.25;
   velocityVariance = 0.10;
   thetaMin = 0.0;
   thetaMax = 180;
   particles = ProjectileParticles;
   ejectionOffset   = 0.5;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvances = false;
   orientParticles  = true;
};
datablock ProjectileData(SampleSpellProjectile)
{
   projectileShapeName = "art/shapes/Projectiles/Projectile.dts";
   
   muzzleVelocity = 20;
   velInheritFactor = 0;
   armingDelay = 0;
   //Explosion = SpellExplosion;
   particleEmitter = ProjectileEmitter;
   lifetime = 10000;
   fadeDelay = 9000;
   isBallistic = false;
   bounceElasticity = 0;
   bounceFriction = 0;
   gravityMod = 0;
   hasLight = true;
   lightRadius = 3;
   lightColor = "0.8 0.8 1.0";
};

function SampleSpellProjectile::onCollision(%this,%col,%fade,%pos)
{
   echo("onCollision");
   //commandToServer('radiusDamage' , %obj , %pos , 2 , 20 , "Projectile" , 0);
   radiusDamage(%obj, %pos, 2, 20, "Projectile", 0);
}

//-----------------------------------------------------------------------------
// Projectile Explosion
/*
datablock AudioProfile(SpellExplosionSound)
{
   filename = "~/data/sound/Crossbow_explosion.wav";
   description = AudioDefault3d;
	preload = true;
};

datablock ParticleData(SpellExplosionParticle)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient      = 2;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.2;
   constantAcceleration = 0.0;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 150;

   colors[0]     = "0.56 0.36 0.26 1.0";
   colors[1]     = "0.56 0.36 0.26 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 1.0;
};

datablock ParticleEmitterData(SpellExplosionEmitter)
{
   ejectionPeriodMS = 7;
   periodVarianceMS = 0;
   ejectionVelocity = 2;
   velocityVariance = 1.0;
   ejectionOffset   = 0.0;
   thetaMin         = 0;
   thetaMax         = 60;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvances = false;
   particles = "SpellExplosionParticle";
};

datablock ParticleData(SpellExplosionSmoke)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoeffiecient     = 100.0;
   gravityCoefficient   = 0;
   inheritedVelFactor   = 0.25;
   constantAcceleration = -0.80;
   lifetimeMS           = 1200;
   lifetimeVarianceMS   = 300;
   useInvAlpha =  true;
   spinRandomMin = -80.0;
   spinRandomMax =  80.0;

   colors[0]     = "0.56 0.36 0.26 1.0";
   colors[1]     = "0.2 0.2 0.2 1.0";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 1.0;
   sizes[1]      = 1.5;
   sizes[2]      = 2.0;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;

};

datablock ParticleEmitterData(SpellExplosionSmokeEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 4;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles = "SpellExplosionSmoke";
};

datablock ParticleData(SpellExplosionSparks)
{
   textureName          = "~/data/shapes/particles/spark";
   dragCoefficient      = 1;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.2;
   constantAcceleration = 0.0;
   lifetimeMS           = 500;
   lifetimeVarianceMS   = 350;

   colors[0]     = "0.60 0.40 0.30 1.0";
   colors[1]     = "0.60 0.40 0.30 1.0";
   colors[2]     = "1.0 0.40 0.30 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 0.25;
   sizes[2]      = 0.25;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(SpellExplosionSparkEmitter)
{
   ejectionPeriodMS = 3;
   periodVarianceMS = 0;
   ejectionVelocity = 13;
   velocityVariance = 6.75;
   ejectionOffset   = 0.0;
   thetaMin         = 0;
   thetaMax         = 180;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvances = false;
   orientParticles  = true;
   lifetimeMS       = 100;
   particles = "SpellExplosionSparks";
};

datablock ExplosionData(SpellSubExplosion1)
{
   offset = 1.0;
   emitter[0] = SpellExplosionSmokeEmitter;
   emitter[1] = SpellExplosionSparkEmitter;
};

datablock ExplosionData(SpellSubExplosion2)
{
   offset = 1.0;
   emitter[0] = SpellExplosionSmokeEmitter;
   emitter[1] = SpellExplosionSparkEmitter;
};

datablock ExplosionData(SpellExplosion)
{
   soundProfile   = SpellExplosionSound;
   lifeTimeMS = 1200;

   // Volume particles
   particleEmitter = SpellExplosionEmitter;
   particleDensity = 80;
   particleRadius = 1;

   // Point emission
   emitter[0] = SpellExplosionSmokeEmitter;
   emitter[1] = SpellExplosionSparkEmitter;

   // Sub explosion objects
   subExplosion[0] = SpellSubExplosion1;
   subExplosion[1] = SpellSubExplosion2;

   // Camera Shaking
   shakeCamera = true;
   camShakeFreq = "10.0 11.0 10.0";
   camShakeAmp = "1.0 1.0 1.0";
   camShakeDuration = 1.5;
   camShakeRadius = 50.0;

   // Dynamic light
   lightStartRadius = 6;
   lightEndRadius = 3;
   lightStartColor = "0.5 0.5 0";
   lightEndColor = "0 0 0";
};*/