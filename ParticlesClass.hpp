#ifndef PARTICLESCLASS_HPP
#define PARTICLESCLASS_HPP

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <time.h>

#include "Globals.hpp"
#include "Camera.hpp"


struct ParticleProperties
{
	DirectX::XMFLOAT3	CameraRelativePosition;
	DirectX::XMFLOAT3	Position;
	DirectX::XMFLOAT3	Color;
	float				Velocity;
	bool				Active;
};

struct ParticleVertexData
{
	DirectX::XMFLOAT3	Position;
	DirectX::XMFLOAT2	TexCoord;
	DirectX::XMFLOAT3	Color;
};


class ParticleSystem
{
private:
	ID3D11InputLayout*			VertexLayout;
	ID3D11VertexShader*			VertexShader;
	ID3D11GeometryShader*		GeometryShader;
	ID3D11PixelShader*			PixelShader;

	ID3D11Buffer*				VertexBuffer;

	int Capacity;
	int MaxVertexCount;
	int CurrentParticleCount;
	float LastKeyTime;
	float CurrentTime;

	ParticleProperties* PropertiesArray;
	ParticleVertexData* VertexArray;

	
	// PRIVATE FUNCTIONS

	void InitializeShaders(
		ID3D11Device* *Device
	);

	void InitializeBuffers(
		ID3D11Device* *Device
	);

	void CreateParticle(
		DirectX::XMFLOAT3 CameraPosition
	);

	void UpdateVertexBuffer(
		ID3D11DeviceContext* *DeviceContext,
		Camera* theCamera
	);

	void UpdateParticlePositions(
		DirectX::XMFLOAT3 CameraPosition
	);



public:
	ParticleSystem();
	~ParticleSystem();
	void ReleaseAll();

	void InitializeAll(
	ID3D11Device* *Device);

	void UpdateSystem(
		ID3D11DeviceContext* *DeviceContext,
		float dt,
		Camera* theCamera
	);

	void SetShadingContext(
		ID3D11DeviceContext* *DeviceContext
	);

	void RenderParticles(
		ID3D11DeviceContext* *DeviceContext
	);

};
#endif