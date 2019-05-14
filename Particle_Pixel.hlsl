
struct PS_IN
{
	float4 Position : SV_POSITION;
	float2 TexUV	: TEXCOORD;
	float3 Color	: COLOR;
};

float4 PARTICLES_PS_main(PS_IN input) : SV_Target
{
	float3 FinalColor = input.Color.xyz;
	return float4(FinalColor, 0);
}