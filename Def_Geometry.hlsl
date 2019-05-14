struct GSSceneIn
{
	float4 Pos		: SV_POSITION;
	float2 TexUV	: TEXCOORD;
	float4 Normal	: NORMAL;
};

struct PSSceneIn
{
	float4 Pos		: SV_POSITION;
	float2 TexUV	: TEXCOORD;
	float4 Normal	: NORMAL;

	float3 worldPos : WORLDPOSITION;
	float3 Kd		: KDVALUES;

	float4 ColorID	: COLORID;
};

cbuffer GSConstantBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;

	float4 Kd;

	float4 ColorID;
};

cbuffer LightBuffer		 : register(b1)	// This cbuffer is needed for backface culling
{										// (NOTE: Only interested in 'CameraPos')
	float4	PointLightCount_yEmpty_zEmpty_wEmpty;
	float4	PointLightArray[1];

	float4 CameraPos;
}

[maxvertexcount(3)]
void DEF_GS_main(triangle GSSceneIn triangle_input[3], inout TriangleStream<PSSceneIn> OutputStream)
{
	PSSceneIn output = (PSSceneIn)0;
	float3 worldSpacePos[3];

	float3	TempVertexHolder[3];
	float3	TempNormal;
	float	TempDotProduct;
	float3	EdgeVector1;
	float3	EdgeVector2;

	TempVertexHolder[0] = mul(world, triangle_input[0].Pos);
	TempVertexHolder[1] = mul(world, triangle_input[1].Pos);
	TempVertexHolder[2] = mul(world, triangle_input[2].Pos);

	EdgeVector1 = normalize(TempVertexHolder[0].xyz - TempVertexHolder[1].xyz);

	EdgeVector2 = normalize(TempVertexHolder[0].xyz - TempVertexHolder[2].xyz);

	TempNormal = normalize(cross(EdgeVector1, EdgeVector2));

	for (uint i = 0; i < 3; i++)
	{
		// Checking per vertex if the triangle is facing away from the camera.
		// If it is, then the dotproduct will be negative.
		TempDotProduct = dot(normalize(TempVertexHolder[i].xyz - CameraPos.xyz), TempNormal.xyz);

		if (TempDotProduct <= 0)
		{
			output.Pos = triangle_input[i].Pos;
			output.Pos = mul(world, output.Pos);

			output.worldPos = output.Pos;			// WORLDSPACE POS SAVED

			output.Pos = mul(view, output.Pos);
			output.Pos = mul(projection, output.Pos);

			output.TexUV = triangle_input[i].TexUV;
			output.Normal = triangle_input[i].Normal;

			output.Kd = Kd.xyz;

			output.ColorID = ColorID;

			OutputStream.Append(output);
		}
	}
	OutputStream.RestartStrip();
};