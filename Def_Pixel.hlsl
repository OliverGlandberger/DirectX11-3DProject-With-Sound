

Texture2D ShaderTexture : register(t0);
Texture2D HeightMapTexture : register(t1);
SamplerState SamplerTypeWrap : register(s0);


struct Deferred_PixelShader_Input
{
	float4 Position : SV_POSITION;
	float2 TexUV	: TEXCOORD;
	float4 Normal	: NORMAL;
	
	float3 worldPos : WORLDPOSITION;
	float3 Kd		: KDVALUES;

	float4 ColorID	: COLORID;
};

// Data which will be rendered to the 'RenderTargetView's inside the 'DeferredBuffersClass'.
struct RenderTarget_Input
{
	// Texture2D's are all float4
	float4 ColorTexture2D	 : SV_Target0;
	float4 NormalTexture2D	 : SV_Target1;
	float4 WorldposTexture2D : SV_Target2;
	float4 ColorIDTexture2D  : SV_Target3;
};


RenderTarget_Input DEF_PS_main(Deferred_PixelShader_Input input) : SV_Target
{
	RenderTarget_Input output = (RenderTarget_Input)0;
	float3 Color;

	float HeightMapChecker = HeightMapTexture.Sample(SamplerTypeWrap, (0.0f, 0.0f)).x;



	if (HeightMapChecker != 0.0f && input.worldPos.y > 2.5f)
	{
		float3 Color1;
		float3 Color2;
		float TempVariable = (input.worldPos.y - 2.5f);

		// Blend mapping code (Blend mapping occurs between 2.5f and 3.5f)
		if (input.worldPos.y > 2.5f && input.worldPos.y < 3.5f)
		{
			Color1 = ShaderTexture.Sample(SamplerTypeWrap, input.TexUV).xyz;
			Color2 = HeightMapTexture.Sample(SamplerTypeWrap, input.TexUV).xyz;

			Color = (TempVariable * Color2.xyz) + ((1 - TempVariable) * Color1.xyz);
		}

		else
		{
			Color = HeightMapTexture.Sample(SamplerTypeWrap, input.TexUV).xyz;
		}
	}

	else
	{
		Color = ShaderTexture.Sample(SamplerTypeWrap, input.TexUV).xyz;
	}

	// If picked, re-color
	Color.y += (input.ColorID.w == 1);

	output.ColorTexture2D = float4(Color, 0);
	output.WorldposTexture2D = float4(input.worldPos, 1);
	output.NormalTexture2D = normalize(input.Normal);
	output.ColorIDTexture2D = input.ColorID;


	return output;
}