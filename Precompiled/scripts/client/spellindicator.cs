//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// This script will spawn a decal on the ground where we are looking, this is
//  used to create an AoE indicator sort of like if you were going to cast a 
//  blizzard in WoW, it creates a ring which shows where it is going to land.

// Spawn the Indicator Decal.
function spawnIndicatorDecal()
{
   // Don't spawn an indicator decal if we already have spawned one.
   if($SpellDecal $= "")
   {
      
      %player = ServerConnection.getControlObject();
      %pos = castClientRayFromOBJ(%player, "point", "statics");
      %norm = castClientRayFromOBJ(%player, "normal", "statics");
      if(%pos !$= false && %norm !$= false)
         $SpellDecal = decalManagerAddDecal( %pos, %norm, 0, 1, "NewDecalData", true );
      schedule(10, 0, updateSpellDecal);
   }
}

// Update the decals location
function updateSpellDecal()
{
   if($SpellDecal !$= "")
   {
      %player = ServerConnection.getControlObject();
      decalManagerRemoveDecal( $SpellDecal );
      %pos = castClientRayFromOBJ(%player, "point", "statics");
      %norm = castClientRayFromOBJ(%player, "normal", "statics");
      if(%pos !$= false && %norm !$= false)
      {
         // This code i commented out here, is an alternative code which is more efficient
         //  but unfortunately demands that you edit in the source code.
         // I will post more information about this code in another resource.
         /*%result = decalManagerMoveDecal( $SpellDecal, %pos, %norm );
         if(!%result) { $SpellDecal = decalManagerAddDecal( %pos, %norm, 0, 1, "NewDecalData", true ); }*/
         
         $SpellDecal = decalManagerAddDecal( %pos, %norm, 0, 1, "NewDecalData", true );
      }
      else
         echo("Out of range!");
      // Update this decal at 50 times per second.
      schedule(20, 0, updateSpellDecal);
   }
}