//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Misc. server commands avialable to clients
//-----------------------------------------------------------------------------

// Cast a spell in center of the client's screen
function serverCmdCastSpell(%client, %spell)
{
   // Retrieve the ScriptObject which matches the spell name.
   %scrObj = getSpellFromName(%spell);
   // Cast the spell at the center of the client's screen.
   %scrObj.CastAtScreenCenter(%client);
}

// Same as above, simply spawns a mob instead of casting a spell.
function serverCmdCastMobSpell(%client, %spell)
{
   %scrObj = getSpellFromName(%spell);
   %scrObj.SpawnMobAtScreenCenter(%client);
}

// Begin playing an animation
function serverCmdSpellAnimation(%client, %anim)
{
<<<<<<< HEAD
   %client.getControlObject().playSpellAnimation(%anim);
=======
	if(isobject(%client.player)
		%client.player.playSpellAnimation(%anim);
>>>>>>> 27f31352860d97b5280eed398be6f4cf7f051ebb
}

function serverCmdPlayerID(%client)
{
   commandToClient(%client, 'PlayerID_Ret', %client.player);
}