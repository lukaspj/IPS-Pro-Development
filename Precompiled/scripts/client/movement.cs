//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// This script provides functions for pausing and continuing movement.
// We are using it for making the player stand still while casting spells.

// Pause all movement
function mvPause()
{
   $mvUpAction = 0;
   $mvDownAction = 0;
   $mvLeftAction = 0;
   $mvRightAction = 0;
   $mvForwardAction = 0;
   $mvBackwardAction = 0;
   $moving = 0;
}

// Continue all movement
function mvContinue(%speed)
{
   $mvUpAction = $oldUpAction;
   $mvDownAction = $oldDownAction;
   $mvLeftAction = $oldLeftAction;
   $mvRightAction = $oldRightAction;
   $mvForwardAction = $oldForwardAction;
   $mvBackwardAction = $oldBackwardAction;
   
   $moving = 1;
}