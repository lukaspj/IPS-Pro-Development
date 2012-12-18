//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// This is a sample spell which throws a fireball.
// This file shows how to setup a simple projectile based spell using this resource

// First instantiate the ScriptObject remember to set Spell as a superClass
// type defines what kind of spell it is
// raytype defines what rayresult we want when we use the CastAtScreenCenter method.
new ScriptObject( Fireball ){
      superClass = Spell;
      type = "projectile";
      raytype = "point";
};
// Always remember to add the new spell ScriptObject to the spells SimGroup!
spells.add(Fireball);

// **Optional** a function which returns this ScriptObject
function getFireballScriptObject()
{
   return Fireball;
}

// The cast function, what happens when this spell is cast?
function Fireball::Cast(%this, %pos, %trans, %path, %team, %client)
{
   // First if we miss any of the needed variables, message the error to the console
   //  and return with an error.
   if(%pos $= ""){
      error("No pos on Fireball::Cast");
      return false;
   }
   if(%trans $= ""){
      error("No trans on Fireball::Cast");
      return false;
   }
   
   // Start the code which summons the fireball!
   %clientplayer = %client.getControlObject();
   
   // Get a vector defining where the camera is pointing
   %eye = %clientplayer.getEyeVector();
   // Get the projectile datablock we will use when instantiating the projectile
   %projectile = SampleSpellProjectile;
   // Set the muzzleVector, which is the vector which defines the velocity and 
   //  direction of our fireball
   %muzzleVector = VectorScale(%eye, %projectile.muzzleVelocity);
   // Instantiate the fireball
   %bullet = new Projectile() {
         dataBlock = SampleSpellProjectile;
         initialVelocity = %muzzleVector;
         initialPosition  = %clientplayer.getWorldBoxCenter();
         sourceObject     = %clientplayer;
         sourceSlot       = 0;
         client           = %client;
   };
   MissionCleanup.add(%bullet);
   // Tell the client to lock for 800 ms and cast spellanimation 2
   commandToClient(%client, 'BeginCast', 800, 2);
   // Return with the instantiated object
   return %bullet;
}

// **Optional**
// Datablock instancing! This could also be instantiated outside of this scriptfile
datablock ParticleData(ProjectileParticles)
{
   textureName          = "art/shapes/particles/spark";
   dragCoeffiecient     = 0.0;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 300;
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

datablock GraphEmitterData(ProjectileEmitter)
{
   ejectionPeriodMS = 1;
  // periodVarianceMS = 1;
   ejectionVelocity = 0.25;
   velocityVariance = 0.10;
   particles = ProjectileParticles;
   ejectionOffset   = 1;
   overrideAdvances = false;
   //yFunc = "cos(t/50)";
   xFunc = "cos(t/50)";
   zFunc = "sin(t/50)";
   Loop = false;
};
datablock ProjectileData(SampleSpellProjectile)
{
   projectileShapeName = "art/shapes/Projectiles/Projectile.dts";
   
   muzzleVelocity = 10;
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

function SampleSpellProjectile::onCollision(%this, %obj, %col, %fade, %pos)
{
   radiusDamage(%col, %pos, 2, 20, "Projectile", 0);
}
