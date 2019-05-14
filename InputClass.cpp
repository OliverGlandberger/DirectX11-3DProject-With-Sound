#include "InputClass.hpp"

InputClass::InputClass()
{
	this->ObjectTextureLoaderArray = new TextureClass[OBJ_COUNT];
}
InputClass::~InputClass()
{

}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void InputClass::ReleaseAll()
{
	for (int i = 0; i < OBJ_COUNT; i++) {
		this->ObjectTextureLoaderArray[i].ReleaseAll();
	}
}





void InputClass::Initialize(
	ObjectClass*			ObjectArray,
	HeightMapClass			*HeightMap,
	ID3D11Device*			*Device,
	ID3D11DeviceContext*	*DeviceContext
)
{
	for (int i = 0; i < OBJ_COUNT; i++) {
		this->ObjectLoader.importObjFile(&ObjectArray[i]);
		this->ObjectTextureLoaderArray[i].loadTextureFromFile(
			Device,
			ObjectArray[i].getObjectData()->textureFileName
		);
	}
	
	for (int i = 0; i < OBJ_COUNT; i++)
		this->ShaderResourceViewArray[i] = *this->ObjectTextureLoaderArray[i].getTextureShaderResourceView();

	// ---------------- Height Map ----------------

	// Loading in the HeightMap and storing it into the Height Map object
	this->HeightMapLoader.LoadHeightMap(
		HeightMap->GetImageData(),
		HeightMap->GetHeightMapVertexDataArray()
	);

	

	// Loading in and storing the HeightMap's seamless texture
	for (int i = 0; i < HEIGHTMAP_TEXTURE_COUNT; i++)
	{
		this->HeightMapTextureLoader.loadTextureFromFile(
			Device,
			this->HeightMapLoader.GetHeightMapTextureName(i)
		);

		this->HeightMapShaderResourceView[i] = *this->HeightMapTextureLoader.getTextureShaderResourceView();
	}
}





// GET FUNCTIONS

ID3D11ShaderResourceView* *InputClass::GetShaderResourceViewArray()
{
	return this->ShaderResourceViewArray;
}

ID3D11ShaderResourceView* *InputClass::GetHeightMapShaderResourceView()
{
	return this->HeightMapShaderResourceView;
}

POINT InputClass::GetCursorMovement(POINT NewCursorCoordinates)
{
	this->UpdateCursorMovement(NewCursorCoordinates);
	return this->CursorMovement;
}

HeightMapLoaderClass* InputClass::GetHeightMapLoader()
{
	return &this->HeightMapLoader;
}

VertexDataClass* *InputClass::GetVerticeData()
{
	return this->ObjectLoader.getVerticeData();
}

ObjectDataClass *InputClass::GetObjectData()
{
	return this->ObjectLoader.getObjectData();
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void InputClass::UpdateCursorMovement(POINT NewCursorCoordinates)
{
	this->CursorMovement.x = NewCursorCoordinates.x - LastMouseCoordinates.x;
	this->CursorMovement.y = NewCursorCoordinates.y - LastMouseCoordinates.y;
	this->LastMouseCoordinates = NewCursorCoordinates;
}