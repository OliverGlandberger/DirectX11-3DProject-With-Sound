#ifndef INPUTCLASS_HPP
#define INPUTCLASS_HPP

#include "objLoader.hpp"
#include "TextureLoader.hpp"

#include "HeightMapLoaderClass.hpp"
#include "HeightMapClass.hpp"
#include "Object.hpp"		// Initialize()

#include "Globals.hpp"

class InputClass {
private:
	// Program Input
	objLoaderClass				ObjectLoader;
	TextureClass				*ObjectTextureLoaderArray;
	HeightMapLoaderClass		HeightMapLoader;
	TextureClass				HeightMapTextureLoader;

	ID3D11ShaderResourceView*	ShaderResourceViewArray[OBJ_COUNT];	// Contains the shaderResourceViews to the Textures in the TextureLoaderArray.
	ID3D11ShaderResourceView*	HeightMapShaderResourceView[HEIGHTMAP_TEXTURE_COUNT];


	// User Input
	MSG msg = { 0 };

	POINT LastMouseCoordinates = { 0, 0 };
	POINT CursorMovement = { 0, 0 };

	void UpdateCursorMovement(POINT NewCursorCoordinates);

public:
	InputClass();
	~InputClass();
	void ReleaseAll();

	void Initialize(
		ObjectClass*			ObjectArray,
		HeightMapClass			*HeightMap,
		ID3D11Device*			*Device,
		ID3D11DeviceContext*	*DeviceContext
	);

	/* ------------- COMMENTS -------------
	Updates CursorMovement based on difference between new and old cursor coordinates,
	then returns the updated CursorMovement.
	*/
	POINT GetCursorMovement(POINT NewCursorCoordinates);

	ID3D11ShaderResourceView*		*GetShaderResourceViewArray();

	ID3D11ShaderResourceView*		*GetHeightMapShaderResourceView();

	HeightMapLoaderClass*			GetHeightMapLoader();

	VertexDataClass* *GetVerticeData();

	ObjectDataClass *GetObjectData();
};


#endif