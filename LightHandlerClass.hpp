#ifndef LIGHTHANDLERCLASS_HPP
#define LIGHTHANDLERCLASS_HPP

#include <D3d11.h>

#include "PointLightClass.hpp"
#include "Globals.hpp"

#define POINT_LIGHT_COUNT 1
// When you change POINT_LIGHT_COUNT, go change the struct manually in Light_Pixel.hlsl.
// Also make sure that all lights are given values in LightHandlerClass() constructor.

struct CB_Lights_Stored {
	// Screen-Width/Height is used by picking, it's implemented in this CB because it's better than using an entire new constantbuffer.
	DirectX::XMFLOAT4 PointLightCount_Y_Z_W = { POINT_LIGHT_COUNT, 0, 0, 0 };	// Container for float info.
	DirectX::XMFLOAT4 PointLightArray[POINT_LIGHT_COUNT];

	DirectX::XMFLOAT4 CameraPosition = { 0, 0, 0, 0 };

};


class LightHandlerClass {
private:
	PointLightClass PointLightArray[POINT_LIGHT_COUNT];	// currently technically unneccesary, but when we add more classes
														// for different type of lights, keeping them structured like this will
														// likely pay off.
	CB_Lights_Stored Formatted_CB_Data;
	ID3D11Buffer* LightBuffer;							// The buffer which contains Formatted_CB_Data


public:
	LightHandlerClass();
	~LightHandlerClass();
	void ReleaseAll();

	CB_Lights_Stored *GetFormattedStruct();
	ID3D11Buffer* *GetLightBuffer();

	/* ------------- COMMENTS -------------
	Creates constantbuffer and sets it to the pixelshader with register(1)
	*/
	void InitializeLightBuffer(
		ID3D11Device*		 *Device,
		ID3D11DeviceContext* *DeviceContext
	);

	/* ------------- COMMENTS -------------
	Fills the the internal Formatted_CB_Data with data from all lights.
	[Currently only PointLights]
	*/
	void InitializeLightBufferStruct();

	





};




#endif