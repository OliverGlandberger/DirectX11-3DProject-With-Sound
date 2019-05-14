#ifndef HEIGHTMAPCLASS_HPP
#define HEIGHTMAPCLASS_HPP

#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>

#include "lodepng.h"


struct HeightMapVertexData
{
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT2 texCoord = { 0.0f, 0.0f };
	DirectX::XMFLOAT3 Normal = { 0.0f, 0.0f, 0.0f };
};





class HeightMapClass
{
private:
	HeightMapVertexData* HeightMapVertexDataArray;
	std::vector<unsigned char> ImageData;





public:
	HeightMapClass();
	~HeightMapClass();




	
	// GET FUNCTIONS

	std::vector<unsigned char>* GetImageData();

	HeightMapVertexData* *GetHeightMapVertexDataArray();
};

#endif