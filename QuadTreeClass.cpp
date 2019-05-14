#include "QuadTreeClass.hpp"

QuadTreeClass::QuadTreeClass()
{
	this->RootNode = nullptr;

	// delete this when you see it
	//this->InitializeQuadTree(3);
}

QuadTreeClass::~QuadTreeClass()
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void QuadTreeClass::ReleaseQuadTree()
{
	this->RootNode->ReleaseAll();
}


void QuadTreeClass::SetPassedCloneCount(int ObjectIndex, int Count)
{
	if (ObjectIndex > -1 && ObjectIndex < OBJ_COUNT)
		this->PassedCloneCounts[ObjectIndex] = Count;
	else 
		MessageBox(NULL, L"GIVEN ObjectIndex OUT OF BOUNDS; BOO.", L"QuadTreeClass::SetPassedCloneCount(), in QuadTreeClass.cpp", MB_OK);
}
void QuadTreeClass::SetPassedCloneIDs(int ObjectIndex, int* ArrayWithPassedCloneIDs)
{
	for (int i = 0; i < this->PassedCloneCounts[ObjectIndex]; i++) {
		this->PassedCloneIDs[ObjectIndex][i] = ArrayWithPassedCloneIDs[i];
	}
}

int* QuadTreeClass::GetPassedCloneCounts()
{
	return this->PassedCloneCounts;
}

int** QuadTreeClass::GetPassedCloneIDs()
{
	return this->PassedCloneIDs;
}


float QuadTreeClass::GetWest()
{
	return this->GridMinX;
}
float QuadTreeClass::GetEast()
{
	return this->GridMaxX;
}
float QuadTreeClass::GetNorth()
{
	return this->GridMaxZ;
}
float QuadTreeClass::GetSouth()
{
	return this->GridMinZ;
}
float QuadTreeClass::GetTop()
{
	return this->GridMaxY;
}
float QuadTreeClass::GetBot()
{
	return this->GridMinY;
}



QuadNodeClass *QuadTreeClass::GetRootNode()
{
	return this->RootNode;
}

QuadNodeClass *QuadTreeClass::FindGridSlot(int GridID)
{
	if (GridID > -1 && GridID < pow(4, LAYERS - 1)) {
		return this->RootNode->FindGridSlot(GridID);
	}
	else
		MessageBox(NULL, L"REQUESTED GRIDID OUT OF BOUNDS; BOO.", L"QuadTreeClass::FindGridSlot(), in QuadTreeClass.cpp", MB_OK);
	return nullptr;
}



void QuadTreeClass::InitializeNodes()
{
	int nr_of_layers = LAYERS;

	if (nr_of_layers > 0) {

		this->RootNode = new QuadNodeClass;
		
		this->RootNode->SetNorth(GridMaxZ);
		this->RootNode->SetSouth(GridMinZ);
		this->RootNode->SetTop(GridMaxY);
		this->RootNode->SetBot(GridMinY);
		this->RootNode->SetEast(GridMaxX);
		this->RootNode->SetWest(GridMinX);



		int TotalChildNodeCount = pow(4, LAYERS - 1);
		this->RootNode->SetTotalChildNodeCount(TotalChildNodeCount);
		

		// Create the IDs for all of the child nodes.
		int* temp_AllChildNodeIDs = new int[TotalChildNodeCount];
		for (int i = 0; i < TotalChildNodeCount; i++) {
			temp_AllChildNodeIDs[i] = i;
		}
		this->RootNode->SetChildNodeIDs(temp_AllChildNodeIDs);


		this->RootNode->Initialize(
			0,
			pow(4, LAYERS - 1),
			nr_of_layers - 1,
			this->RootNode->GetNorth(),
			this->RootNode->GetSouth(),
			this->RootNode->GetEast(),
			this->RootNode->GetWest(),
			this->RootNode->GetTop(),
			this->RootNode->GetBot()
		);

		delete[] temp_AllChildNodeIDs;
	}
}



void QuadTreeClass::InitializeGrid(ObjectClass *AllObjects)
{
	//// Make sure that the first values given will be accepted.
	this->GridMaxX = FLT_MIN, this->GridMinX = FLT_MAX;
	this->GridMaxY = FLT_MIN, this->GridMinY = FLT_MAX;
	this->GridMaxZ = FLT_MIN, this->GridMinZ = FLT_MAX;
	

	// Per Object Template.
	for (int i = 0; i < OBJ_COUNT; i++) {


		// Per Object Clone.
		int loopCount = AllObjects[i].GetCloneCount();
		for (int j = 0; j < loopCount; j++) {


			if (AllObjects[i].GetGreatestX_array()[j] > this->GridMaxX)
				this->GridMaxX = AllObjects[i].GetGreatestX_array()[j];

			if (AllObjects[i].GetLowestX_array()[j] < this->GridMinX)
				this->GridMinX = AllObjects[i].GetLowestX_array()[j];


			if (AllObjects[i].GetGreatestY_array()[j] > this->GridMaxY)
				this->GridMaxY = AllObjects[i].GetGreatestY_array()[j];

			if (AllObjects[i].GetLowestY_array()[j] < this->GridMinY)
				this->GridMinY = AllObjects[i].GetLowestY_array()[j];


			if (AllObjects[i].GetGreatestZ_array()[j] > this->GridMaxZ)
				this->GridMaxZ = AllObjects[i].GetGreatestZ_array()[j];

			if (AllObjects[i].GetLowestZ_array()[j] < this->GridMinZ)
				this->GridMinZ = AllObjects[i].GetLowestZ_array()[j];


		}
	}

}



void QuadTreeClass::AssignObjectsToNodes(ObjectClass *AllObjects)
{
	// Initialize CulledCloneIDs because here's the first place we know the Total amount of clones
	int TotalCloneCount = 0;
	for (int i = 0; i < OBJ_COUNT; i++)
		TotalCloneCount += AllObjects[i].GetCloneCount();
	for (int i = 0; i < OBJ_COUNT; i++)
		this->PassedCloneIDs[i] = new int[TotalCloneCount];

	// Per Object
	for (int i = 0; i < OBJ_COUNT; i++) {
		
		this->RootNode->GetNodeObjects()[i].CloneCount = AllObjects[i].GetCloneCount();
		
		// Per Clone
		int CurrentObjectCloneCount = this->RootNode->GetNodeObjects()[i].CloneCount;

		this->RootNode->GetNodeObjects()[i].CloneIDs = new int[CurrentObjectCloneCount];
		this->RootNode->GetNodeObjects()[i].GreatestCloneX_array = new float[CurrentObjectCloneCount];
		this->RootNode->GetNodeObjects()[i].LowestCloneX_array = new float[CurrentObjectCloneCount];
		this->RootNode->GetNodeObjects()[i].GreatestCloneY_array = new float[CurrentObjectCloneCount];
		this->RootNode->GetNodeObjects()[i].LowestCloneY_array = new float[CurrentObjectCloneCount];
		this->RootNode->GetNodeObjects()[i].GreatestCloneZ_array = new float[CurrentObjectCloneCount];
		this->RootNode->GetNodeObjects()[i].LowestCloneZ_array = new float[CurrentObjectCloneCount];

		for (int j = 0; j < CurrentObjectCloneCount; j++) {

			// Give them an ID which will be used to fetch them later.
			this->RootNode->GetNodeObjects()[i].CloneIDs[j] = j;


			// Assign each clone with the corresponding Greatest/Lowest X/Y.
			this->RootNode->GetNodeObjects()[i].GreatestCloneX_array[j] = AllObjects[i].GetGreatestX_array()[j];
			this->RootNode->GetNodeObjects()[i].LowestCloneX_array[j] = AllObjects[i].GetLowestX_array()[j];

			this->RootNode->GetNodeObjects()[i].GreatestCloneY_array[j] = AllObjects[i].GetGreatestY_array()[j];
			this->RootNode->GetNodeObjects()[i].LowestCloneY_array[j] = AllObjects[i].GetLowestY_array()[j];

			this->RootNode->GetNodeObjects()[i].GreatestCloneZ_array[j] = AllObjects[i].GetGreatestZ_array()[j];
			this->RootNode->GetNodeObjects()[i].LowestCloneZ_array[j] = AllObjects[i].GetLowestZ_array()[j];
			

		}
	}

	// Recursively assign the objects to the tree.
	this->RootNode->AssignObjects(
		this->RootNode->GetNodeObjects(),
		this->RootNode->GetNodeObjects(),
		(LAYERS - 1)
	);
}



void QuadTreeClass::TestFrustumIntersection(Plane* AllFrustumPlanes)
{


	int TotalObjectCount = 0;
	for (int i = 0; i < OBJ_COUNT; i++)
		TotalObjectCount += this->RootNode->GetNodeObjects()[i].CloneCount;

	// 3xn-array which contains the ID of the passed clones per object.
	int** Passed_Obj_CloneIDs = new int*[OBJ_COUNT];
	for (int i = 0; i < OBJ_COUNT; i++) {

		Passed_Obj_CloneIDs[i] = new int[TotalObjectCount];
		for (int j = 0; j < TotalObjectCount; j++)
			Passed_Obj_CloneIDs[i][j] = -1;

	}

	// An array containing the total number of approved clones per object.
	int* Passed_Obj_CloneCounts = new int[OBJ_COUNT];
	for (int i = 0; i < OBJ_COUNT; i++)
		Passed_Obj_CloneCounts[i] = 0;

	// Per GridSlot
	for (int i = 0; i < pow(4, LAYERS - 1); i++) {

		QuadNodeClass *CurrentGridSlot;

		CurrentGridSlot = this->FindGridSlot(i);
		if (CurrentGridSlot != nullptr) {	// If CurrentGridSlot has been found.
			if (CurrentGridSlot->TestLeafFrustumIntersection(AllFrustumPlanes) > -1) {// The grid intersects the frustum.

				/*
				Proceed to test clones in this grid
				*/

				// Per object
				for (int j = 0; j < OBJ_COUNT; j++) {

					int CloneCount = CurrentGridSlot->GetNodeObjects()[j].CloneCount;

					// Per clone
					for (int k = 0; k < CloneCount; k++) {

						// Test clone against frustum.
						if (CurrentGridSlot->TestCloneFrustumIntersection(AllFrustumPlanes, j, k) != -1) {
							Passed_Obj_CloneIDs[j][Passed_Obj_CloneCounts[j]] = CurrentGridSlot->GetNodeObjects()[j].CloneIDs[k];
							Passed_Obj_CloneCounts[j]++;
						}

					}

 				}




			}
		}
	}


	// Fill QuadTreeClass's CulledCloneCount & CulledCloneIDs
	for (int i = 0; i < OBJ_COUNT; i++) {

		this->PassedCloneCounts[i] = Passed_Obj_CloneCounts[i];

		for (int j = 0; j < this->PassedCloneCounts[i]; j++) {

			this->PassedCloneIDs[i][j] = Passed_Obj_CloneIDs[i][j];

		}

	}

}


int QuadTreeClass::CornerDive(DirectX::XMVECTOR FrustumCorner)
{
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, FrustumCorner);
	float x = temp.x;
	float z = temp.z;

	return this->RootNode->CornerDive(LAYERS - 1, x, z);
}



void QuadTreeClass::InitializeHoverCam()
{
	float MiddleX = (this->GridMinX + GridMaxX) / 2;
	float HeightY = this->GridMaxY * 1.2;	// So it's a little above the grid.
	float MiddleZ = (this->GridMaxZ + this->GridMinZ) / 2;;

	this->HoverCam.CameraPosition = { MiddleX, HeightY, MiddleZ };
	this->HoverCam.CameraDirection = { 0, -1, 0 };
	this->HoverCam.CameraUpDirection = { 0, 0, 1 };


	this->HoverCam.View = DirectX::XMMatrixLookToLH(
		this->HoverCam.CameraPosition,
		this->HoverCam.CameraDirection,
		this->HoverCam.CameraUpDirection
	);

	this->HoverCam.Projection = DirectX::XMMatrixPerspectiveFovLH(
		PI*0.45,
		SCREEN_WIDTH / SCREEN_HEIGHT,
		0.5,
		HeightY*1.5
	);

}


DirectX::XMMATRIX QuadTreeClass::GetHoverCamView()
{
	return this->HoverCam.View;
}
DirectX::XMMATRIX QuadTreeClass::GetHoverCamProjection()
{
	return this->HoverCam.Projection;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+