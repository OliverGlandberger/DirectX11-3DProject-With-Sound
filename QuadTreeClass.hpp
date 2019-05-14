#ifndef QUADTREECLASS_HPP
#define QUADTREECLASS_HPP

#include "QuadNodeClass.hpp"

#include "ObjectHandlerClass.hpp"	// Brings ObjectClass & OBJ_COUNT

#define LAYERS 4

struct HoverCamStruct { //-+-+-+-+-+-+-+-+-+-+

	DirectX::XMVECTOR CameraPosition;		//
	DirectX::XMVECTOR CameraDirection;		//
	DirectX::XMVECTOR CameraUpDirection;	//

	DirectX::XMMATRIX View;					//
	DirectX::XMMATRIX Projection;			//

};	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class QuadTreeClass {
private:

	QuadNodeClass *RootNode;

	float GridMaxX, GridMinX;
	float GridMaxY, GridMinY;
	float GridMaxZ, GridMinZ;

	int PassedCloneCounts[OBJ_COUNT];
	int* PassedCloneIDs[OBJ_COUNT];	// Grid; Gets initialized in AssignObjectsToGrid

	HoverCamStruct HoverCam;


public:
	QuadTreeClass();
	~QuadTreeClass();
	void ReleaseQuadTree();

	/* ------------- COMMENTS -------------
	Needs to be called BEFORE SetPassedCloneCount
	*/
	void SetPassedCloneCount(int ObjectIndex, int Count);
	/* ------------- COMMENTS -------------
	Needs to be called AFTER SetPassedCloneCount
	*/
	void SetPassedCloneIDs(int ObjectIndex, int* ArrayWithPassedCloneIDs);
	int* GetPassedCloneCounts();
	int**GetPassedCloneIDs();

	float GetWest();
	float GetEast();
	float GetNorth();
	float GetSouth();
	float GetTop();
	float GetBot();

	QuadNodeClass *GetRootNode();
	
	QuadNodeClass *FindGridSlot(int GridID);

	/* ------------- COMMENTS -------------
	Builds the tree-node-structure based on the nr_of_layers parameter.
	Defines the size of each node (Except the RootNode) in the tree-node-structure,
	based on North,South,East,West.
	*/
	void InitializeNodes();

	/* ------------- COMMENTS -------------
	Defines the Grid size of the RootNode based on North,South,East,West.
	*/
	void InitializeGrid(ObjectClass *AllObjects);

	/* ------------- COMMENTS -------------
	Intializes the NodeObjects in the RootNode based on the AllObjects array.
	The cache is this functions slave :)
	*/
	void AssignObjectsToNodes(ObjectClass *AllObjects);

	/* ------------- COMMENTS -------------
	*/
	void TestFrustumIntersection(Plane* AllFrustumPlanes);

	/* ------------- COMMENTS -------------
	*/
	int CornerDive(DirectX::XMVECTOR FrustumCorner);


	void InitializeHoverCam();
	DirectX::XMMATRIX GetHoverCamView();
	DirectX::XMMATRIX GetHoverCamProjection();


};





#endif