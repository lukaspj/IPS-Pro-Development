float4x4 WVP;

texture	textureMap : DiffuseMap; // Particle texture
sampler	textureSampler=
	sampler_state
	{
		Texture = <textureMap>;
		AddressU  = CLAMP;        
		AddressV  = CLAMP;
		MIPFILTER = LINEAR;
		MINFILTER = LINEAR;
		MAGFILTER = LINEAR;
	};

struct PS_INPUT
{
	float2 textureCoordinate	: TEXCOORD0;
	float4 Color				: COLOR0;
};

float4 main(PS_INPUT input) : COLOR
{
	float2 textureCoordinate = input.textureCoordinate.xy;
	float4 col = tex2D(textureSample, textureCoordinate) * input.Color;
	return col;
}