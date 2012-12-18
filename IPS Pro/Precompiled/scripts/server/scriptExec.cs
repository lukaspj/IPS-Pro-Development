//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// Load up all scripts.  This function is called when
// a server is constructed.
exec("./spell.cs");
//Load all utility scripts.
for(%utilFile = findFirstFile("./utility/*.cs"); %utilFile !$= ""; %utilFile = findNextFile())
{
   exec(%utilFile);
}