//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

// Load up all datablocks.  This function is called when
// a server is constructed.

// Do the various effects next -- later scripts/datablocks may need them
exec("./particles.cs");
exec("./genericparticles.cs");

exec("./../templates/defaultDatablocks.cs");
exec("./../templates/graphExamples.cs");