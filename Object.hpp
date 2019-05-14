#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <DirectXMath.h>
#include <string>

#include <d3d11.h>	// Needed for MessageBox()

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									NECESSARY STRUCTS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct Plane { //-+-+-+-+-+-+-+-+-
	DirectX::XMFLOAT3 Normal;	//
	float Distance;				//
}; //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

struct HitBox { //-+-+-+-+-+-+-+-+
	Plane left;					//
	Plane right;				//
	Plane top;					//
	Plane bottom;				//
	Plane near_;				//
	Plane far_;					//-+-+
									//
	DirectX::XMFLOAT3 TopLeftFar;	//
	DirectX::XMFLOAT3 TopRightFar;	//
	DirectX::XMFLOAT3 TopLeftNear;	//
	DirectX::XMFLOAT3 TopRightNear;	//
	DirectX::XMFLOAT3 BotLeftFar;	//
	DirectX::XMFLOAT3 BotRightFar;	//
	DirectX::XMFLOAT3 BotLeftNear;	//
	DirectX::XMFLOAT3 BotRightNear;	//
};	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



/* ------------- COMMENTS -------------
This is the struct that is sent to the shaders
*/
struct VertexDataClass	//-+-+-+-+-+-+
{									//				
	DirectX::XMFLOAT3 position;		//
	DirectX::XMFLOAT2 texCoord;		//
	DirectX::XMFLOAT3 normal;		//
};	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/* ------------- COMMENTS -------------
Struct that contains general information per object,
used in a bunch of different functions.
*/
struct ObjectDataClass
{
	unsigned long *indices;
	int vertexCount;
	int texCoordCount;
	int normalCount;
	int faceCount;

	std::string materialFileName;
	std::string materialName;
	std::wstring textureFileName;
	int illumValue;
	DirectX::XMFLOAT3 KdValues;
	DirectX::XMFLOAT3 KaValues;
	DirectX::XMFLOAT3 TfValues;
	float NiValue;
};
//---------------------------------------------------------------------


class ObjectClass {
private:
	VertexDataClass		*VerticeData;				// Gets initialized by ObjectLoader.
	ObjectDataClass		ObjectData;					// 
	float TemplateGreatestX, TemplateLowestX;		// 
	float TemplateGreatestY, TemplateLowestY;		// 
	float TemplateGreatestZ, TemplateLowestZ;		// 
	

	std::string			FileName;					// Gets initialized by ObjectManager.

	int CloneCount;									// Gets initialized by SetAllObjectData.
	DirectX::XMFLOAT4X4 *ClonesWorldMatrixArray;	//
	HitBox* ClonesHitBox_array;						//
	float* ClonesGreatestX_array;					//
	float* ClonesLowestX_array;						//
	float* ClonesGreatestY_array;					//
	float* ClonesLowestY_array;						//
	float* ClonesGreatestZ_array;					//
	float* ClonesLowestZ_array;						//
	
	bool* Picked_array = false;



public:
	ObjectClass();
	~ObjectClass();
	void ReleaseAll();

	VertexDataClass*	*getVerticeData();
	ObjectDataClass		*getObjectData();
	std::string			*getFileName();

	void SetGreatestX(float GreatestX);
	void SetLowestX(float LowestX);
	void SetGreatestY(float GreatestY);
	void SetLowestY(float LowestY);
	void SetGreatestZ(float GreatestZ);
	void SetLowestZ(float LowestZ);
	float* GetGreatestX_array();
	float* GetLowestX_array();
	float* GetGreatestY_array();
	float* GetLowestY_array();
	float* GetGreatestZ_array();
	float* GetLowestZ_array();

	float GetGreatestY();
	float GetLowestY();

	bool* ModifyPickedArray(int TargetIndex);

	int GetCloneCount();
	void SetCloneCount(int CloneCount);	// Used for assigning objects to grid in QuadTreeClass.

	DirectX::XMFLOAT4X4* GetClonesWorldMatrixArray();
	void SetCloneWorldMatrixArray(DirectX::XMFLOAT4X4 *CloneWorldMatrixArray);

	/* ------------- COMMENTS -------------
	Sets both input parameters to internal CloneCount & CloneWorldMatrixArray
	*/
	void CreateAllClones(int ObjectCount, DirectX::XMFLOAT4X4 *WorldMatrixArray);




};




#endif