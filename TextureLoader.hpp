#ifndef TEXTURELOADER_HPP
#define TEXTURELOADER_HPP

#include "WICTextureLoader.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <string>



class TextureClass
{
private:
	ID3D11ShaderResourceView* TextureShaderResourceView;

public:
	TextureClass();
	~TextureClass();
	void ReleaseAll();


	/* ------------- COMMENTS -------------
	Loads an image (PNG, JPG, etc.) from 'FileName', converting it to a resource
	which is then directly stored into the internal ShaderResourceView.
	*/
	void loadTextureFromFile(
		ID3D11Device* *Device,
		std::wstring FileName
	);

	ID3D11ShaderResourceView* *getTextureShaderResourceView();
};

#endif