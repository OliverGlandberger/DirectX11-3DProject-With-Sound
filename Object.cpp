#include "Object.hpp"


ObjectClass::ObjectClass()
{
	this->FileName = "EMPTY";
}

ObjectClass::~ObjectClass()
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ObjectClass::ReleaseAll()
{
	delete[] this->ClonesWorldMatrixArray;
	delete[] this->ClonesGreatestX_array;
	delete[] this->ClonesLowestX_array;
	delete[] this->ClonesGreatestZ_array;
	delete[] this->ClonesLowestZ_array;
}



VertexDataClass* *ObjectClass::getVerticeData()
{
	return &this->VerticeData;
}

ObjectDataClass *ObjectClass::getObjectData()
{
	return &this->ObjectData;
}

std::string *ObjectClass::getFileName()
{
	return &this->FileName;
}




void ObjectClass::SetGreatestX(float GreatestX)
{
	this->TemplateGreatestX = GreatestX;
}
void ObjectClass::SetLowestX(float LowestX)
{
	this->TemplateLowestX = LowestX;
}
void ObjectClass::SetGreatestY(float GreatestY)
{
	this->TemplateGreatestY = GreatestY;
}
void ObjectClass::SetLowestY(float LowestY)
{
	this->TemplateLowestY = LowestY;
}
void ObjectClass::SetGreatestZ(float GreatestZ)
{
	this->TemplateGreatestZ = GreatestZ;
}
void ObjectClass::SetLowestZ(float LowestZ)
{
	this->TemplateLowestZ = LowestZ;
}




float* ObjectClass::GetGreatestX_array()
{
	return this->ClonesGreatestX_array;
}
float* ObjectClass::GetLowestX_array()
{
	return this->ClonesLowestX_array;
}
float* ObjectClass::GetGreatestY_array()
{
	return this->ClonesGreatestY_array;
}
float* ObjectClass::GetLowestY_array()
{
	return this->ClonesLowestY_array;
}
float* ObjectClass::GetGreatestZ_array()
{
	return this->ClonesGreatestZ_array;
}
float* ObjectClass::GetLowestZ_array()
{
	return this->ClonesLowestZ_array;
}

float ObjectClass::GetGreatestY()
{
	return this->TemplateGreatestY;
}
float ObjectClass::GetLowestY()
{
	return this->TemplateLowestY;
}



int ObjectClass::GetCloneCount()
{
	if (this->CloneCount < 1) 
		MessageBox(NULL, L"An object's CloneCount is being fetched without the clones being created first", L"ERROR: 'GetCloneCount() in Object.cpp'", MB_OK);

	return this->CloneCount;
}

void ObjectClass::SetCloneCount(int CloneCount)
{
	this->CloneCount = CloneCount;
}



DirectX::XMFLOAT4X4* ObjectClass::GetClonesWorldMatrixArray()
{
	return this->ClonesWorldMatrixArray;
}

void ObjectClass::SetCloneWorldMatrixArray(DirectX::XMFLOAT4X4 *CloneWorldMatrixArray)
{
	this->ClonesWorldMatrixArray = CloneWorldMatrixArray;
}


bool* ObjectClass::ModifyPickedArray(int TargetIndex)
{
	return &this->Picked_array[TargetIndex];
}







void ObjectClass::CreateAllClones(int ObjectCount, DirectX::XMFLOAT4X4 *WorldMatriceArray)
{
	this->CloneCount = ObjectCount;
	this->ClonesWorldMatrixArray = new DirectX::XMFLOAT4X4[this->CloneCount];
	this->ClonesHitBox_array = new HitBox[this->CloneCount];
	this->ClonesGreatestX_array = new float[this->CloneCount];
	this->ClonesLowestX_array = new float[this->CloneCount];
	this->ClonesGreatestY_array = new float[this->CloneCount];
	this->ClonesLowestY_array = new float[this->CloneCount];
	this->ClonesGreatestZ_array = new float[this->CloneCount];
	this->ClonesLowestZ_array = new float[this->CloneCount];

	// Picking-related
	this->Picked_array = new bool[this->CloneCount];

	for (int i = 0; i < this->CloneCount; i++) {
		this->ClonesWorldMatrixArray[i] = WorldMatriceArray[i];

		this->ClonesGreatestX_array[i] = this->TemplateGreatestX + this->ClonesWorldMatrixArray[i]._41;
		this->ClonesLowestX_array[i] = this->TemplateLowestX + this->ClonesWorldMatrixArray[i]._41;
		this->ClonesGreatestY_array[i] = this->TemplateGreatestY + this->ClonesWorldMatrixArray[i]._42;
		this->ClonesLowestY_array[i] = this->TemplateLowestY + this->ClonesWorldMatrixArray[i]._42;
		this->ClonesGreatestZ_array[i] = this->TemplateGreatestZ + this->ClonesWorldMatrixArray[i]._43;
		this->ClonesLowestZ_array[i] = this->TemplateLowestZ + this->ClonesWorldMatrixArray[i]._43;

		// Picking-related
		this->Picked_array[i] = false;


		// Left & Right planes
		this->ClonesHitBox_array[i].left.Normal = { -1, 0, 0 };
		this->ClonesHitBox_array[i].left.Distance = this->ClonesLowestX_array[i];

		this->ClonesHitBox_array[i].right.Normal = { 1, 0, 0 };
		this->ClonesHitBox_array[i].right.Distance = this->ClonesGreatestX_array[i];


		// Top & Bottom planes
		this->ClonesHitBox_array[i].top.Normal = { 0, 1, 0 };
		this->ClonesHitBox_array[i].top.Distance = this->ClonesGreatestY_array[i];

		this->ClonesHitBox_array[i].bottom.Normal = { 0, -1, 0 };
		this->ClonesHitBox_array[i].bottom.Distance = this->ClonesLowestY_array[i];


		// Near & Far planes.
		this->ClonesHitBox_array[i].near_.Normal = { 0, 0, -1 };
		this->ClonesHitBox_array[i].near_.Distance = this->ClonesLowestZ_array[i];

		this->ClonesHitBox_array[i].far_.Normal = { 0, 0, 1 };
		this->ClonesHitBox_array[i].far_.Distance = this->ClonesGreatestZ_array[i];
	}
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+