//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas J�rgensen, 2012
//-----------------------------------------------------------------------------

// Load up all scripts.  This function is called when
// a server is constructed.
exec("./spell.cs");
//Load all utility scripts.
for(%utilFile = findFirstFile("./utility/*.cs"); %utilFile !$= ""; %utilFile = findNextFile())
{
   exec(%utilFile);
}