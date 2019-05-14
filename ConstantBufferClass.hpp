#ifndef CONSTANTBUFFERCLASS_HPP
#define CONSTANTBUFFERCLASS_HPP

#include <d3d11.h>
#include <DirectXMath.h>

#include "Globals.hpp"	// PI,SCREENWIDTH/HEIGHT in InitializeConstantMatrices
#include "objLoader.hpp"


struct MatrixBufferLoaded
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct MatrixBufferStored
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	DirectX::XMFLOAT4 Kd;

	DirectX::XMFLOAT4 ColorID;
};


class ConstantBufferClass {
private:
	MatrixBufferLoaded	UnformattedMatrixData;
	MatrixBufferStored	FormattedMatrixData;
	ID3D11Buffer*		ConstantBuffer;

	void InitializeConstantMatrices();
	void CreateSetConstantBuffers(
		ID3D11Device*			*Device,
		ID3D11DeviceContext*	*DeviceContext
	);


public:
	ConstantBufferClass();
	~ConstantBufferClass();
	/* ------------- COMMENTS -------------
	Releases internal [ID3D11*Buffer ConstantBuffer]
	*/
	void ReleaseAll();

	MatrixBufferStored *GetFormattedMatrixData();
	ID3D11Buffer* *GetConstantBuffer();

	/* ------------- COMMENTS -------------
	Initializes UnformattedMatrixData,
	Reformats to FormattedMatrixData,
	Creates & sets constantbuffer to register (0).
	*/
	void Initialize(
		ID3D11Device*		 *Device,
		ID3D11DeviceContext* *DeviceContext
	);

	/* ------------- COMMENTS -------------
	Adds the Kd-values from the objectData parameter to the
	FormattedMatrixData.
	*/
	void AddMaterialDataToFormattedStruct(ObjectDataClass objectData);

	/*
	Reformats the internal UnformattedDataStruct to the internal FormattedDataStruct.
	*/
	void MatrixToFloat4X4Reformat();

};



// -------------- NON-CLASS FUNCTIONS --------------
void AlterConstantBuffers(
	ID3D11Buffer* TargetBuffer,
	MatrixBufferStored TargetStruct,
	ID3D11DeviceContext* *DeviceContext
);





#endif