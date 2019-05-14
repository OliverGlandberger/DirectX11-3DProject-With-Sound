


SamplerState SamplerTypeWrap	: register(s0);
SamplerState SamplerTypeClamp	: register(s1);

Texture2D GBuffer_Color			: register(t0); // Of these 4 textures, the depth has
Texture2D GBuffer_Normal		: register(t1); // to be last due to how rendertargets
Texture2D GBuffer_Worldpos		: register(t2); // are unbinded in the 'unbind function'.
Texture2D GBuffer_ColorID		: register(t3);	// NOTE: ColorID has to be second last!
Texture2D GBuffer_Depth			: register(t4);	// <-- Depth
Texture2D GBuffer_ShadowDepth	: register(t5);

// RWStructuredBuffer<float4> PickingBuffer : register(u1);	//Backbuffer relies in u0.





struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 uv :	TEXCOORD;
};

cbuffer PSConstantBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;

	float4 Kd;
};

cbuffer LightBuffer		 : register(b1)
{
	float4	CONTAINER_PointLightCount_Y_Z_W;	// Purely a container for data, currently Y,Z,W are empty.
	float4	PointLightArray[1];	// This has to be in-sync with POINT_LIGHT_COUNT in LightHandlerClass.hpp

	float4 CameraPos;
}

cbuffer ShadowMapBuffer			: register(b2)
{
	float4x4 ShadowMapLightView;
	float4x4 ShadowMapLightProjection;
}



float4 LIGHT_PS_main(PSSceneIn input) : SV_Target
{

		// MAIN VARIABLES //
//----------------------------------------------------------------------------
	float3 Color;
	float PointLightCount = CONTAINER_PointLightCount_Y_Z_W.x;
	
	float3 Light_Source;
	float3 Pos_To_Light;
	float DotProd;
	float TotalDiffuseEffect = 0;
	float TotalSpecularLight = 0;
	
	float TotalLight = 0;
	float3 Normal = GBuffer_Normal.Sample(SamplerTypeWrap, input.uv);
	float3 worldPos = GBuffer_Worldpos.Sample(SamplerTypeWrap, input.uv).xyz;
//----------------------------------------------------------------------------

		// SHADOW MAPPING VARIABLES //
//----------------------------------------------------------------------------
	float4 WorldPos4 = GBuffer_Worldpos.Sample(SamplerTypeClamp, input.uv);
	float4 ShadowMapLightPos = { -10.0f, 20.0f, -10.0f, 1.0f };
	float4 LightViewPosition = mul(ShadowMapLightView, WorldPos4);
	LightViewPosition = mul(ShadowMapLightProjection, LightViewPosition);

	float2 ShadowMapTexCoord;
	ShadowMapTexCoord.x = LightViewPosition.x / LightViewPosition.w / 2.0f + 0.5f;
	ShadowMapTexCoord.y = -LightViewPosition.y / LightViewPosition.w / 2.0f + 0.5f;

	float SampledDepthValue = 0.0f;
	float CalculatedDepthValue = 0.0f;
	float bias = 0.0001f;

	bool outsideOfShadowMap = false;
//----------------------------------------------------------------------------

	if ((saturate(ShadowMapTexCoord.x) == ShadowMapTexCoord.x) &&
		(saturate(ShadowMapTexCoord.y) == ShadowMapTexCoord.y))
	{
		SampledDepthValue = GBuffer_ShadowDepth.Sample(SamplerTypeClamp, ShadowMapTexCoord).r;

		CalculatedDepthValue = LightViewPosition.z / LightViewPosition.w;
		CalculatedDepthValue = CalculatedDepthValue - bias;
	}

	else
		outsideOfShadowMap = true;




	if (CalculatedDepthValue < SampledDepthValue || outsideOfShadowMap)
	{

		// Fetch texturecolor.
		Color = GBuffer_Color.Sample(SamplerTypeWrap, input.uv);

		// For every light,
		for (int i = 0; i < PointLightCount; i++) {

			// -------------------- Diffuse -----------------------
			// Fetch the current light,
			Light_Source = PointLightArray[i].xyz;

			// Calculate the light's effect,
			Pos_To_Light = (Light_Source - worldPos);
			Pos_To_Light = normalize(Pos_To_Light);
			DotProd = dot(Pos_To_Light, Normal);

			// Sum up the light's effect with the other light's effects.
			TotalDiffuseEffect += clamp(DotProd, 0.0f, 1.0f);

			// -------------------- Specular -----------------------
			//
			// ptl n1 ref  ptc
			//	\  |จจ/  ด
			//	 \ | / ด
			//	  \|/ด
			//			จจ = d
			//
			float n1 = dot(Pos_To_Light, Normal);

			// Only calculate specular light if the light is coming towards the surface - not from underneath
			if (n1 > 0.0) {

				float3 ReflectVector = reflect(Pos_To_Light, Normal);

				float3 Cam_To_Pos = GBuffer_Worldpos.Sample(SamplerTypeWrap, input.uv) - CameraPos;

				float angle = clamp(dot(normalize(Cam_To_Pos), normalize(ReflectVector)), 0.0f, 1.0f);

				float shinyness = 10;
				float Material_spec = 1;

				TotalSpecularLight += Material_spec*(pow(angle, shinyness));
			}

		}

		float3 AmbientLight = clamp((0.45f * (GBuffer_Color.Sample(SamplerTypeWrap, input.uv)).xyz), 0.0f, 1.0f);

		// Add Diffuse and specular light
		Color.xyz *= clamp(TotalDiffuseEffect + TotalSpecularLight, 0.0f, 1.0f);

		// Add ambient light
		Color.xyz += clamp((0.45f * (GBuffer_Color.Sample(SamplerTypeWrap, input.uv)).xyz), 0.0f, 1.0f);
	}

	else
	{
		// Add ambient light
		Color.xyz = clamp((0.15f * (GBuffer_Color.Sample(SamplerTypeWrap, input.uv)).xyz), 0.0f, 1.0f);
	}

	return float4(Color, 0);
}