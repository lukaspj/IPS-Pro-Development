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

texture	positionMap;
sampler	positionSampler = 
	sampler_state
	{
		Texture   = <positionMap>;
		MipFilter = None;
		MinFilter = Point;
		MagFilter = Point;
		AddressU  = Clamp;
		AddressV  = Clamp;
	};

struct VS_INPUT {
	float4	vertexData	: POSITION;
	float4	color		: COLOR0;
};

struct VS_OUTPUT {
	float4	position	: POSITION;
	float4	color		: COLOR0;
	float	Size		: PSIZE0;
};

struct PS_INPUT
{
	float2 textureCoordinate	: TEXCOORD0;
	float4 Color				: COLOR0;
};

float screenHeight = 720;

VS_OUTPUT Transform(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4	realPosition = tex2Dlod ( positionSample, float4(In.vertexData.x, In.vertexData.y, 0, 0 ) );
	Out.color = In.color;
	realPosition.w = 1;
	Out.position = mul(realPosition, WVP);
	return Out;
}

float4 ApplyTexture(PS_INPUT input) : COLOR
{
	float2 textureCoordinate = input.textureCoordinate.xy;
	float4 col = tex2D(textureSample, textureCoordinate) * input.Color;
	return col;
}

technique TransformAndTexture
{
	pass P0
	{
		vertexShader = compile vs_3_0 Transform();
		pixelShader = compile vs_3_0 ApplyTexture();
	}
}