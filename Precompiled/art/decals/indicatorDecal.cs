//-----------------------------------------------------------------------------
// Simple Spellcasting resource
// Copyright Lukas Jørgensen, 2012
//-----------------------------------------------------------------------------

// Just a sample decal which indicates where an AoE spell will land.
datablock DecalData(NewDecalData)
{
   Material = "newMaterial";
   textureCoordCount = "0";
};

singleton Material(newMaterial)
{
   mapTo = "unmapped_mat";
   diffuseMap[0] = "art/decals/target.png";
   specularPower[0] = "1";
   glow[0] = "0";
   scrollDir[0] = "-1 -1";
   scrollSpeed[0] = "1.412";
   rotSpeed[0] = "-0.35";
   rotPivotOffset[0] = "-0.5 -0.5";
   waveFreq[0] = "4.219";
   waveAmp[0] = "0.203";
   translucent = "1";
};