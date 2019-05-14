
cbuffer GSConstantBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;

	float4 Kd;
};

struct GS_IN
{
	float4 Position	: SV_POSITION;
	float2 TexUV	: TEXCOORD;
	float3 Color	: COLOR;
};

struct PS_IN
{
	float4 Position	: SV_POSITION;
	float2 TexUV	: TEXCOORD;
	float3 Color	: COLOR;
};

[maxvertexcount(3)]
void PARTICLES_GS_main(triangle GS_IN triangle_input[3], inout TriangleStream<PS_IN> OutputStream)
{
	PS_IN output = (PS_IN)0;

	// TEMPORARY

	for (uint i = 0; i < 3; i++)
	{
		output.Position = triangle_input[i].Position;

		output.Position = mul(world, output.Position);
		output.Position = mul(view, output.Position);
		output.Position = mul(projection, output.Position);

		output.TexUV = triangle_input[i].TexUV;
		output.Color = triangle_input[i].Color;

		OutputStream.Append(output);
	}
	OutputStream.RestartStrip();
}