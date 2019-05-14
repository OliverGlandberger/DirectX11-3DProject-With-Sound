#include "HeightMapClass.hpp"

HeightMapClass::HeightMapClass()
{
	this->HeightMapVertexDataArray = nullptr;
}

HeightMapClass::~HeightMapClass()
{
	delete this->HeightMapVertexDataArray;
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// GET FUNCTIONS

std::vector<unsigned char>* HeightMapClass::GetImageData()
{
	return &this->ImageData;
};

HeightMapVertexData* *HeightMapClass::GetHeightMapVertexDataArray()
{
	return &this->HeightMapVertexDataArray;
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//
//
//