#include "TextureLoader.hpp"

TextureClass::TextureClass()
{
	this->TextureShaderResourceView = nullptr;
}

TextureClass::~TextureClass()
{
	
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void TextureClass::ReleaseAll()
{
	this->TextureShaderResourceView->Release();
}





void TextureClass::loadTextureFromFile(
	ID3D11Device* *Device,
	std::wstring FileName)
{
	std::wcin >> FileName;	

	const WCHAR* ConvertedFileName = FileName.c_str();

	HRESULT hr;

	hr = DirectX::CreateWICTextureFromFile(
		*Device,
		ConvertedFileName,
		nullptr,
		&this->TextureShaderResourceView
	);

	if (FAILED(hr))
		MessageBox(NULL, L"Failed to Convert Texture File From .PNG to 'WIC'.", L"ERROR: 'CreateWICTextureFromFile() in TextureLoader.cpp.'", MB_OK);
}




ID3D11ShaderResourceView* *TextureClass::getTextureShaderResourceView()
{ 
	return &this->TextureShaderResourceView;
}