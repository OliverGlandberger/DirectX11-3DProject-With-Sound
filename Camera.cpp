#include "Camera.hpp"

Camera::Camera()
{
	this->CameraPosition = this->CameraStartPosition;
	this->CameraDirection = { 0, 0, 1 };
	this->CameraUpDirection = { 0, 1, 0 };
}
Camera::~Camera()
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Camera::UpdateCamera(
	TCHAR					CharacterMessage,
	POINT					MouseCoordinates,
	MatrixBufferStored		*FormattedStructData,
	ID3D11Buffer*			*GSConstantBuffer,
	ID3D11DeviceContext*	*DeviceContext,
	bool					HoverCamActivationStatus
)
{
	bool NeedsToUpdate = false;

	// CAMERA MOVEMENT
	if (CharacterMessage == 'W') {
		NeedsToUpdate = true;
		this->MoveCameraForward();
	}
	else if (CharacterMessage == 'A') {
		NeedsToUpdate = true;
		this->MoveCameraLeft();
	}
	else if (CharacterMessage == 'S') {
		NeedsToUpdate = true;
		this->MoveCameraBackward();
	}
	else if (CharacterMessage == 'D') {
		NeedsToUpdate = true;
		this->MoveCameraRight();
	}
	else if (CharacterMessage == 32) {	// 32 == Spacebar
		NeedsToUpdate = true;
		this->MoveCameraUp();
	}
	else if (CharacterMessage == 'H')	// By-product of merging WindowsProcedure's handling of input and
		NeedsToUpdate = true;			// the attempt here.


	// CAMERA ROTATION
	if (MouseCoordinates.y != 0) {
		NeedsToUpdate = true;
		this->RotateCameraVertically(MouseCoordinates);
	}
	if (MouseCoordinates.x != 0) {
		NeedsToUpdate = true;
		this->RotateCameraHorizontally(MouseCoordinates);
	}


	// MISC COMMANDS - Needs to be called after movement and rotation.
	if (CharacterMessage == 'R') {
		NeedsToUpdate = true;
		this->ResetCamera();
	}


	// IF DATA HAS BEEN UPDATED
	if (NeedsToUpdate) {	// If HoverCam is activated, MainCam values are still updated but not the view matrix.
		// Creates the new viewMatrix
		this->View = DirectX::XMMatrixLookToLH(
			this->CameraPosition,
			this->CameraDirection,
			this->CameraUpDirection
		);

		this->UpdateRightDirection();

		if (!HoverCamActivationStatus) {
			// Updates GSConstantDataFloats with the new viewmatrix.
			DirectX::XMFLOAT4X4 FormattedViewMatrix;
			DirectX::XMStoreFloat4x4(&FormattedViewMatrix, this->View);
			FormattedStructData->view = FormattedViewMatrix;

			// Alters the constantbuffer with the updated GSConstDataFloats
			AlterConstantBuffers(*GSConstantBuffer, *FormattedStructData, DeviceContext);
		}


	}
}


// Resets the camera to its original position
void Camera::ResetCamera() {
	this->CameraPosition = this->CameraStartPosition;
	this->CameraDirection = { 0, 0, 1 };
	this->CameraUpDirection = { 0, 1, 0 };
}






// GET FUNCTIONS

DirectX::XMFLOAT3 Camera::GetCameraPositionFloat3()
{
	DirectX::XMFLOAT3 TempFloat3;
	DirectX::XMStoreFloat3(&TempFloat3, this->CameraPosition);
	return TempFloat3;
}







//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// Moves the camera Upwards!
void Camera::MoveCameraUp()
{
	DirectX::XMVECTOR upVector = DirectX::XMVector3Normalize(this->CameraUpDirection);
	upVector = DirectX::XMVectorScale(upVector, this->CameraMovementSpeed);
	this->CameraPosition = DirectX::XMVectorAdd(this->CameraPosition, upVector);
}





// Moves the CameraPosition to the Left, based on the crossproduct of
// the current CameraDirection & CameraUpDirection
void Camera::MoveCameraLeft() {
	DirectX::XMVECTOR MovementVector = DirectX::XMVector3Cross(this->CameraDirection, this->CameraUpDirection);// Creates rightVector
	DirectX::XMVector3Normalize(MovementVector);
	MovementVector = DirectX::XMVectorScale(MovementVector, CameraMovementSpeed);		// The lower the scale, the slower the movement

	this->CameraPosition = DirectX::XMVectorAdd(this->CameraPosition, MovementVector);
}





// Moves the CameraPosition to the Right, based on the crossproduct of
// the current CameraUpDirection & CameraDirection
void Camera::MoveCameraRight() {
	DirectX::XMVECTOR MovementVector = DirectX::XMVector3Cross(this->CameraUpDirection, this->CameraDirection);// Creates rightVector
	DirectX::XMVector3Normalize(MovementVector);
	MovementVector = DirectX::XMVectorScale(MovementVector, CameraMovementSpeed);		// The lower the scale, the slower the movement

	this->CameraPosition = DirectX::XMVectorAdd(this->CameraPosition, MovementVector);
}





// Moves Camera forward based on the current CameraDirection
void Camera::MoveCameraForward() {
	DirectX::XMVECTOR MovementVector = this->CameraDirection;
	DirectX::XMVector3Normalize(MovementVector);
	MovementVector = DirectX::XMVectorScale(MovementVector, CameraMovementSpeed);

	this->CameraPosition = DirectX::XMVectorAdd(this->CameraPosition, MovementVector);
}





// Moves Camera backward based on the current CameraDirection
void Camera::MoveCameraBackward() {
	DirectX::XMVECTOR MovementVector = DirectX::XMVectorScale(this->CameraDirection, -1);
	DirectX::XMVector3Normalize(MovementVector);
	MovementVector = DirectX::XMVectorScale(MovementVector, CameraMovementSpeed);

	this->CameraPosition = DirectX::XMVectorAdd(this->CameraPosition, MovementVector);
}





// Rotates the camera Vertically through Quaternions based
// on the MouseMovement.y
// AIRPLANE MOVEMENT IS TYPICALLY TURNED OFF.
void Camera::RotateCameraVertically(POINT MouseMovement) {
	DirectX::XMVECTOR RightAxis = DirectX::XMVector3Cross(this->CameraUpDirection, this->CameraDirection);

	float RotationAngle = MouseMovement.y * CameraRotationSpeed;
	bool MovingUp = MouseMovement.y < 0;

	DirectX::XMVECTOR RotationQuaternion = DirectX::XMQuaternionRotationAxis(RightAxis, RotationAngle);

	// Camera-Plane movement is currently turned OFF. Un-comment the line below to turn it ON.
	//	this->CameraUpDirection = DirectX::XMVector3Rotate(this->CameraUpDirection, RotationQuaternion);
	

	DirectX::XMFLOAT3 New;

	DirectX::XMVECTOR NewRotation = DirectX::XMVector3Rotate(this->CameraDirection, RotationQuaternion);
	DirectX::XMStoreFloat3(&New, NewRotation);

	DirectX::XMFLOAT3 Old;
	DirectX::XMStoreFloat3(&Old, this->CameraDirection);

	DirectX::XMFLOAT3 tempFloat;
	DirectX::XMVECTOR tempVec = DirectX::XMVector3Dot(DirectX::XMVECTOR{ 0, 0, New.z }, DirectX::XMVECTOR{ 0, 0, Old.z });
	DirectX::XMStoreFloat3(&tempFloat, tempVec);

	if (tempFloat.x > 0) {

		if (MovingUp) {
			if (New.y > Old.y) {
				this->CameraDirection = NewRotation;
			}
		}
		else {
			if (New.y < Old.y) {
				this->CameraDirection = NewRotation;
			}
		}

	}
	
/*
	if (tempFloat.y > tempFloat2.y) {

	
	}
*/

}





// Rotates the camera Horiontally through Quaternions based
// on the MouseMovement.x
void Camera::RotateCameraHorizontally(POINT MouseMovement) {
	float RotationAngle = MouseMovement.x * CameraRotationSpeed;

	DirectX::XMVECTOR RotationQuaternion = DirectX::XMQuaternionRotationAxis(this->CameraUpDirection, RotationAngle);

	// Camera-Plane movement is currently turned OFF. Un-comment the line below to turn it ON.
	//this->CameraUpDirection = DirectX::XMVector3Rotate(this->CameraUpDirection, RotationQuaternion);
	this->CameraDirection = DirectX::XMVector3Rotate(this->CameraDirection, RotationQuaternion);
}





void Camera::UpdateRightDirection()
{
	this->CameraRightDirection = DirectX::XMVector3Cross(this->CameraUpDirection, this->CameraDirection);
}