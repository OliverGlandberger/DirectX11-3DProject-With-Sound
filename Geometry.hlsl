struct GSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 uv :	TEXCOORD;
	float4 Normal : NORMAL;
};

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 uv : TEXCOORD;
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

[maxvertexcount(3)]
void GS_main(triangle GSSceneIn triangle_input[3], inout TriangleStream<PSSceneIn> OutputStream)
{
	PSSceneIn output = (PSSceneIn)0;
	float3 worldSpacePos[3];

	for (uint i = 0; i < 3; i++)
	{
		output.Pos = triangle_input[i].Pos;
		output.Pos = mul(world, output.Pos);
		//output.Pos = mul(Y_RotationMatrix, output.Pos);
		output.worldPos = output.Pos;			// WORLDSPACE POS SAVED
		output.Pos = mul(view, output.Pos);
		output.Pos = mul(projection, output.Pos);

		output.uv = triangle_input[i].uv;
		output.Normal = triangle_input[i].Normal;

		output.Kd = Kd.xyz;

		OutputStream.Append(output);
	}
	OutputStream.RestartStrip();
};