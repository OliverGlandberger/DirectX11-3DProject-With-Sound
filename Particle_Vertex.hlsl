
struct VS_IN
{
	float3 Position	: POSITION;
	float2 TexUV	: TEXCOORD;
	float3 Color	: COLOR;
};

struct GS_IN
{
	float4 Position : SV_POSITION;
	float2 TexUV	: TEXCOORD;
	float3 Color	: COLOR;
};

GS_IN PARTICLES_VS_main(VS_IN input)
{
	GS_IN output = (GS_IN)0;

	output.Position = float4(input.Position, 1);
	output.TexUV = input.TexUV;
	output.Color = input.Color;

	return output;
}