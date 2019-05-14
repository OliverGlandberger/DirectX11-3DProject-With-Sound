

struct VS_IN
{
	float3 Position : POSITION;
	float2 TexUV	: TEXCOORD;
	float3 Normal	: NORMAL;
};

struct Deferred_PixelShader_Input
{
	float4 Position : SV_POSITION;
	float2 TexUV	: TEXCOORD;
	float4 Normal	: NORMAL;
};


Deferred_PixelShader_Input DEF_VS_main(VS_IN input)
{
	Deferred_PixelShader_Input output = (Deferred_PixelShader_Input)0;

	output.Position = float4(input.Position, 1);
	output.TexUV = input.TexUV;
	output.Normal = float4(input.Normal, 0);

	return output;
}