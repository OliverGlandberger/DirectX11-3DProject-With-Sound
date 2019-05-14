struct VS_IN
{
	float3 Pos : POSITION;
	float2 uv :	TEXCOORD;
	float3 Normal : NORMAL;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 uv :	TEXCOORD;
	float4 Normal : NORMAL;
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

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = float4(input.Pos, 1);
	output.uv = input.uv;
	output.Normal = float4(input.Normal, 0);
	//output.Pos.xyz += input.Normal.xyz;

	return output;
}