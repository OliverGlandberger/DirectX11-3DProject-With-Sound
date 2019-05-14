#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <d3d11.h>
#include <DirectXMath.h>
#include "ConstantBufferClass.hpp"




class Camera {
private:
	float CameraMovementSpeed = 2.0;
	float CameraRotationSpeed = 0.02;
	DirectX::XMVECTOR CameraStartPosition = { 10, 10, 10 };

	void MoveCameraUp();
	void MoveCameraLeft();
	void MoveCameraRight();
	void MoveCameraForward();
	void MoveCameraBackward();
	void RotateCameraVertically(POINT MouseMovement);
	void RotateCameraHorizontally(POINT mouseMovement);
	void UpdateRightDirection();

public:
	DirectX::XMVECTOR CameraPosition;
	DirectX::XMVECTOR CameraDirection;
	DirectX::XMVECTOR CameraUpDirection;
	DirectX::XMVECTOR CameraRightDirection;
	DirectX::XMMATRIX View;

	Camera();
	~Camera();

	// Current controls:
	// 'w','a','s','d'	controls camera movement.
	// mouse cursor		controls camera direction.
	// 'r'				resets the camera.
	// 'h'				switches to hovercam
	void UpdateCamera(
		TCHAR					CharacterMessage,
		POINT					MouseCoordinates,
		MatrixBufferStored		*FormattedStructData,
		ID3D11Buffer*			*GSConstantBuffer,
		ID3D11DeviceContext*	*DeviceContext,
		bool					HoverCamActivationStatus
	);


	void ResetCamera();




	// GET FUNCTIONS

	DirectX::XMFLOAT3 GetCameraPositionFloat3();
};




#endif