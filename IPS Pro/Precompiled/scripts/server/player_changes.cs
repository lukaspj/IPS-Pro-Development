/-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas J�rgensen, 2012
/-----------------------------------------------------------------------------

// Add this anywhere in player.cs
function Player::playSpellAnimation(%this, %anim)
{
   if (%this.getState() !$= "Dead")
      %this.setActionThread("spell_"@%anim);
}