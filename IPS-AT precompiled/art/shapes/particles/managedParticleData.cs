//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

// This is the default save location for any Particle datablocks created in the
// Particle Editor (this script is executed from onServerCreated())

datablock ParticleData(TeleporterFlash : DefaultParticle)
{
   dragCoefficient = "30";
   inheritedVelFactor = "0";
   constantAcceleration = "0";
   lifetimeMS = "500";
   spinRandomMin = "-90";
   spinRandomMax = "90";
   textureName = "art/shapes/particles/flare.png";
   animTexName = "art/shapes/particles/flare.png";
   colors[0] = "0.678431 0.686275 0.913726 0.207";
   colors[1] = "0 0.543307 1 0.759";
   colors[2] = "0.0472441 0.181102 0.92126 0.838";
   colors[3] = "0.141732 0.0393701 0.944882 0";
   sizes[0] = "0";
   sizes[1] = "0";
   sizes[2] = "4";
   sizes[3] = "0.1";
   times[1] = "0.166667";
   times[2] = "0.666667";
   lifetimeVarianceMS = "0";
   gravityCoefficient = "-9";
};
