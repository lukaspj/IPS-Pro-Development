//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

#include "./postFx.hlsl"
#include "../torque.hlsl"

uniform float damageFlash;
uniform float whiteOut;
uniform sampler2D backBuffer : register(S0);

float4 main(PFXVertToPix IN) : COLOR0
{
 float4 color1 = tex2D(backBuffer, IN.uv0); 
 float4 color2 = color1 * MUL_COLOR;
 float4 damage = lerp(color1,color2,damageFlash);
 return lerp(damage,WHITE_COLOR,whiteOut);
}
