#include "ObjectHandlerClass.hpp"

ObjectHandlerClass::ObjectHandlerClass()
{
	std::string FileNameArray[3];
	FileNameArray[0] = "obj_files/cube_green_phong_12_tris_TRIANGULATED.obj";
	FileNameArray[1] = "obj_files/donut_red_lambert_20000_tris_TRIANGLULATED.obj";
	FileNameArray[2] = "obj_files/sphere_blue_blinn_760_tris_TRIANGULATED.obj";

	this->VertexBufferArray = new ID3D11Buffer*[OBJ_COUNT];
	for (int i = 0; i < OBJ_COUNT; i++)
		this->VertexBufferArray[i] = nullptr;

	this->AllObjects = new ObjectClass[OBJ_COUNT];

	for (int i = 0; i < OBJ_COUNT; i++)
		*this->AllObjects[i].getFileName() = FileNameArray[i];
}

ObjectHandlerClass::~ObjectHandlerClass()
{

}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ObjectHandlerClass::ReleaseAll()
{
	for (int i = 0; i < OBJ_COUNT; i++) {
		this->VertexBufferArray[i]->Release();
	}
}








// SET FUNCTIONS

void ObjectHandlerClass::SetAllObjectData(
	ID3D11Device*	*Device,
	ObjectClass		*ObjectArray
)
{
	// Fetch objDataClass & calcDataClass from internal object array.
	// To do this, import 
	HRESULT hr;

	// Set up description & fill it with common data
	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	// Set up vertexdata container and fill it with the common data.
	D3D11_SUBRESOURCE_DATA vertexData;
	memset(&vertexData, 0, sizeof(vertexData));
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	for (int i = 0; i < OBJ_COUNT; i++) {

		// Set description&Vertexdata-parameters which are unique to each object.
		vertexBufferDesc.ByteWidth = (sizeof(VertexDataClass) * (ObjectArray[i].getObjectData()->faceCount * 3));
		vertexData.pSysMem = *ObjectArray[i].getVerticeData();


		hr = (*Device)->CreateBuffer(&vertexBufferDesc, &vertexData, &this->VertexBufferArray[i]);
		if (FAILED(hr))
			MessageBox(NULL, L"Failed to create one of maybe many VertexBuffers.", L"ERROR: 'SetAllObjectData() in ObjectHandlerClass.cpp'", MB_OK);
	}
	
	// Creation of all the clones
	//
	//
	//

	// --------------- All Cubes ---------------
	int CubeCount = 20;

	DirectX::XMFLOAT4X4 *CloneWorldMatrixArray_Cube;
	CloneWorldMatrixArray_Cube = new DirectX::XMFLOAT4X4[CubeCount];

	for (int i = 0; i < CubeCount; i++) {

		CloneWorldMatrixArray_Cube[i] =
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			3.0f*i, 5.0f, 0.0f, 1.0f
		};

	}

	// --------------- All Donuts ---------------
	int DonutCount = 20;

	DirectX::XMFLOAT4X4 *CloneWorldMatrixArray_Donut;
	CloneWorldMatrixArray_Donut = new DirectX::XMFLOAT4X4[DonutCount];

	for (int i = 0; i < DonutCount; i++) {

		CloneWorldMatrixArray_Donut[i] =
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 5.0f, 3.0f*i, 1.0f
		};
	}

	// --------------- All Spheres ---------------
	int SphereCount = 20;
	
	DirectX::XMFLOAT4X4 *CloneWorldMatrixArray_Sphere;
	CloneWorldMatrixArray_Sphere = new DirectX::XMFLOAT4X4[SphereCount];

	for (int i = 0; i < SphereCount; i++) {

		CloneWorldMatrixArray_Sphere[i] =
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, (5+3.0f*i), 0.0f, 1.0f
		};
	}



	// --------------- Allocate everything. ---------------

	AllObjects[0].CreateAllClones(CubeCount, CloneWorldMatrixArray_Cube);
	AllObjects[1].CreateAllClones(DonutCount, CloneWorldMatrixArray_Donut);
	AllObjects[2].CreateAllClones(SphereCount, CloneWorldMatrixArray_Sphere);
	

	// -----Delete allocated arrays.
	delete[] CloneWorldMatrixArray_Cube;
	delete[] CloneWorldMatrixArray_Donut;
	delete[] CloneWorldMatrixArray_Sphere;


}


void ObjectHandlerClass::SetHeightMapVertexData(
	ID3D11Device*			*Device,
	HeightMapVertexData*	*HeightMapVertexDataArray,
	HeightMapLoaderClass*	HeightMapData
)
{
	// We're fetching the HeightMapVertexArray from within the HeightMapClass
	// through a 'GetFunction'.
	HRESULT hr;

	// Set up description & fill it with common data
	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	// Set up vertexdata container and fill it with the common data
	D3D11_SUBRESOURCE_DATA vertexData;
	memset(&vertexData, 0, sizeof(vertexData));
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	vertexData.pSysMem = (*HeightMapVertexDataArray);

	vertexBufferDesc.ByteWidth = (sizeof(HeightMapVertexData) * HeightMapData->GetFaceCount() * 3);

	hr = (*Device)->CreateBuffer(&vertexBufferDesc, &vertexData, &this->HeightMapVertexBuffer);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create HeightMapVertexBuffer.", L"ERROR: 'SetHeigthMapVertexData() in ObjectManagerClass.cpp'", MB_OK);
}





// GET FUNCTIONS

ObjectClass* *ObjectHandlerClass::GetAllObjects()
{
	return &this->AllObjects;
}

HeightMapClass* ObjectHandlerClass::GetHeightMap()
{
	return &this->HeightMap;
}

ID3D11Buffer** *ObjectHandlerClass::GetVertexBuffers()
{
	return &this->VertexBufferArray;
}

ID3D11Buffer* *ObjectHandlerClass::GetHeightMapVertexBuffer()
{
	return &this->HeightMapVertexBuffer;
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//
//
//