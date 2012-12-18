//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

// Editmygame.com
// Copyright (c) Laurentiu Danila
// GENERIC PARTICLES

// MEDIUM SMOKE

datablock ParticleData(MediumSmoke)
{
   textureName          = "art/shapes/particles/smoke";
   dragCoefficient     = 0.3;
   gravityCoefficient   = -0.2;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = true;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   //colors[0]     = "0.0 0.0 0.0 0.0";
   //colors[1]     = "0.2 0.2 0.2 0.1";
   //colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 1.5;
   sizes[1]      = 2.75;
   sizes[2]      = 6.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock SphereEmitterData(MediumSmokeEmitter)
{
   ejectionPeriodMS = 400;
   periodVarianceMS = 5;

   ejectionVelocity = 0.0;
   velocityVariance = 0.0;

   thetaMin         = 0.0;
   thetaMax         = 90.0;

   particles = MediumSmoke;
};

datablock SphereEmitterNodeData(MediumSmokeEmitterNode)
{
   timeMultiple = 1;
};

// BIG SMOKE

datablock ParticleData(BigSmoke)
{
   textureName          = "art/shapes/particles/smoke";
   dragCoefficient     = 0.2;
   gravityCoefficient   = -0.1;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = true;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   //colors[0]     = "0.0 0.0 0.0 0.0";
   //colors[1]     = "0.2 0.2 0.2 0.1";
   //colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 4.5;
   sizes[1]      = 8.75;
   sizes[2]      = 20.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock SphereEmitterData(BigSmokeEmitter)
{
   ejectionPeriodMS = 120;
   periodVarianceMS = 5;

   ejectionVelocity = 0.2;
   velocityVariance = 0.0;

   thetaMin         = 0.0;
   thetaMax         = 160.0;

   particles = BigSmoke;
};

datablock SphereEmitterNodeData(BigSmokeEmitterNode)
{
   timeMultiple = 1;
};

//GENERIC FIRE

datablock ParticleData(GenericFire)
{
   textureName          = "art/shapes/particles/smoke";
   dragCoefficient      = 0.0;
   windCoefficient      = 0.0;
   gravityCoefficient   = -0.05;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 5000;
   lifetimeVarianceMS   = 1000;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;
   spinSpeed = 1.0;

   colors[0]     = "0.2 0.2 0.0 0.2";
   colors[1]     = "0.6 0.2 0.0 0.2";
   colors[2]     = "0.4 0.0 0.0 0.1";
   colors[3]     = "0.1 0.04 0.0 0.3";

   sizes[0]      = 0.5;
   sizes[1]      = 2.0;
   sizes[2]      = 3.0;
   sizes[3]      = 4.0;

   times[0]      = 0.0;
   times[1]      = 0.1;
   times[2]      = 0.2;
   times[3]      = 0.3;
};

datablock ParticleData(GenericSmoke)
{
   textureName          = "art/shapes/particles/smoke";
   dragCoefficient     = 0.3;
   gravityCoefficient   = -0.5;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = true;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   //colors[0]     = "0.0 0.0 0.0 0.0";
   //colors[1]     = "0.2 0.2 0.2 0.1";
   //colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 1.5;
   sizes[1]      = 2.75;
   sizes[2]      = 6.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock SphereEmitterData(GenericFireEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;

   ejectionVelocity = 0.55;
   velocityVariance = 0.00;
   ejectionOffset = 1.0;

   thetaMin         = 1.0;
   thetaMax         = 100.0;

   particles = "GenericFire";
};

datablock SphereEmitterData(GenericSmokeEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;

   ejectionVelocity = 0.55;
   velocityVariance = 0.00;
   ejectionOffset = 1.0;

   thetaMin         = 1.0;
   thetaMax         = 100.0;

   particles = "GenericSmoke";
};

datablock SphereEmitterNodeData(GenericFireEmitterNode)
{
   timeMultiple = 1;
};

datablock SphereEmitterNodeData(GenericSmokeEmitterNode)
{
   timeMultiple = 1;
};

//DUST PARTICLES

datablock ParticleData(DustParticle)
{
   textureName          = "art/shapes/particles/firefly";
   dragCoefficient      = 0.0;
   windCoefficient      = 5.0;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 8000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "0 0 0 0 ";
   colors[1]     = "1 0 0 1";
   colors[2]     = "1 1 0 1";
   colors[3]     = "0 0 0 0";

   sizes[0]      = 0.0;
   sizes[1]      = 0.15;
   sizes[2]      = 0.2;
   sizes[3]      = 0.0;

   times[0]      = 0.0;
   times[1]      = 0.1;
   times[2]      = 0.5;
   times[3]      = 1.0;

};

datablock SphereEmitterData(DustEmitter)
{
   ejectionPeriodMS = 300;
   periodVarianceMS = 0;

   ejectionVelocity = 3;
   velocityVariance = 1.00;
   ejectionOffset = 1.0;

   thetaMin         = 75.0;
   thetaMax         = 90.0;

   phiReferenceVel  = 360.00;
   phiVariance      = 360.00;

   particles = "DustParticle";
};

datablock SphereEmitterNodeData(DustNode)
{
   timeMultiple = 1;
};

//EMBER AROUND...

datablock ParticleData(EmberParticle)
{
   textureName          = "art/shapes/particles/ember";
   dragCoefficient      = 0.0;
   windCoefficient      = 0.0;
   gravityCoefficient   = -0.05;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 5000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "1.000000 0.800000 0.000000 0.800000";
   colors[1]     = "1.000000 0.700000 0.000000 0.800000";
   colors[2]     = "1.000000 0.000000 0.000000 0.200000";

   sizes[0]      = 0.05;
   sizes[1]      = 0.1;
   sizes[2]      = 0.05;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock SphereEmitterData(EmberEmitter)
{
   ejectionPeriodMS = 100;
   periodVarianceMS = 0;

   ejectionVelocity = 0.75;
   velocityVariance = 0.00;
   ejectionOffset = 2.0;

   thetaMin         = 1.0;
   thetaMax         = 100.0;

   particles = "EmberParticle";
};

datablock SphereEmitterNodeData(EmberNode)
{
   timeMultiple = 1;
};

// LargeFire

datablock ParticleData(LargeFire)
{
   textureName          = "art/shapes/particles/smoke";
   dragCoefficient      = 0.0;
   windCoefficient      = 0.0;
   gravityCoefficient   = -0.05;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 7000;
   lifetimeVarianceMS   = 1000;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;
   spinSpeed = 1.0;

   colors[0]     = "0.2 0.2 0.0 0.2";
   colors[1]     = "0.6 0.2 0.0 0.2";
   colors[2]     = "0.4 0.0 0.0 0.1";
   colors[3]     = "0.1 0.04 0.0 0.3";

   sizes[0]      = 1.5;
   sizes[1]      = 1.0;
   sizes[2]      = 5.0;
   sizes[3]      = 6.0;

   times[0]      = 0.0;
   times[1]      = 0.1;
   times[2]      = 0.2;
   times[3]      = 0.3;
};

datablock ParticleData(LargeSmoke)
{
   textureName          = "art/shapes/particles/smoke";
   dragCoefficient     = 0.3;
   gravityCoefficient   = -0.5;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 5000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = true;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.0 0.0 0.0 0.0";
   colors[1]     = "0.2 0.2 0.2 0.1";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 7.5;
   sizes[1]      = 12.75;
   sizes[2]      = 19.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock SphereEmitterData(LargeFireEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;

   ejectionVelocity = 0.55;
   velocityVariance = 0.00;
   ejectionOffset = 2.0;
   
   //highResOnly = false;

   thetaMin         = 1.0;
   thetaMax         = 120.0;

   particles = "LargeFire";
};

datablock SphereEmitterData(LargeSmokeEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;

   ejectionVelocity = 0.55;
   velocityVariance = 0.00;
   ejectionOffset = 2.0;
   
   //highResOnly = false;

   thetaMin         = 1.0;
   thetaMax         = 120.0;

   particles = "LargeSmoke";
};

datablock SphereEmitterNodeData(LargeFireEmitterNode)
{
   timeMultiple = 1;
};

datablock SphereEmitterNodeData(LargeSmokeEmitterNode)
{
   timeMultiple = 1;
};

// Steam effects
datablock ParticleData(SlowSteamParticle)
{
   textureName          = "art/shapes/particles/steam";
   dragCoefficient      = 0.3;
   gravityCoefficient   = -0.1;   // rises slowly
   inheritedVelFactor   = 0.00;
   windCoefficient      = 0;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = true;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "1 1 1 0.0";
   colors[1]     = "1 1 1 0.1";
   colors[2]     = "1 1 1 0.0";

   sizes[0]      = 0.25;
   sizes[1]      = 1.5;
   sizes[2]      = 2.0;

   times[0]      = 0.0;
   times[1]      = 0.3;
   times[2]      = 1.0;
};

datablock SphereEmitterData(SlowSteamEmitter)
{
   ejectionPeriodMS = 100;
   periodVarianceMS = 25;

   ejectionVelocity = 0.0;
   velocityVariance = 0.0;

   thetaMin         = 0.0;
   thetaMax         = 90.0;  

   particles = SlowSteamParticle;
};

// Steam effects
datablock ParticleData(PressuredSteamParticle)
{
   textureName          = "art/shapes/particles/steam";
   dragCoefficient      = 2.0;
   gravityCoefficient   = -0.15;   // rises slowly
   inheritedVelFactor   = 0.00;
   windCoefficient	= 0;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 1050;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;
   useinvalpha = 1;

   colors[0]     = "1 1 1 0.0";
   colors[1]     = "1 1 1 0.3";
   colors[2]     = "1 1 1 0.2";
   colors[3]     = "1 1 1 0.0";

   sizes[0]      = 0.15;
   sizes[1]      = 0.2;
   sizes[2]      = 0.75;
   sizes[3]      = 1.5;

   times[0]      = 0.0;
   times[1]      = 0.1;
   times[2]      = 0.3;
   times[3]      = 1.0;
};

datablock SphereEmitterData(PressuredSteamEmitter)
{
   ejectionPeriodMS = 20;
   periodVarianceMS = 0;

   ejectionVelocity = 5.0;
   velocityVariance = 1.0;

   thetaMin         = 0.0;
   thetaMax         = 1.0;  

   particles = PressuredSteamParticle;
};

// Water effects
datablock ParticleData(WaterSplatterParticle)
{
   textureName          = "art/shapes/particles/droplet";
   dragCoefficient      = 0.1;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   windCoefficient	= 0;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 1050;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;
   useinvalpha = 1;

   colors[0]     = "0.9 1 0.9 0.0";
   colors[1]     = "0.9 1 0.9 0.2";
   colors[2]     = "0.9 1 0.9 0.0";

   sizes[0]      = 0.15;
   sizes[1]      = 0.2;
   sizes[2]      = 0.2;

   times[0]      = 0.0;
   times[1]      = 0.3;
   times[2]      = 1.0;
};

datablock SphereEmitterData(WaterSplatterEmitter)
{
   ejectionPeriodMS = 50;
   periodVarianceMS = 0;

   ejectionVelocity = 1.5;
   velocityVariance = 0.5;

   phiReferenceVel  = 0;
   phiVariance      = 360;
   thetaMin         = 0;
   thetaMax         = 180;
   orientparticles  = 1;

   particles = WaterSplatterParticle;
};

datablock ParticleData(WaterDripParticle)
{
   textureName          = "art/shapes/particles/waterDrip";
   dragCoefficient      = 0.0;
   gravityCoefficient   = 0.02;   // rises slowly
   inheritedVelFactor   = 0.00;
   windCoefficient	= 0;
   lifetimeMS           = 8000;
   lifetimeVarianceMS   = 0;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;
   useinvalpha = 1;

   colors[0]     = "0.9 1 0.9 0.1";
   colors[1]     = "0.9 1 0.9 0.5";
   colors[2]     = "0.9 1 0.9 0.0";

   sizes[0]      = 0.3;
   sizes[1]      = 0.35;
   sizes[2]      = 0.5;

   times[0]      = 0.0;
   times[1]      = 0.3;
   times[2]      = 1.0;
};

datablock SphereEmitterData(WaterDripEmitter)
{
   ejectionPeriodMS = 30;
   periodVarianceMS = 10;

   ejectionVelocity = 5.0;
   velocityVariance = 1.0;
   orientparticles  = 1;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   thetaMin         = 178;
   thetaMax         = 180;

   particles = WaterDripParticle;
};


// Desert Dust Particle Datablocks

datablock SphereEmitterNodeData(DesertDustSphereEmitterNodeData)
{
   timeMultiple = 1;
};

datablock ParticleData(DesertDustParticleData)
{
   textureName          = "art/shapes/particles/smoke.png";
   dragCoefficient     = 0;
   gravityCoefficient   = 0;  
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 5000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = 0;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.889764 0.818898 0.582677 0";
   colors[1]     = "0.724409 0.685039 0.480315 0.708661";
   colors[2]     = "0.944882 0.84252 0.551181 0";

   sizes[0]      = 7.49863;
   sizes[1]      = 12.748;
   sizes[2]      = 19.4989;

   times[0]      = 0.0;
   times[1]      = 0.494118;
   times[2]      = 1.0;
   animTexName = "art/shapes/particles/smoke.png";
   colors[3] = "0.996078 0.00784314 0.0313726 1";
   sizes[3] = "0";
   originalName = "DesertDustParticleData";
};

datablock ParticleData(DesertDustDescendingParticleData)
{
   textureName          = "art/shapes/particles/smoke.png";
   dragCoefficient     = 0;
   gravityCoefficient   = 0.0952381;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 5000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = 0;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.889764 0.818898 0.590551 0";
   colors[1]     = "0.733333 0.698039 0.486275 0.724409";
   colors[2]     = "0.944882 0.84252 0.559055 0";

   sizes[0]      = 7.49863;
   sizes[1]      = 12.748;
   sizes[2]      = 19.4989;

   times[0]      = 0.0;
   times[1]      = 0.494118;
   times[2]      = 1.0;
   animTexName = "art/shapes/particles/smoke.png";
   colors[3] = "0.996078 0.00784314 0.0313726 1";
   sizes[3] = "0";
   constantAcceleration = "-1";
};

datablock SphereEmitterData(DesertDustSphereEmitterData)
{
   ejectionPeriodMS = 625;
   periodVarianceMS = 10;

   ejectionVelocity = 1;
   velocityVariance = 0;
   orientparticles  = 0;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   thetaMin         = 0;
   thetaMax         = 39.75;

   particles = DesertDustParticleData;
   blendStyle = "NORMAL";
   ejectionOffset = "10";
   softParticles = "1";
};

datablock SphereEmitterData(DesertDustWideSphereEmitterData)
{
   ejectionPeriodMS = 625;
   periodVarianceMS = 10;

   ejectionVelocity = 1;
   velocityVariance = 0;
   orientparticles  = 0;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   thetaMin         = 0;
   thetaMax         = 39;

   particles = DesertDustParticleData;
   blendStyle = "NORMAL";
   ejectionOffset = "40";
   softParticles = "1";
   originalName = "DesertDustWideSphereEmitterData";
};

datablock SphereEmitterData(DesertDustDescendingSphereEmitterData)
{
   ejectionPeriodMS = 500;
   periodVarianceMS = 10;

   ejectionVelocity = 1;
   velocityVariance = 0;
   orientparticles  = 0;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   thetaMin         = 0;
   thetaMax         = 39;

   particles = DesertDustDescendingParticleData;
   blendStyle = "NORMAL";
   ejectionOffset = "10";
   softParticles = "1";
};
