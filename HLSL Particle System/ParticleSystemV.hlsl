uniform float4x4 WVP;
uniform float sizeModifier = 3.5f;

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

struct Vertex
{
   float4 pos : POSITION;
   float4 color : COLOR0;
};

struct VS_OUTPUT {
	float4	position	: POSITION;
	float4	color		: COLOR0;
	float	Size		: PSIZE0;
};

VS_OUTPUT main(Vertex In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4	realPosition = tex2Dlod ( positionSampler, float4(In.pos.x, In.pos.y, 0, 0 ) );
	Out.color = In.color;
	realPosition.w = 1;
	Out.position = mul(realPosition, WVP);
	return 
}