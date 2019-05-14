Texture2D txDiffuse : register(t0);
SamplerState Sampler : register(s0);

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 uv :	TEXCOORD;
	float4 Normal : NORMAL;

	float3 worldPos : WORLDPOSITION;
	float3 Kd : KDVALUES;
};

cbuffer GSConstantBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;

	float4x4 X_RotationMatrix;
	float4x4 Y_RotationMatrix;

	float4 Kd;
};

float4 PS_main(PSSceneIn input) : SV_Target
{
	float3 Light_Source = { 0.0f, 4.0f, -2.01f };
	float3 Pos_To_Light;
	float DotProd;

	float3 Color = txDiffuse.Sample(Sampler, input.uv).xyz;
	//float3 Color = { 1.0f, 1.0f, 1.0f };

	//Colour = Colour + input.Normal.xyz;

	//DOT PRODUCT CALCULATIONS
	Pos_To_Light = (Light_Source - input.worldPos.xyz);
	Pos_To_Light = normalize(Pos_To_Light);
	DotProd = dot(Pos_To_Light, input.Normal);
	Color = (Color * clamp(DotProd, 0.0f, 1.0f));

	////Adding ambient light (0.015f ~ 50/255 increased colour)
	Color.xyz += clamp((0.15f * (txDiffuse.Sample(Sampler, input.uv).xyz)), 0.0f, 1.0f);
	Color.xyz *= input.Kd.xyz;

	//return float4(Colour, 1.0f);
	return float4(Color, 1.0f);
};