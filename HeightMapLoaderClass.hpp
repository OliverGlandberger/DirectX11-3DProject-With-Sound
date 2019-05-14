#ifndef HEIGHTMAPLOADERCLASS_HPP
#define HEIGHTMAPLOADERCLASS_HPP

#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>

#include "Globals.hpp"

#include "lodepng.h"
#include "WICTextureLoader.h"
#include <iterator>
#include <codecvt>
#include <locale>
#include <string>

#include "HeightMapClass.hpp"


// V/VT/VN V/VT/VN V/VT/VN
struct FaceIndexData
{
	DirectX::XMFLOAT3 Vertex1 = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 Vertex2 = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 Vertex3 = { 0.0f, 0.0f, 0.0f };
};





class HeightMapLoaderClass {
private:
	std::string HeightMapFileName;
	std::string HeightMapImageName;
	std::wstring *HeightMapTextureName;

	bool SmoothNormals;
	int HeightMapWidth;
	int HeightMapHeight;
	int VertexCount;
	int fc;

	float *VertexHeightArray;
	DirectX::XMFLOAT3 *TempNormalArray;




	// PRIVATE FUNCTIONS

	void StoreToVertexData(HeightMapVertexData *HeightMapVertexDataArray);
	void ConvertToVertexData(HeightMapVertexData* *HeightMapVertexDataArray);





public:
	HeightMapLoaderClass();
	~HeightMapLoaderClass();
	void ReleaseAll();

	/* ------------- COMMENTS -------------
	Loads a PNG file directly into raw pixel data, returned in
	as a 'vector<unsigned char> in the following format: RGBARGBA...
	*/
	void LoadHeightMap(
		std::vector<unsigned char>	*ImageData,
		HeightMapVertexData*		*HeightMapVertexDataArray
	);





	// GET FUNCTIONS

	int GetFaceCount();

	int GetHeightMapWidth();
	int GetHeightMapHeight();

	float* GetVertexHeightArray();

	std::wstring GetHeightMapTextureName(int TextureSlot);
};

#endif