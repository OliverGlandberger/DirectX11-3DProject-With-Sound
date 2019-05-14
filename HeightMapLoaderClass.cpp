#include "HeightMapLoaderClass.hpp"

HeightMapLoaderClass::HeightMapLoaderClass()
{
	//this->HeightMapFileName = "HeightMap_files/HeightMap_SmallHands.txt";
	this->HeightMapFileName = "HeightMap_files/Mini_Heightmap.txt";
	//this->HeightMapFileName = "HeightMap_files/Maze_Heightmap_1.txt";

	this->HeightMapImageName = "EMPTY";
	this->HeightMapTextureName = new std::wstring[HEIGHTMAP_TEXTURE_COUNT];

	this->SmoothNormals = true;
	this->HeightMapWidth = 0;
	this->HeightMapHeight = 0;
	this->VertexCount = 0;
	this->fc = 0;

	this->VertexHeightArray = nullptr;
	this->TempNormalArray = nullptr;
}

HeightMapLoaderClass::~HeightMapLoaderClass()
{
	this->ReleaseAll();
}

void HeightMapLoaderClass::ReleaseAll()
{
	delete this->VertexHeightArray;
	delete this->TempNormalArray;
	delete[] this->HeightMapTextureName;
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void HeightMapLoaderClass::LoadHeightMap(
	std::vector<unsigned char>	*ImageData,
	HeightMapVertexData*		*HeightMapVertexDataArray
)
{
	// PART 1
	///		OPENING .txt FILE AND READING HEIGHT MAP INFORMATION
	// --------------------------------------------------------

	HRESULT hr;
	std::ifstream input_file;

	input_file.open(this->HeightMapFileName);

	if (!input_file.is_open())
		MessageBox(NULL, L"Failed to open 'HeightMap Text File' for READING.", L"ERROR: 'LoadHeightMap() - HeightMapLoaderClass'", MB_OK);


	else	// 'Reading-the-file' code
	{
		std::string CurrentString;
		int CurrentInt;

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		int TextureCount = 0;

		// READING FROM FILE; looping till the end of the file is reached
		while (!input_file.eof())
		{
			input_file >> CurrentString;

			if (CurrentString == "SmoothNormals")
			{
				input_file >> CurrentString;

				if (CurrentString == "false")
					this->SmoothNormals = false;

				else if (CurrentString == "true")
					this->SmoothNormals = true;
			}

			else if (CurrentString == "TextureName")
			{
				input_file >> CurrentString;
				CurrentString = "HeightMap_files/" + CurrentString;

				if (TextureCount < HEIGHTMAP_TEXTURE_COUNT) //Safety measure to avoid over extending the array
				{
					this->HeightMapTextureName[TextureCount] = converter.from_bytes(CurrentString);
					TextureCount++;
				}
			}

			else if (CurrentString == "FileName")
			{
				input_file >> CurrentString;
				this->HeightMapImageName = ("HeightMap_files/" + CurrentString);
			}

			else if (CurrentString == "Width")
			{
				input_file >> CurrentInt;
				this->HeightMapWidth = CurrentInt;
			}

			else if (CurrentString == "Height")
			{
				input_file >> CurrentInt;
				this->HeightMapHeight = CurrentInt;
			}

			else if (CurrentString == "vc")
			{
				input_file >> CurrentInt;
				this->VertexCount = CurrentInt;
			}
		}

		if (input_file.is_open())
			input_file.close();
	}

	// --------------------------------------------------------





	// PART 2
	///		LOADING THE .png FILE
	// -------------------------

	unsigned UnsignedWidth, UnsignedHeight;
	unsigned error; // If error != 0, then the 'decode' function failed

	UnsignedWidth = abs(this->HeightMapWidth);
	UnsignedHeight = abs(this->HeightMapHeight);

	error = lodepng::decode((*ImageData), UnsignedWidth, UnsignedWidth, this->HeightMapImageName);

	if (error != 0)
		MessageBox(NULL, L"Failed to load 'HeightMap'.", L"ERROR: 'LoadHeightMap() - HeightMapClass.cpp'", MB_OK);

	/// -------------------------





	// PART 3
	///		ALLOCATING 'VertexHeightArray' & STORING HEIGHT VALUES PER VERTEX
	// ---------------------------------------------------------------------------------

	this->VertexHeightArray = new float[this->VertexCount]; // Temporarily holds height data
	for (int s = 0; s < this->VertexCount; s++)
		this->VertexHeightArray[s] = 0.0f;

	// Will only read the 'R' value (R = G = B = HeightValue);
	for (int i = 0; i < this->VertexCount; i++)
	{
		this->VertexHeightArray[i] = (float)(*ImageData)[(i * 4)];
		this->VertexHeightArray[i] /= 255;	// Converting to a value between 0.0f and 1.0f
	}

	// ---------------------------------------------------------------------------------





	// PART 4
	///		(Hover over function for description)
	this->ConvertToVertexData(HeightMapVertexDataArray);
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void HeightMapLoaderClass::StoreToVertexData(HeightMapVertexData *HeightMapVertexDataArray)
{

}

/* ------------- COMMENTS -------------
Using all the data provided by the 'LoadHeightMap()' function, this function
creates the Vertices, TexCoordinates, Normals and Faces along with indexing
everything. The 'to-be-used' data is stored in the HeightMapVertexData parameter.
*/
void HeightMapLoaderClass::ConvertToVertexData(HeightMapVertexData* *HeightMapVertexDataArray)
{
	// "LEFT SIDE! MANY VARIABLES! NOW, HANDLE IT!!!"
	//

	//	VARIABLES LIST
	// ---------------------------------------------------------------------------------

	DirectX::XMVECTOR TempPointHolder1 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR TempPointHolder2 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR Edge1Vector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR Edge2Vector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR FaceNormalVector1 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR FaceNormalVector2 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 FaceNormalFloat3_1 = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 FaceNormalFloat3_2 = { 0.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR TempVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	int IndexHolder;	// Provides optimization through reduced calculations

						// ---------------------------------------------------------------------------------




						// INITIALIZATION
						// ---------------------------------------------------------------------------------

						// VERTICES --- MEMORY ALLOCATION
	DirectX::XMFLOAT3 *TempVertexArray;
	TempVertexArray = new DirectX::XMFLOAT3[this->VertexCount];

	for (int s = 0; s < this->VertexCount; s++)
		TempVertexArray[s] = { 0.0f, 0.0f, 0.0f };



	// VERTICES --- GRID IS CREATED (x,y,z positions in local space)
	for (int i = 0; i < this->HeightMapHeight; i++)		// Current Y-Coordinate of the 'Grid'	(i)
	{
		for (int j = 0; j < this->HeightMapWidth; j++)	// Current X-Coordinate of the 'Grid'	(j)
		{
			IndexHolder = ((i * this->HeightMapWidth) + j);

			TempVertexArray[IndexHolder].x = ((float)j * HEIGHTMAP_WIDTH_MULTIPLIER);
			TempVertexArray[IndexHolder].y = (VertexHeightArray[IndexHolder] * HEIGHTMAP_HEIGHT_MULTIPLIER);
			TempVertexArray[IndexHolder].z = ((float)i * HEIGHTMAP_WIDTH_MULTIPLIER);
		}
	}



	// FACES & UV-TEXCOORDS --- MEMORY ALLOCATION
	this->fc = ((this->HeightMapWidth - 1) * (this->HeightMapHeight - 1)) * 2; // Number of quads * 2 triangles
	FaceIndexData *FaceIndexDataArray = new FaceIndexData[this->fc];

	DirectX::XMFLOAT2 *TempTexCoordArray = new DirectX::XMFLOAT2[this->VertexCount];
	for (int s = 0; s < this->VertexCount; s++)
		TempTexCoordArray[s] = { 0.0f, 0.0f };

	float TexUCoord = 0.0f;
	float TexVCoord = 0.0f;

	// ---------------------------------------------------------------------------------





	IndexHolder = 0; // RESET

					 // TEXCOORDS	--- CALCULATION & INDEXING
					 // VERTICES		--- INDEXING
	for (int i = 0; i < (this->HeightMapHeight - 1); i++)		// Current Y-Coordinate of the 'Grid'	(i)
	{
		for (int j = 0; j < (this->HeightMapWidth - 1); j++)	// Current X-Coordinate of the 'Grid'	(j)
		{
			// UV-TEXTCOORDS
			// TOP LEFT VERTEX
			TempTexCoordArray[((i * this->HeightMapWidth) + j)] = { (TexUCoord + 0.0f), (TexVCoord + 0.0f) };

			// TOP RIGHT VERTEX
			TempTexCoordArray[((i * this->HeightMapWidth) + (j + 1))] = { (TexUCoord + 1.0f), (TexVCoord + 0.0f) };

			// BOTTOM LEFT VERTEX
			TempTexCoordArray[(((i + 1) * this->HeightMapWidth) + j)] = { (TexUCoord + 0.0f), (TexVCoord + 1.0f) };

			// BOTTOM RIGHT VERTEX
			TempTexCoordArray[(((i + 1) * this->HeightMapWidth) + (j + 1))] = { (TexUCoord + 1.0f), (TexVCoord + 1.0f) };

			TexUCoord++;	// THIS = The grid will be 'TEXTURE WRAPPED' (1/2)





							// FACE1 (Indexing Vertices & UV-Coordinates)
							// BOTTOM LEFT VERTEX
			FaceIndexDataArray[IndexHolder].Vertex1.x = (((i + 1) * this->HeightMapWidth) + j);
			FaceIndexDataArray[IndexHolder].Vertex1.y = (((i + 1) * this->HeightMapWidth) + j);

			// TOP LEFT VERTEX
			FaceIndexDataArray[IndexHolder].Vertex2.x = ((i * this->HeightMapWidth) + j);
			FaceIndexDataArray[IndexHolder].Vertex2.y = ((i * this->HeightMapWidth) + j);

			// TOP RIGHT VERTEX
			FaceIndexDataArray[IndexHolder].Vertex3.x = ((i * this->HeightMapWidth) + (j + 1));
			FaceIndexDataArray[IndexHolder].Vertex3.y = ((i * this->HeightMapWidth) + (j + 1));

			IndexHolder++;	// Incrementing for next Face

							// FACE2 (Indexing Vertices & UV-Coordinates)
							// TOP RIGHT VERTEX
			FaceIndexDataArray[IndexHolder].Vertex1.x = ((i * this->HeightMapWidth) + (j + 1));
			FaceIndexDataArray[IndexHolder].Vertex1.y = ((i * this->HeightMapWidth) + (j + 1));

			// BOTTOM RIGHT VERTEX
			FaceIndexDataArray[IndexHolder].Vertex2.x = (((i + 1) * this->HeightMapWidth) + (j + 1));
			FaceIndexDataArray[IndexHolder].Vertex2.y = (((i + 1) * this->HeightMapWidth) + (j + 1));

			// BOTTOM LEFT VERTEX
			FaceIndexDataArray[IndexHolder].Vertex3.x = (((i + 1) * this->HeightMapWidth) + j);
			FaceIndexDataArray[IndexHolder].Vertex3.y = (((i + 1) * this->HeightMapWidth) + j);

			IndexHolder++;	// Incrementing for next loop
		}

		TexUCoord = 0;		// Reseting this as we move to the next row
		TexVCoord++;		// THIS: The grid will be 'TEXTURE WRAPPED' (2/2)
	}





	IndexHolder = 0; // RESET

					 // NORMALS --- ALLOCATING MEMORY
					 // SmoothNormals = true -> Memory Allocation: VertexCount
	if (this->SmoothNormals)
	{
		this->TempNormalArray = new DirectX::XMFLOAT3[this->VertexCount];
		for (int s = 0; s < this->VertexCount; s++)
			this->TempNormalArray[s] = { 0.0f, 0.0f, 0.0f };

	}

	// SmoothNormals = false -> Memory Allocation: FaceCount
	else
	{
		this->TempNormalArray = new DirectX::XMFLOAT3[this->fc];
		for (int s = 0; s < this->fc; s++)
			this->TempNormalArray[s] = { 0.0f, 0.0f, 0.0f };
	}





	// NORMALS --- CALCULATING & INDEXING
	// LOOP COUNT IS HALF -> calculating 2 FACES of 1 QUAD >> each loop
	for (int i = 0; i < (this->fc / 2); i++)
	{
		// PART1: CALCULATING

		// FACE1, Vertex1 (for 'FACE NORMAL')
		IndexHolder = (int)FaceIndexDataArray[(i * 2)].Vertex1.x;

		TempPointHolder1 = DirectX::XMVectorSet(
			TempVertexArray[IndexHolder].x,
			TempVertexArray[IndexHolder].y,
			TempVertexArray[IndexHolder].z,
			1.0f
		);



		// FACE1, Vertex2 (for 'FACE NORMAL')
		IndexHolder = (int)FaceIndexDataArray[(i * 2)].Vertex2.x;

		TempPointHolder2 = DirectX::XMVectorSet(
			TempVertexArray[IndexHolder].x,
			TempVertexArray[IndexHolder].y,
			TempVertexArray[IndexHolder].z,
			1.0f
		);

		Edge1Vector = DirectX::XMVectorSubtract(TempPointHolder1, TempPointHolder2);



		// FACE1, Vertex3 (for 'FACE NORMAL')
		IndexHolder = (int)FaceIndexDataArray[(i * 2)].Vertex3.x;

		TempPointHolder2 = DirectX::XMVectorSet(
			TempVertexArray[IndexHolder].x,
			TempVertexArray[IndexHolder].y,
			TempVertexArray[IndexHolder].z,
			1.0f
		);

		Edge2Vector = DirectX::XMVectorSubtract(TempPointHolder1, TempPointHolder2);

		FaceNormalVector1 = DirectX::XMVector3Cross(Edge2Vector, Edge1Vector);	// Creating the NORMAL for FACE1

		FaceNormalVector1 = DirectX::XMVector3Normalize(FaceNormalVector1);	// NOT SURE IF I NEED TO NORMALIZE





																			// FACE2, Vertex1 (for 'FACE NORMAL')
		IndexHolder = (int)FaceIndexDataArray[((i * 2) + 1)].Vertex1.x;

		TempPointHolder1 = DirectX::XMVectorSet(
			TempVertexArray[IndexHolder].x,
			TempVertexArray[IndexHolder].y,
			TempVertexArray[IndexHolder].z,
			1.0f
		);



		// Face2, Vertex2 (for 'FACE NORMAL')
		IndexHolder = (int)FaceIndexDataArray[((i * 2) + 1)].Vertex2.x;

		TempPointHolder2 = DirectX::XMVectorSet(
			TempVertexArray[IndexHolder].x,
			TempVertexArray[IndexHolder].y,
			TempVertexArray[IndexHolder].z,
			1.0f
		);

		Edge1Vector = DirectX::XMVectorSubtract(TempPointHolder1, TempPointHolder2);



		// Face2, Vertex3 (for 'FACE NORMAL')
		IndexHolder = (int)FaceIndexDataArray[((i * 2) + 1)].Vertex3.x;

		TempPointHolder2 = DirectX::XMVectorSet(
			TempVertexArray[IndexHolder].x,
			TempVertexArray[IndexHolder].y,
			TempVertexArray[IndexHolder].z,
			1.0f
		);

		Edge2Vector = DirectX::XMVectorSubtract(TempPointHolder1, TempPointHolder2);

		FaceNormalVector2 = DirectX::XMVector3Cross(Edge2Vector, Edge1Vector);	// Creating the NORMAL for FACE2

		FaceNormalVector2 = DirectX::XMVector3Normalize(FaceNormalVector2);	// NOT SURE IF I NEED TO NORMALIZE




																			// PART2: INDEXING
		if (this->SmoothNormals)
		{
			DirectX::XMStoreFloat3(&FaceNormalFloat3_1, FaceNormalVector1);

			DirectX::XMStoreFloat3(&FaceNormalFloat3_2, FaceNormalVector2);

			// FACE 1
			// BOTTOM LEFT VERTEX
			IndexHolder = (int)FaceIndexDataArray[(i * 2)].Vertex1.x;

			this->TempNormalArray[IndexHolder].x += FaceNormalFloat3_1.x; // (We're taking the already indexed vertex and making that the normal index as well (a unique normal per vertex))
			this->TempNormalArray[IndexHolder].y += FaceNormalFloat3_1.y;
			this->TempNormalArray[IndexHolder].z += FaceNormalFloat3_1.z;

			FaceIndexDataArray[(i * 2)].Vertex1.z = FaceIndexDataArray[(i * 2)].Vertex1.x;

			// TOP LEFT VERTEX
			IndexHolder = (int)FaceIndexDataArray[(i * 2)].Vertex2.x;

			this->TempNormalArray[IndexHolder].x += FaceNormalFloat3_1.x;
			this->TempNormalArray[IndexHolder].y += FaceNormalFloat3_1.y;
			this->TempNormalArray[IndexHolder].z += FaceNormalFloat3_1.z;

			FaceIndexDataArray[(i * 2)].Vertex2.z = FaceIndexDataArray[(i * 2)].Vertex2.x;

			// TOP RIGHT VERTEX
			IndexHolder = (int)FaceIndexDataArray[(i * 2)].Vertex3.x;

			this->TempNormalArray[IndexHolder].x += FaceNormalFloat3_1.x;
			this->TempNormalArray[IndexHolder].y += FaceNormalFloat3_1.y;
			this->TempNormalArray[IndexHolder].z += FaceNormalFloat3_1.z;

			FaceIndexDataArray[(i * 2)].Vertex3.z = FaceIndexDataArray[(i * 2)].Vertex3.x;





			// FACE 2
			// TOP RIGHT VERTEX
			IndexHolder = (int)FaceIndexDataArray[((i * 2) + 1)].Vertex1.x;

			this->TempNormalArray[IndexHolder].x += FaceNormalFloat3_2.x;
			this->TempNormalArray[IndexHolder].y += FaceNormalFloat3_2.y;
			this->TempNormalArray[IndexHolder].z += FaceNormalFloat3_2.z;

			FaceIndexDataArray[((i * 2) + 1)].Vertex1.z = FaceIndexDataArray[((i * 2) + 1)].Vertex1.x;

			// BOTTOM RIGHT VERTEX
			IndexHolder = (int)FaceIndexDataArray[((i * 2) + 1)].Vertex2.x;

			this->TempNormalArray[IndexHolder].x += FaceNormalFloat3_2.x;
			this->TempNormalArray[IndexHolder].y += FaceNormalFloat3_2.y;
			this->TempNormalArray[IndexHolder].z += FaceNormalFloat3_2.z;

			FaceIndexDataArray[((i * 2) + 1)].Vertex2.z = FaceIndexDataArray[((i * 2) + 1)].Vertex2.x;

			// BOTTOM LEFT VERTEX
			IndexHolder = (int)FaceIndexDataArray[((i * 2) + 1)].Vertex3.x;

			this->TempNormalArray[IndexHolder].x += FaceNormalFloat3_2.x;
			this->TempNormalArray[IndexHolder].y += FaceNormalFloat3_2.y;
			this->TempNormalArray[IndexHolder].z += FaceNormalFloat3_2.z;

			FaceIndexDataArray[((i * 2) + 1)].Vertex3.z = FaceIndexDataArray[((i * 2) + 1)].Vertex3.x;
		}




		// !SmoothNormals (This results in hard edges)
		else
		{
			DirectX::XMStoreFloat3(&this->TempNormalArray[(i * 2)], FaceNormalVector1); // ((i * 2) + 1) for cool effect

			FaceIndexDataArray[(i * 2)].Vertex1.z = (i * 2);
			FaceIndexDataArray[(i * 2)].Vertex2.z = (i * 2);
			FaceIndexDataArray[(i * 2)].Vertex3.z = (i * 2);

			DirectX::XMStoreFloat3(&this->TempNormalArray[((i * 2) + 1)], FaceNormalVector2);

			FaceIndexDataArray[((i * 2) + 1)].Vertex1.z = ((i * 2) + 1);
			FaceIndexDataArray[((i * 2) + 1)].Vertex2.z = ((i * 2) + 1);
			FaceIndexDataArray[((i * 2) + 1)].Vertex3.z = ((i * 2) + 1);

		}
	}




	// NORMALIZING all normal vectors, if SmoothNormals = true
	if (this->SmoothNormals)
	{

		for (int n = 0; n < this->VertexCount; n++)
		{
			TempVector = DirectX::XMVectorSet(
				this->TempNormalArray[n].x,
				this->TempNormalArray[n].y,
				this->TempNormalArray[n].z,
				0.0f
			);

			TempVector = DirectX::XMVector4Normalize(TempVector);

			DirectX::XMStoreFloat3(&this->TempNormalArray[n], TempVector);
		}
	}




	// FINAL STEP: STORING THE DATA
	(*HeightMapVertexDataArray) = new HeightMapVertexData[(this->fc * 3)];

	for (int i = 0; i < this->fc; i++)
	{
		(*HeightMapVertexDataArray)[((i * 3) + 2)].Position = TempVertexArray[(int)FaceIndexDataArray[i].Vertex1.x];
		(*HeightMapVertexDataArray)[((i * 3) + 2)].texCoord = TempTexCoordArray[(int)FaceIndexDataArray[i].Vertex1.y];
		(*HeightMapVertexDataArray)[((i * 3) + 2)].Normal = this->TempNormalArray[(int)FaceIndexDataArray[i].Vertex1.z];

		(*HeightMapVertexDataArray)[((i * 3) + 1)].Position = TempVertexArray[(int)FaceIndexDataArray[i].Vertex2.x];
		(*HeightMapVertexDataArray)[((i * 3) + 1)].texCoord = TempTexCoordArray[(int)FaceIndexDataArray[i].Vertex2.y];
		(*HeightMapVertexDataArray)[((i * 3) + 1)].Normal = this->TempNormalArray[(int)FaceIndexDataArray[i].Vertex2.z];

		(*HeightMapVertexDataArray)[((i * 3) + 0)].Position = TempVertexArray[(int)FaceIndexDataArray[i].Vertex3.x];
		(*HeightMapVertexDataArray)[((i * 3) + 0)].texCoord = TempTexCoordArray[(int)FaceIndexDataArray[i].Vertex3.y];
		(*HeightMapVertexDataArray)[((i * 3) + 0)].Normal = this->TempNormalArray[(int)FaceIndexDataArray[i].Vertex3.z];
	}

	// RELEASING MEMORY
	delete TempVertexArray;
	delete TempTexCoordArray;
	delete FaceIndexDataArray;
	//this->ReleaseAll();
}





// GET FUNCTIONS

int HeightMapLoaderClass::GetFaceCount()
{
	return this->fc;
}

int HeightMapLoaderClass::GetHeightMapWidth()
{
	return this->HeightMapWidth;
}

int HeightMapLoaderClass::GetHeightMapHeight()
{
	return this->HeightMapHeight;
}

float* HeightMapLoaderClass::GetVertexHeightArray()
{
	return this->VertexHeightArray;
}

std::wstring HeightMapLoaderClass::GetHeightMapTextureName(int TextureSlot)
{
	return this->HeightMapTextureName[TextureSlot];
}