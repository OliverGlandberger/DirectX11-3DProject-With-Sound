#include "LightHandlerClass.hpp"



LightHandlerClass::LightHandlerClass()
{
	// Declare temporary array so that all PointLight's positions can be set here from the handler and not internally.
	DirectX::XMFLOAT3 AllPointPositions[2];
	AllPointPositions[0] = { -5.0f, 1.0f, 1.0f };	// To the left of all current objects.
	AllPointPositions[1] = { 0.0f, 20.0f, 0.0f };	// Above all current objects.

	for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
		this->PointLightArray[i].SetWorldPos(AllPointPositions[i]);
	}
}
LightHandlerClass::~LightHandlerClass()
{

}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void LightHandlerClass::ReleaseAll()
{
	this->LightBuffer->Release();
}





CB_Lights_Stored *LightHandlerClass::GetFormattedStruct()
{
	return &this->Formatted_CB_Data;
}

ID3D11Buffer* *LightHandlerClass::GetLightBuffer()
{
	return &this->LightBuffer;
}





void LightHandlerClass::InitializeLightBuffer(
	ID3D11Device*		*Device,
	ID3D11DeviceContext* *DeviceContext
)
{
	// BUFFER DESCRIPTION: 'Settings'
	D3D11_BUFFER_DESC cbDesc;
	memset(&cbDesc, 0, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(Formatted_CB_Data);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;						// Currently contains the CameraPosition which is a dynamic 
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// value that must be updated.
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			// 
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// SUBRESOURCE DATA: Package the data
	D3D11_SUBRESOURCE_DATA InitData;
	memset(&InitData, 0, sizeof(InitData));
	InitData.pSysMem = &this->Formatted_CB_Data;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	(*Device)->CreateBuffer(&cbDesc, &InitData, &this->LightBuffer);
	(*DeviceContext)->PSSetConstantBuffers(1, 1, &this->LightBuffer);
	(*DeviceContext)->GSSetConstantBuffers(1, 1, &this->LightBuffer);
}





void LightHandlerClass::InitializeLightBufferStruct()
{
	for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
		this->Formatted_CB_Data.PointLightArray[i].x = this->PointLightArray[i].GetWorldPos().x;
		this->Formatted_CB_Data.PointLightArray[i].y = this->PointLightArray[i].GetWorldPos().y;
		this->Formatted_CB_Data.PointLightArray[i].z = this->PointLightArray[i].GetWorldPos().z;
	}
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+