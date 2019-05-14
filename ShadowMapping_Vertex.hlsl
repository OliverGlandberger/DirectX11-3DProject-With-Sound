cbuffer GSConstantBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;

	float4 Kd;
};


cbuffer SMConstantBuffer : register(b1)
{
	float4x4 ShadowMapLightView;
	float4x4 ShadowMapLightProjection;
}

struct VS_IN
{
	float3 Position : POSITION;
	float2 TexUV	: TEXCOORD;
	float3 Normal	: NORMAL;
};

struct PS_IN
{
	float4 Position	: SV_POSITION;
	float2 TexUV : TEXCOORD;
};

PS_IN SHADOW_VS_main(VS_IN input)
{
	PS_IN output = (PS_IN)0;
	output.Position = float4(input.Position, 1);

	output.Position = mul(world, output.Position);
	output.Position = mul(ShadowMapLightView, output.Position);
	output.Position = mul(ShadowMapLightProjection, output.Position);

	output.TexUV = input.TexUV;
	return output;
}