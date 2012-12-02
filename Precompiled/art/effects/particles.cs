datablock ParticleData(pillarParticle : DefaultParticle)
{
   inheritedVelFactor = "0.951076";
   constantAcceleration = "1.667";
   useInvAlpha = "0";
   textureName = "art/shapes/particles/smoke.png";
   animTexName = "art/shapes/particles/smoke.png";
   colors[1] = "1 0.622047 0 0.629921";
   colors[2] = "1 0.393701 0 0.377953";
   colors[3] = "0 0 0 0";
};

//-----------------------------------------------------------------------------
// Spell bolt projectile particles

datablock ParticleData(SpellBoltParticle)
{
   textureName          = "art/shapes/particles/smoke";
   dragCoeffiecient     = 0.0;
   gravityCoefficient   = -0.2;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 500;  // lasts 0.7 second
   lifetimeVarianceMS   = 150;   // ...more or less
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.56 0.36 0.26 1.0";
   colors[1]     = "0.56 0.36 0.26 1.0";
   colors[2]     = "0 0 0 0";

   sizes[0]      = 0.25;
   sizes[1]      = 0.5;
   sizes[2]      = 1.0;

   times[0]      = 0.0;
   times[1]      = 0.3;
   times[2]      = 1.0;
};

datablock SphereEmitterData(SpellBoltEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 90.0;

   particles = SpellBoltParticle;
};

datablock SphereEmitterNodeData(SpellBoltNode)
{
   timeMultiple = 1;
};

//-----------------------------------------------------------------------------
// Spell bolt projectile particles

datablock ParticleData(coloredParticle)
{
   textureName          = "art/shapes/particles/smoke";
   dragCoeffiecient     = 0.0;
   gravityCoefficient   = -0.2;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 500;  // lasts 0.7 second
   lifetimeVarianceMS   = 150;   // ...more or less
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.56 0.36 0.26 1.0";
   colors[1]     = "0.56 0.36 0.26 1.0";
   colors[2]     = "0 0 0 0";

   sizes[0]      = 0.25;
   sizes[1]      = 0.5;
   sizes[2]      = 1.0;

   times[0]      = 0.0;
   times[1]      = 0.3;
   times[2]      = 1.0;
};

datablock SphereEmitterData(broadEmitter)
{
   ejectionPeriodMS = "1";
   periodVarianceMS = "0";

   ejectionVelocity = "0";
   velocityVariance = "0";

   thetaMin         = "90";
   thetaMax         = "90";

   particles = "pillarParticle";
   ejectionOffset = "2.5";
   lifetimeMS = "833";
   blendStyle = "NORMAL";
   softParticles = "0";
};

datablock SphereEmitterNodeData(broadNode)
{
   timeMultiple = 1;
};
