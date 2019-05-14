#include "QuadNodeClass.hpp"

QuadNodeClass::QuadNodeClass()
{
	for (int i = 0; i < 4; i++)
		this->ChildNodes[i] = nullptr;
}

QuadNodeClass::~QuadNodeClass()
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void QuadNodeClass::ReleaseAll()
{
	// If one of the childnodes is null, all of them are null.
	if (this->ChildNodes[0] != nullptr) {

		// Keep releasing until the 'if - case' above stops.
		for (int i = 0; i < 4; i++) {
			this->ChildNodes[i]->ReleaseAll();

			//// Release everything related to ObjectNodes
			//for (int i = 0; i < OBJ_COUNT; i++) {
			//	delete[] this->NodeObjects->CloneIDs;
			//	delete[] this->NodeObjects->GreatestCloneX_array;
			//	delete[] this->NodeObjects->LowestCloneX_array;
			//	delete[] this->NodeObjects->GreatestCloneZ_array;
			//	delete[] this->NodeObjects->LowestCloneZ_array;
			//}

	//		delete[] this->ChildNodeIDs;
			delete this->ChildNodes[i];
			this->ChildNodes[i] = nullptr;
		}

	}

}





QuadNodeClass *QuadNodeClass::FindGridSlot(int GridID)
{
	// Per ChildNode
	for (int i = 0; i < 4; i++) {

		// Per ID
		int Count = this->ChildNodes[i]->TotalChildNodeCount;
		
		if (Count == 1) {// If the current childnode only has 1 ChildNodeCount.
			if (this->ChildNodes[i]->GetNodeID() == GridID)
				return this->ChildNodes[i];
		}
		else {

			for (int j = 0; j < Count; j++) {
				if (this->ChildNodes[i]->ChildNodeIDs[j] == GridID)
					return this->ChildNodes[i]->FindGridSlot(GridID);
			}

		}
	
		
	
	}
}


int QuadNodeClass::TestLeafFrustumIntersection(Plane* AllFrustumPlanes)
{
	//	Frustum Planes: 
	//	0	Left
	//	1	Right
	//	2	Top
	//	3	Bottom
	//	4	Near
	//	5	Far
	
	//	0 --- 1
	//  |     |
	//  |     |
	//  2 --- 3
	DirectX::XMVECTOR ClosestToNormal = { 0, 0, 0 };
	DirectX::XMVECTOR CurrentDiagonal = { 0, 0, 0 };
	DirectX::XMVECTOR TempVector = { 0, 0, 0 };
	DirectX::XMFLOAT3 TempVectorF = { 0, 0, 0 };
	float CurrentDifference = 0;
	float ClosestDifference = 0;
	int ClosestDifferenceIndex = 0;
	int N_Index = -1;						// The corner which always has the lowest distance to the plane.
	DirectX::XMVECTOR Corners[8];
	Corners[0] = { this->West, this->Top, this->North };
	Corners[1] = { this->East, this->Top, this->North };
	Corners[2] = { this->West, this->Top, this->South };
	Corners[3] = { this->East, this->Top, this->South };
	Corners[4] = { this->West, this->Bot, this->North };
	Corners[5] = { this->East, this->Bot, this->North };
	Corners[6] = { this->West, this->Bot, this->South };
	Corners[7] = { this->East, this->Bot, this->South };


	// Per plane
	for (int i = 0; i < 4; i++) {

		ClosestToNormal = { 0, 0, 0 };
		ClosestDifference = FLT_MIN;

		// 0 - 7
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[0], Corners[7]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);	// Normal in XMVECTOR form.
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);	// Vector where are components are the dotproduct
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);				// Translate the above mentioned to XMFLOAT form
		CurrentDifference = TempVectorF.x;								// Fetch the dotproduct value out of one of the components.
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 7;
		}
		// 7 - 0
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[7], Corners[0]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		CurrentDifference = TempVectorF.x;
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 0;
		}




		// 3 - 4
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[3], Corners[4]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		CurrentDifference = TempVectorF.x;
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 4;
		}
		// 4 - 3
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[4], Corners[3]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		CurrentDifference = TempVectorF.x;
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 3;
		}




		// 2 - 5
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[2], Corners[5]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		CurrentDifference = TempVectorF.x;
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 5;
		}
		// 5 - 2
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[5], Corners[2]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		CurrentDifference = TempVectorF.x;
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 2;
		}




		// 1 - 6
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[1], Corners[6]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		CurrentDifference = TempVectorF.x;
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 6;
		}
		// 6 - 1
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[6], Corners[1]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		CurrentDifference = TempVectorF.x;
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 1;
		}

		//-----------------------------------------------------

		// Calculate the distance of the corner Corners[N_Index] to the plane, if it's positive the LeafNode does not
		// intersect the plane.
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(TempVector, Corners[N_Index]);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		float Distance = TempVectorF.x + AllFrustumPlanes[i].Distance;
		
		if (Distance > 0.05)	// To prevent floating point errors.
			return -1;

	}

	return this->NodeID;
}



int QuadNodeClass::TestCloneFrustumIntersection(
	Plane* AllFrustumPlanes,
	int TargetObject,
	int TargetClone
)
{
	//	Frustum Planes: 
	//	0	Left
	//	1	Right
	//	2	Top
	//	3	Bottom
	//	4	Near
	//	5	Far

	//	0 --- 1 
	//  |     | 
	//  |     | 
	//  2 --- 3 
	DirectX::XMVECTOR ClosestToNormal = { 0, 0, 0 };
	DirectX::XMVECTOR CurrentDiagonal = { 0, 0, 0 };
	DirectX::XMVECTOR TempVector = { 0, 0, 0 };
	DirectX::XMFLOAT3 TempVectorF = { 0, 0, 0 };
	float CurrentDifference = 0;
	float ClosestDifference = 0;
	int ClosestDifferenceIndex = 0;
	int N_Index = -1;						// The corner which always has the lowest distance to the plane.
	DirectX::XMVECTOR Corners[8];

	float North, South, Top, Bot, East, West;
	North = this->NodeObjects[TargetObject].GreatestCloneZ_array[TargetClone];
	South = this->NodeObjects[TargetObject].LowestCloneZ_array[TargetClone];
	Top = this->NodeObjects[TargetObject].GreatestCloneY_array[TargetClone];
	Bot = this->NodeObjects[TargetObject].LowestCloneY_array[TargetClone];
	West = this->NodeObjects[TargetObject].LowestCloneX_array[TargetClone];;
	East = this->NodeObjects[TargetObject].GreatestCloneX_array[TargetClone];;

	Corners[0] = { West, Top, North };
	Corners[1] = { East, Top, North };
	Corners[2] = { West, Top, South };
	Corners[3] = { East, Top, South };
	Corners[4] = { West, Bot, North };
	Corners[5] = { East, Bot, North };
	Corners[6] = { West, Bot, South };
	Corners[7] = { East, Bot, South };

	// Total Processs accepts leaves not unbased on top/bot planes, but not the objects.
	//
	// So we'll need to adapt the corners to make eight corners respectively.
	// 
	// 
	// 
	// 
	// 
	// 
	// 
	// 
	// 
	// 
	
	// 0,7
	// 3,4
	// 2,5
	// 1,6


	// Test per plane
	for (int i = 0; i < 6; i++) {

		ClosestToNormal = { 0, 0, 0 };
		ClosestDifference = FLT_MIN;

		// 0 - 7
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[0], Corners[7]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);	// Normal in XMVECTOR form.
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);	// Vector where are components are the dotproduct
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);				// Translate the above mentioned to XMFLOAT form
		CurrentDifference = TempVectorF.x;								// Fetch the dotproduct value out of one of the components.
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 7;
		}
		// 7 - 0
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[7], Corners[0]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);		
		CurrentDifference = TempVectorF.x;					
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 0;
		}




		// 3 - 4
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[3], Corners[4]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);			
		CurrentDifference = TempVectorF.x;				
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 4;
		}
		// 4 - 3
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[4], Corners[3]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);	
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);	
		CurrentDifference = TempVectorF.x;				
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 3;
		}




		// 2 - 5
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[2], Corners[5]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);	
		CurrentDifference = TempVectorF.x;				
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 5;
		}
		// 5 - 2
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[5], Corners[2]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);	
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);	
		CurrentDifference = TempVectorF.x;			
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 2;
		}




		// 1 - 6
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[1], Corners[6]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);	
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);	
		CurrentDifference = TempVectorF.x;	
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 6;
		}
		// 6 - 1
		CurrentDiagonal = DirectX::XMVectorSubtract(Corners[6], Corners[1]);
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(CurrentDiagonal, TempVector);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		CurrentDifference = TempVectorF.x;
		if (CurrentDifference > ClosestDifference) {
			ClosestDifference = CurrentDifference;
			N_Index = 1;
		}


		// Calculate the distance of the corner Corners[N_Index] to the plane, if it's positive the LeafNode does not
		// intersect the plane.
		TempVector = DirectX::XMLoadFloat3(&AllFrustumPlanes[i].Normal);
		TempVector = DirectX::XMVector3Dot(TempVector, Corners[N_Index]);
		DirectX::XMStoreFloat3(&TempVectorF, TempVector);
		float Distance = TempVectorF.x + AllFrustumPlanes[i].Distance;
		
		if (Distance > 0)
			return -1;

	}


	return this->NodeID;
}





void QuadNodeClass::Initialize(
	int ID_from,
	int ID_to,
	int Depth,
	float North,
	float South,
	float East,
	float West,
	float Top,
	float Bot
)
{
	
	//    _________
	//   | 0  |	1  |
	//   |----|----|
	//   | 2  |	3  |
	//    ииииииииии

	if (Depth > 0) {

		int* TempIDs = new int[this->TotalChildNodeCount / 4];
		int TempIndexHolder = 0;
		int From_array[4];	// Per childnode, what gridslots rely in that node?
		int To_array[4];	// Ex: Parent has 16. child0: 0to3, child1: 4to7, child2: 8to11, child3: 12to15
		for (int i = 0; i < 4; i++) {	// Ex: If parent has 16 and i = 2, child 2
			From_array[i] = ID_from + ((ID_to -ID_from) / 4)*i;			// From 16/4*2 = 8
			To_array[i] = ID_from + ((ID_to - ID_from) / 4)*(i + 1);	// To (16/4*3)-1 = 11

			this->ChildNodes[i] = new QuadNodeClass; // While we're here, Do everything that needs to be done per childnode.
			this->ChildNodes[i]->SetTotalChildNodeCount(this->TotalChildNodeCount / 4);	// Allocates memory for ChildNodeIDs
		}



		// ------------------ 0 ------------------
		TempIndexHolder = 0;
		for (int i = From_array[0]; i < To_array[0]; i++) {
			TempIDs[TempIndexHolder] = i;
			TempIndexHolder++;
		}

		// Set IDs
		if (this->TotalChildNodeCount == 4) {	// If we've reached groundzero, set the nodesID to the only ID available

			this->ChildNodes[0]->SetNodeID(TempIDs[0]);
	
		}
		else {									// If we still have childnodes, keep sending their IDs down.
			int *TempIDs_0 = new int[this->TotalChildNodeCount / 4];
			for (int i = 0; i < this->TotalChildNodeCount / 4; i++)
				TempIDs_0[i] = TempIDs[i];
			this->ChildNodes[0]->SetChildNodeIDs(TempIDs_0);
		}
		
		// Set Distances
		this->ChildNodes[0]->SetNorth(North);
		this->ChildNodes[0]->SetSouth((North + South) / 2);
		this->ChildNodes[0]->SetEast((West + East) / 2);
		this->ChildNodes[0]->SetWest(West);
		this->ChildNodes[0]->SetTop(Top);
		this->ChildNodes[0]->SetBot(Bot);

		this->ChildNodes[0]->Initialize(
			From_array[0],
			To_array[0],
			Depth - 1,
			North,
			(North + South) / 2,
			(West + East) / 2,
			West,
			Top,
			Bot
		);


		// ------------------ 1 ------------------

		TempIndexHolder = 0;
		for (int i = From_array[1]; i < To_array[1]; i++) {
			TempIDs[TempIndexHolder] = i;
			TempIndexHolder++;
		}

		// Set IDs
		if (this->TotalChildNodeCount == 4) {	// If we've reached groundzero, set the nodesID to the only ID available

			this->ChildNodes[1]->SetNodeID(TempIDs[0]);

		}
		else {									// If we still have childnodes, keep sending their IDs down.
			int *TempIDs_1 = new int[this->TotalChildNodeCount / 4];
			for (int i = 0; i < this->TotalChildNodeCount / 4; i++)
				TempIDs_1[i] = TempIDs[i];
			this->ChildNodes[1]->SetChildNodeIDs(TempIDs_1);
		}

		// Set Distances
		this->ChildNodes[1]->SetNorth(North);
		this->ChildNodes[1]->SetSouth((North + South) / 2);
		this->ChildNodes[1]->SetEast(East);
		this->ChildNodes[1]->SetWest((West + East) / 2);
		this->ChildNodes[1]->SetTop(Top);
		this->ChildNodes[1]->SetBot(Bot);

		this->ChildNodes[1]->Initialize(
			From_array[1],
			To_array[1],
			Depth - 1,
			North,
			(North + South) / 2,
			East,
			(West + East) / 2,
			Top,
			Bot
		);


		// ------------------ 2 ------------------

		TempIndexHolder = 0;
		for (int i = From_array[2]; i < To_array[2]; i++) {
			TempIDs[TempIndexHolder] = i;
			TempIndexHolder++;
		}

		// Set IDs
		if (this->TotalChildNodeCount == 4) {	// If we've reached groundzero, set the nodesID to the only ID available

			this->ChildNodes[2]->SetNodeID(TempIDs[0]);

		}
		else {									// If we still have childnodes, keep sending their IDs down.
			int *TempIDs_2 = new int[this->TotalChildNodeCount / 4];
			for (int i = 0; i < this->TotalChildNodeCount / 4; i++)
				TempIDs_2[i] = TempIDs[i];
			this->ChildNodes[2]->SetChildNodeIDs(TempIDs_2);
		}

		// Set Distances
		this->ChildNodes[2]->SetNorth((North + South) / 2);
		this->ChildNodes[2]->SetSouth(South);
		this->ChildNodes[2]->SetEast((West + East) / 2);
		this->ChildNodes[2]->SetWest(West);
		this->ChildNodes[2]->SetTop(Top);
		this->ChildNodes[2]->SetBot(Bot);

		this->ChildNodes[2]->Initialize(
			From_array[2],
			To_array[2],
			Depth - 1,
			(North + South) / 2,
			South,
			(West + East) / 2,
			West,
			Top,
			Bot
		);

		// ------------------ 3 ------------------

		TempIndexHolder = 0;
		for (int i = From_array[3]; i < To_array[3]; i++) {
			TempIDs[TempIndexHolder] = i;
			TempIndexHolder++;
		}

		// Set IDs
		if (this->TotalChildNodeCount == 4) {	// If we've reached groundzero, set the nodesID to the only ID available

			this->ChildNodes[3]->SetNodeID(TempIDs[0]);

		}
		else {									// If we still have childnodes, keep sending their IDs down.
			int *TempIDs_3 = new int[this->TotalChildNodeCount / 4];
			for (int i = 0; i < this->TotalChildNodeCount / 4; i++)
				TempIDs_3[i] = TempIDs[i];
			this->ChildNodes[3]->SetChildNodeIDs(TempIDs_3);
		}

		// Set Distances
		this->ChildNodes[3]->SetNorth((North + South) / 2);
		this->ChildNodes[3]->SetSouth(South);
		this->ChildNodes[3]->SetEast(East);
		this->ChildNodes[3]->SetWest((West + East) / 2);
		this->ChildNodes[3]->SetTop(Top);
		this->ChildNodes[3]->SetBot(Bot);


		this->ChildNodes[3]->Initialize(
			From_array[3],
			To_array[3],
			Depth - 1,
			(North + South) / 2,
			South,
			East,
			(West + East) / 2,
			Top,
			Bot
		);
	}
}





void QuadNodeClass::AssignObjects(
	NodeObject* RootNodeObjects,
	NodeObject* ParentsNodeObjects,
	int nr_of_layers
)
{
	//    _________							 (west+east)/2		  Z	^ 
	//   | 0  |	1  |							   |				|
	//   |----|----|           (north+south)/2 ----|----			-----> X
	//   | 2  |	3  |                               |
	//    иииииииии

	/*
	This function first finds out how many clones there are in each object for all the childnodes.
	It then splits the parents(this) clones among the childnodes based on their position and the grid of each childnode.

	It continues recursively according to nr_of_layers.

	An object is identified by its "ID", which is the clone's CloneWorldMatrix index in the original
	AllObjects(array) which contains all the objects and all the clones.
	*/

	
	if (nr_of_layers > 0) {

		int TotalCloneCount = 0;									// All of the temporary ChildNodeX_etc arrays
		for (int i = 0; i < OBJ_COUNT; i++) {						// needs to be the size of the total clonecount, 
			TotalCloneCount += ParentsNodeObjects[i].CloneCount;	// in case all of the objects were to reside in the
		}															// same childnode.

		int** ChildNode_ObjectClone_Counts;
		ChildNode_ObjectClone_Counts = new int*[4];
		for (int i = 0; i < 4; i++) {
			ChildNode_ObjectClone_Counts[i] = new int[OBJ_COUNT];
		}

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < OBJ_COUNT; j++)
				ChildNode_ObjectClone_Counts[i][j] = 0;

		/*
		This node has 4 child nodes.
		Every child node has 3(OBJ_COUNT) objects in it.
		every object has a maximum of 60(TotalCloneCount) clones in it.

		Every clone has its own ID.
		*/

		int*** IDgrid;
		IDgrid = new int**[4];	// Four child nodes

		for (int i = 0; i < 4; i++) {

			IDgrid[i] = new int*[OBJ_COUNT];	// Three unique objects.

			for (int j = 0; j < OBJ_COUNT; j++) {
				IDgrid[i][j] = new int[TotalCloneCount];	// Maximum of (currently) 60 Clones.
			}
		}

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < OBJ_COUNT; j++)
				for (int k = 0; k < TotalCloneCount; k++)
					IDgrid[i][j][k] = -1;




		// Per object.
		for (int i = 0; i < OBJ_COUNT; i++) {

			// Per clone.
			int CurrentObjectCloneCount = ParentsNodeObjects[i].CloneCount;
			for (int j = 0; j < CurrentObjectCloneCount; j++) {
				

				// Handle 0 
				if (ParentsNodeObjects[i].LowestCloneX_array[j] < (this->West + this->East) / 2) {		// The point furthest to the west is to the left of the west/east-split.
					if (ParentsNodeObjects[i].GreatestCloneZ_array[j] > (this->North + this->South) / 2) {	// The point furthest to the north is above the north/south-split.
						/*
						So we've found that the object at hand resides (at least partially) inside the node_0.

						Now we need to make sure that the clone's ID and WorldMatrix gets added to node_0

						Also we need to increase node_0's clone count.
						*/

						IDgrid[0][i][ChildNode_ObjectClone_Counts[0][i]] = ParentsNodeObjects[i].CloneIDs[j];
						ChildNode_ObjectClone_Counts[0][i]++;
					}
				}


				// Handle 1 
				if (ParentsNodeObjects[i].GreatestCloneX_array[j] > (this->West + this->East) / 2) {	// The point furthest to the east is to the right of the west/east-split.
					if (ParentsNodeObjects[i].GreatestCloneZ_array[j] > (this->North + this->South) / 2) {	// The point furthest to the north is above the north/south-split.


						IDgrid[1][i][ChildNode_ObjectClone_Counts[1][i]] = ParentsNodeObjects[i].CloneIDs[j];
						ChildNode_ObjectClone_Counts[1][i]++;
					}
				}


				// Handle 2 
				if (ParentsNodeObjects[i].LowestCloneX_array[j] < (this->West + this->East) / 2) {		// The point furthest to the west is to the left of the west/east-split.
					if (ParentsNodeObjects[i].LowestCloneZ_array[j] < (this->North + this->South) / 2) {	// The point furthest to the south is below the north/south-split.


						IDgrid[2][i][ChildNode_ObjectClone_Counts[2][i]] = ParentsNodeObjects[i].CloneIDs[j];
						ChildNode_ObjectClone_Counts[2][i]++;
					}
				}


				// Handle 3 
				if (ParentsNodeObjects[i].GreatestCloneX_array[j] > (this->West + this->East) / 2) {	// The point furthest to the east is to the right of the west/east-split.
					if (ParentsNodeObjects[i].LowestCloneZ_array[j] < (this->North + this->South) / 2) {	// The point furthest to the south is below the north/south-split.


						IDgrid[3][i][ChildNode_ObjectClone_Counts[3][i]] = ParentsNodeObjects[i].CloneIDs[j];
						ChildNode_ObjectClone_Counts[3][i]++;
					}
				}
			}
		}


		for (int i = 0; i < 4; i++) {	// Per ChildNode


			for (int j = 0; j < OBJ_COUNT; j++) {	// Per NodeObject.
				/*
				Each NodeObject contains CloneCount, an array of CloneIDs, and an array of CloneWorldMatrices.

				So per NodeObject we need to fill the Clonecount and use it to allocate the arrays of CloneIDs and CloneWorldMatrices

				And per Clone we need to fill the clone's ID and CloneWorldMatrix.
				*/

				// Fill CloneCount
				this->ChildNodes[i]->GetNodeObjects()[j].CloneCount = ChildNode_ObjectClone_Counts[i][j];

				// Allocate the ararys.
				if (this->ChildNodes[i]->GetNodeObjects()[j].CloneCount != 0) {



					this->ChildNodes[i]->GetNodeObjects()[j].CloneIDs = new int[this->ChildNodes[i]->GetNodeObjects()[j].CloneCount];

					this->ChildNodes[i]->GetNodeObjects()[j].GreatestCloneX_array = new float[this->ChildNodes[i]->GetNodeObjects()[j].CloneCount];
					this->ChildNodes[i]->GetNodeObjects()[j].LowestCloneX_array = new float[this->ChildNodes[i]->GetNodeObjects()[j].CloneCount];

					this->ChildNodes[i]->GetNodeObjects()[j].GreatestCloneY_array = new float[this->ChildNodes[i]->GetNodeObjects()[j].CloneCount];
					this->ChildNodes[i]->GetNodeObjects()[j].LowestCloneY_array = new float[this->ChildNodes[i]->GetNodeObjects()[j].CloneCount];

					this->ChildNodes[i]->GetNodeObjects()[j].GreatestCloneZ_array = new float[this->ChildNodes[i]->GetNodeObjects()[j].CloneCount];
					this->ChildNodes[i]->GetNodeObjects()[j].LowestCloneZ_array = new float[this->ChildNodes[i]->GetNodeObjects()[j].CloneCount];




					for (int k = 0; k < ChildNode_ObjectClone_Counts[i][j]; k++) {	// Per Clone.
						this->ChildNodes[i]->GetNodeObjects()[j].CloneIDs[k] = IDgrid[i][j][k];

						int ID = this->ChildNodes[i]->GetNodeObjects()[j].CloneIDs[k];

						// ChildNode nr:i, NodeObject nr:j, and index of the array:k  - Fetch the value from NodeObject nr:j with the index:ID.
						this->ChildNodes[i]->GetNodeObjects()[j].GreatestCloneX_array[k] = RootNodeObjects[j].GreatestCloneX_array[ID];
						this->ChildNodes[i]->GetNodeObjects()[j].LowestCloneX_array[k] = RootNodeObjects[j].LowestCloneX_array[ID];
						this->ChildNodes[i]->GetNodeObjects()[j].GreatestCloneY_array[k] = RootNodeObjects[j].GreatestCloneY_array[ID];
						this->ChildNodes[i]->GetNodeObjects()[j].LowestCloneY_array[k] = RootNodeObjects[j].LowestCloneY_array[ID];
						this->ChildNodes[i]->GetNodeObjects()[j].GreatestCloneZ_array[k] = RootNodeObjects[j].GreatestCloneZ_array[ID];
						this->ChildNodes[i]->GetNodeObjects()[j].LowestCloneZ_array[k] = RootNodeObjects[j].LowestCloneZ_array[ID];

					}
				}
				else {
					this->ChildNodes[i]->GetNodeObjects()[j].CloneIDs = nullptr;
					this->ChildNodes[i]->GetNodeObjects()[j].GreatestCloneX_array = nullptr;
					this->ChildNodes[i]->GetNodeObjects()[j].LowestCloneX_array = nullptr;
					this->ChildNodes[i]->GetNodeObjects()[j].GreatestCloneY_array = nullptr;
					this->ChildNodes[i]->GetNodeObjects()[j].LowestCloneY_array = nullptr;
					this->ChildNodes[i]->GetNodeObjects()[j].GreatestCloneZ_array = nullptr;
					this->ChildNodes[i]->GetNodeObjects()[j].LowestCloneZ_array = nullptr;
				}
			}

		}
		
		for (int i = 0; i < 4; i++) {
			this->ChildNodes[i]->AssignObjects(
				RootNodeObjects,
				this->ChildNodes[i]->GetNodeObjects(),
				(nr_of_layers - 1)			
			);
		}


	}
}





int QuadNodeClass::CornerDive(int nr_of_layers, float x, float z)
{

	
	//    _________							 (west+east)/2		  Z	^ 
	//   | 0  |	1  |							   |				|
	//   |----|----|           (north+south)/2 ----|----			-----> X
	//   | 2  |	3  |                               |
	//    иииииииии

	// 
	//
	//
	//

	if (nr_of_layers > 0) {

		if (nr_of_layers == 1) {
			if ((x < this->West || x > this->East) || (z > this->North || z < this->South))
				return -1;
		}


		// Handle 0
		if (x <= (this->West + this->East) / 2) {
			if (z >= (this->South + this->North) / 2) {

				return this->ChildNodes[0]->CornerDive(nr_of_layers - 1, x, z);


			}
		}


		// Handle 1
		if (x >= (this->West + this->East) / 2) {
			if (z >= (this->South + this->North) / 2) {

				return this->ChildNodes[1]->CornerDive(nr_of_layers - 1, x, z);

			}
		}


		// Handle 2
		if (x <= (this->West + this->East) / 2) {
			if (z <= (this->South + this->North) / 2) {

				return this->ChildNodes[2]->CornerDive(nr_of_layers - 1, x, z);

			}
		}


		// Handle 3
		if (x >= (this->West + this->East) / 2) {
			if (z <= (this->South + this->North) / 2) {

				return this->ChildNodes[3]->CornerDive(nr_of_layers - 1, x, z);

			}
		}
	}
	else { 
		return this->NodeID;
	}

}


void QuadNodeClass::SetTotalChildNodeCount(int Count)
{
	this->TotalChildNodeCount = Count;

	if (this->ChildNodeIDs == nullptr) {
		this->ChildNodeIDs = new int[this->TotalChildNodeCount];
	}
	else {
		MessageBox(NULL, L"TotalChildNodeCount tried to be set twice", L"QuadNodeClass::SetTotalChildNodeCount(), in QuadNodeClass.cpp", MB_OK);
	}
}

void QuadNodeClass::SetChildNodeIDs(int* ChildNodeIDs)
{
	this->ChildNodeIDs = ChildNodeIDs;
}

void QuadNodeClass::SetNodeID(int NodeID)
{
	this->NodeID = NodeID;
}

void QuadNodeClass::SetNorth(float North)
{
	this->North = North;
}

void QuadNodeClass::SetSouth(float South)
{
	this->South = South;
}
void QuadNodeClass::SetEast(float East)
{
	this->East = East;
}
void QuadNodeClass::SetWest(float West)
{
	this->West = West;
}
void QuadNodeClass::SetTop(float Top)
{
	this->Top = Top;
}
void QuadNodeClass::SetBot(float Bot)
{
	this->Bot = Bot;
}

int QuadNodeClass::GetTotalChildNodeCount()
{
	return this->TotalChildNodeCount;
}

int* QuadNodeClass::GetChildNodeIDs()
{
	return this->ChildNodeIDs;
}
int QuadNodeClass::GetNodeID()
{
	return this->NodeID;
}

float QuadNodeClass::GetNorth()
{
	return this->North;
}

float QuadNodeClass::GetSouth()
{
	return this->South;
}
float QuadNodeClass::GetEast()
{
	return this->East;
}
float QuadNodeClass::GetWest()
{
	return this->West;
}
float QuadNodeClass::GetTop()
{
	return this->Top;
}
float QuadNodeClass::GetBot()
{
	return this->Bot;
}



NodeObject* QuadNodeClass::GetNodeObjects()
{
	return this->NodeObjects;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+