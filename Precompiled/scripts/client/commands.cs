//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// Begin casting a spell when the server tells us to!
function clientCmdBeginCast(%timer, %anim)
{
   // If no time is set, set it to a default of 2 seconds.
   if(%timer $= "")
      %timer = 2000;
   // Pause all the clients movement
   mvPause();
   // Continue all movement after the set amount of time
   schedule(%timer, 0, "mvContinue", 1);
   // Command the server to start casting the set spell animation.
   commandToServer('SpellAnimation', %anim);
}