//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// This is a sample spell which summons a Rogue ( a mob ).
// This file shows how to setup a simple pointbased Summon spell using this resource

// ** This spell is NOT functioning Out of the box **
// This is an example of how i have set up my summon scripts.
// To make this spell actually work, you will need a modified version of 
//  the Improved AI Guard resource

// First instantiate the ScriptObject remember to set Spell as a superClass
// type defines what kind of spell it is
// raytype defines what rayresult we want when we use the CastAtScreenCenter method.
new ScriptObject( Rogue ){
      superClass = Spell;
      type = "summon";
      raytype = "point";
};
// Always remember to add the new spell ScriptObject to the spells SimGroup!
spells.add(Rogue);

// **Optional** a function which returns this ScriptObject
function getRogueScriptObject()
{
   return Rogue;
}

// The cast function, what happens when this spell is cast?
function Rogue::Cast(%this, %pos, %trans, %path, %team, %cast)
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
   if(%path $= ""){
      error("No path on Rogue::Cast");
   }
   if(%team $= ""){
      error("No team on Rogue::Cast");
      return false;
   }
   
   // Spawn the AIPlayer. This is a very long function which is not included in this resource, 
   //  if there is a demand i might create another resource on how to merge the two.
   return %player = AIPlayer::spawnAtPos("Foul evil man", %pos, %trans, %path, "charge", "melee", %team, false, rogueBlock);
}

// Define the player datablock
datablock PlayerData(rogueBlock : DemoPlayer)
{
   maxRange = 10;
};