#ifndef QUADNODECLASS_HPP
#define QUADNODECLASS_HPP

#include "ObjectHandlerClass.hpp"	// Gives objectClass and OBJ_COUNT

#include <DirectXMath.h>

struct NodeObject { //+-+-+-+-+-+-+-+-+-+-+-+-+-+-
												//
	int CloneCount = 0;							//
	int *CloneIDs;								//
	float *GreatestCloneX_array;				//
	float *LowestCloneX_array;					//
	float *GreatestCloneY_array;				//
	float *LowestCloneY_array;					//
	float *GreatestCloneZ_array;				//
	float *LowestCloneZ_array;					//
												//
}; // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

class QuadNodeClass {
private:
	// The class can't hold an array of itself, cus it would infinitely expand.
	// however it can hold a pointer to an array of itself, so that when those get
	// allocated they don't automatically reproduce infinitely.
	QuadNodeClass *ChildNodes[4];	
	int TotalChildNodeCount = -1;		
	int *ChildNodeIDs = nullptr;

	int NodeID = -1;							
	float North, South, East, West, Top, Bot;
	NodeObject NodeObjects[OBJ_COUNT];




public:
	QuadNodeClass();
	~QuadNodeClass();
	void ReleaseAll();
	 

	QuadNodeClass *FindGridSlot(int GridID);

	/* ------------- COMMENTS -------------
	Returns NodeID if it does intersect, otherwise it returns -1.
	*/
	int TestLeafFrustumIntersection(Plane* AllFrustumPlanes);
	
	/* ------------- COMMENTS -------------
	Returns an array of all the cloneIDs (Of targetObject) which rely inside the frustum.
	*/
	int TestCloneFrustumIntersection(
		Plane* AllFrustumPlanes,
		int TargetObject,
		int TargetClone
	);



	/* ------------- COMMENTS -------------
	Assigns all nodes with NorthSouthEastWest, and assign the 
	final leaf-nodes IDs.
	*/
	void Initialize(
		int ID_from,
		int ID_to,
		int Depth,
		float North,
		float South,
		float East,
		float West,
		float Top,
		float Bot
	);



	void AssignObjects(
		NodeObject* RootNodeObjects,
		NodeObject* ParentsNodeObjects,
		int nr_of_layers);

	/* ------------- COMMENTS -------------
	Checks which final childnode the Corner X,Z resides in, and returns the index of that node.
	*/
	int CornerDive(int nr_of_layers, float x, float z);

	/* ------------- COMMENTS -------------
	Also allocates ChildNodeIDs array according to size, if it's not already allocated.
	*/
	void SetTotalChildNodeCount(int Count);
	void SetChildNodeIDs(int* ChildNodeIDs);
	void SetNodeID(int ID);
	void SetNorth(float North);
	void SetSouth(float South);
	void SetEast(float East);
	void SetWest(float West);
	void SetTop(float Top);
	void SetBot(float Bot);

	int GetTotalChildNodeCount();
	int* GetChildNodeIDs();
	int GetNodeID();
	float GetNorth();
	float GetSouth();
	float GetEast();
	float GetWest();
	float GetTop();
	float GetBot();

	NodeObject* GetNodeObjects();
	




};


#endif