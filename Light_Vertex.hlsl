cbuffer GSConstantBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;

	float4x4 X_RotationMatrix;
	float4x4 Y_RotationMatrix;

	float4 Kd;
};


struct VS_IN
{
	float3 Pos : POSITION;
	float2 uv :	TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 uv :	TEXCOORD;
};



VS_OUT LIGHT_VS_main(uint id: SV_VERTEXID)
{
	VS_OUT output = (VS_OUT)0;

	//	ID-structure
	//	0	1	-	x	x
	//	2	3	-		x
	// Boolean math! If the expression is true, set 1, otherwise set 0.
	output.Pos.x = ((id == 1 || id == 3) * 2) - 1;	// if 1||3, become 1, otherwise become 0.
	output.Pos.y = ((id == 0 || id == 1) * 2) - 1;	// FLIPPED, because pipeline flips image upside down somewhere ;_;
	output.Pos.z = 0;	// Z-Coordinate: Always zero (No Depth).
	output.Pos.w = 1;	// W-Coordinate: Always one (Point).

	//	0	1	-		x
	//	2	3	-	x	x	
	output.uv.x = (id == 1|| id == 3);
	output.uv.y = (id == 2 || id == 3);

	return output;
}