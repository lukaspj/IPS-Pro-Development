//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// This is a sample spell which summons a Pillar of Flame.
// This file shows how to setup a simple pointbased AoE spell using this resource

// First instantiate the ScriptObject remember to set Spell as a superClass
// type defines what kind of spell it is
// raytype defines what rayresult we want when we use the CastAtScreenCenter method.
new ScriptObject( Flamepillar ){
      superClass = Spell;
      type = "AOE";
      raytype = "point";
};
// Always remember to add the new spell ScriptObject to the spells SimGroup!
spells.add(Flamepillar);

// **Optional** a function which returns this ScriptObject
function getFlamepillarScriptObject()
{
   return Flamepillar;
}

// The cast function, what happens when this spell is cast?
function Flamepillar::Cast(%this, %pos, %trans, %path, %team, %client)
{
   // First if we miss any of the needed variables, message the error to the console
   //  and return with an error.
   if(%pos $= ""){
      error("No pos on Rogue::Cast");
      return false;
   }
   if(%trans $= ""){
      error("No trans on Rogue::Cast");
      return false;
   }
   
   // Instantiate the SphereEmitterNode which shows the almighty Pillar of Flame
   %flame = new SphereEmitterNode(){
      position = %pos;
      rotation = "1 0 0 0";
      scale = "1 1 1 1";
      dataBlock = broadNode;
      emitter = broadEmitter;
      velocity = 1;
   };
   // Dish out some damage when the Pillar of Flame is spawned
   radiusDamage(%col, %pos, 2.5, 20, "Projectile", 0);
   // Tell the client to lock for 2000 ms and cast spellanimation 1
   commandToClient(%client, 'BeginCast', 2000, 1);
   // Return with the instantiated object
   return %flame;
}
// Here we have instantiated the datablocks in an external script!