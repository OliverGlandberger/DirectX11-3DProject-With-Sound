#include "objLoader.hpp"

objLoaderClass::objLoaderClass()
{
	this->temp_vertices = nullptr;
	this->temp_texCoords = nullptr;
	this->temp_normals = nullptr;
}

objLoaderClass::~objLoaderClass()
{

}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void objLoaderClass::importObjFile(ObjectClass *Object)
{
	float GreatestX = FLT_MIN, LowestX = FLT_MAX;	// Used to create quadtree-grid.
	float GreatestY = FLT_MIN, LowestY = FLT_MAX;	//
	float GreatestZ = FLT_MIN, LowestZ = FLT_MAX;	//

	input_file.open(*Object->getFileName());
	if (!input_file.is_open())
	{
		MessageBox(NULL, L"Failed to open a .obj file.", L"ERROR: 'importObjFile()'", MB_OK);
	}

	else
	{
		std::string currentString;				// Storing text read in from file; once at start loop
		char currentCharacter;					// Discards single characters
		std::string throwAwayLine;				// Discards lines

		float float_holder[3];					// Holds x,y,z values / u,v coordinates
		int faceVertexIndices[3];				/// These three variables are
		int faceTexCoordIndices[3];				/// all used to temporarily store
		int faceNormalIndices[3];				/// values to be written in z,y,x order.

		int indexHolder = 0;					// Index Value, which resets for each new 'category'
		std::string categoryHolder = "empty";	// Keeps track of current type of data (v, vt, vn, f)

												/// currentString = "LOOP_ITERATION_FINISHED";
												// -- Each of these are all safety measures
												// -- to combat abnormaly written text files.





		while (!input_file.eof())	// Runs until end of file is reached
		{
			input_file >> currentString;





			if (currentString == "#")	// Comments are discarded
				getline(input_file, throwAwayLine);




			// -- OPENING, READING AND STORING VALUES FROM A 'MATERIAL FILE'
			else if (currentString == "mtllib")
			{
				input_file >> currentString;

				Object->getObjectData()->materialFileName = currentString;
				currentString = "obj_files/" + currentString;

				std::ifstream mlt_file;
				mlt_file.open(currentString);
				if (!mlt_file.is_open())
				{
					MessageBox(NULL, L"Failed to open a .mtl file.", L"ERROR: 'importObjFile()'", MB_OK);
				}

				else while (!mlt_file.eof())
				{
					mlt_file >> currentString;

					if (currentString == "newmtl")
					{
						mlt_file >> currentString;

						Object->getObjectData()->materialName = currentString;
						currentString = "LOOP_ITERATION_FINISHED";
					}

					else if (currentString == "Texture")
					{
						mlt_file >> currentString;

						// Converting to a 'wide string' so that the Texture loader can function
						std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
						Object->getObjectData()->textureFileName = converter.from_bytes(currentString);

						currentString = "LOOP_ITERATION_FINISHED";
					}

					else if (currentString == "illum")
					{
						mlt_file >> Object->getObjectData()->illumValue;
						currentString = "LOOP_ITERATION_FINISHED";
					}

					else if (currentString == "Kd")
					{
						for (int i = 0; i < 3; i++)
						{
							mlt_file >> float_holder[i];
						}

						Object->getObjectData()->KdValues = { float_holder[0], float_holder[1], float_holder[2] };
						currentString = "LOOP_ITERATION_FINISHED";
					}

					else if (currentString == "Ka")
					{
						for (int j = 0; j < 3; j++)
						{
							mlt_file >> float_holder[j];
						}

						Object->getObjectData()->KaValues = { float_holder[0], float_holder[1], float_holder[2] };
						currentString = "LOOP_ITERATION_FINISHED";
					}

					else if (currentString == "Tf")
					{
						for (int k = 0; k < 3; k++)
						{
							mlt_file >> float_holder[k];
						}

						Object->getObjectData()->TfValues = { float_holder[0], float_holder[1], float_holder[2] };
						currentString = "LOOP_ITERATION_FINISHED";
					}

					else if (currentString == "Ni")
					{
						mlt_file >> Object->getObjectData()->NiValue;
						currentString = "LOOP_ITERATION_FINISHED";
					}
				}

				if (!mlt_file.is_open())
					std::string currentString = "Opps!";	// This is for DEBUGGING purposes; leave this breakpoint

				if (mlt_file.is_open())
					mlt_file.close();

				currentString = "LOOP_ITERATION_FINISHED";
			}





			else if (currentString == "\n"); // Do nothing; restart 'while loop'





			// -- ACQUIRING VERTEX COUNT AS WELL AS ALLOCATING MEMORY --
			else if (currentString == "vc")
			{
				input_file >> Object->getObjectData()->vertexCount;

				this->temp_vertices = new DirectX::XMFLOAT3[Object->getObjectData()->vertexCount];
				currentString = "LOOP_ITERATION_FINISHED";
			}




			// -- ACQUIRING TEXCOORD COUNT AS WELL AS ALLOCATING MEMORY --
			else if (currentString == "vtc")
			{
				input_file >> Object->getObjectData()->texCoordCount;

				this->temp_texCoords = new DirectX::XMFLOAT2[Object->getObjectData()->texCoordCount];
				currentString = "LOOP_ITERATION_FINISHED";
			}





			// -- ACQUIRING TEXCOORD COUNT AS WELL AS ALLOCATING MEMORY --
			else if (currentString == "vnc")
			{
				input_file >> Object->getObjectData()->normalCount;

				this->temp_normals = new DirectX::XMFLOAT3[Object->getObjectData()->normalCount];
				currentString = "LOOP_ITERATION_FINISHED";
			}





			// -- ACQUIRING FACE COUNT AS WELL AS ALLOCATING ITS MEMORY --
			else if (currentString == "fc")
			{
				input_file >> Object->getObjectData()->faceCount;

				(*Object->getVerticeData()) = new VertexDataClass[(Object->getObjectData()->faceCount) * 3];
				
				for (int i = 0; i < (Object->getObjectData()->faceCount) * 3; i++)
				{
					(*Object->getVerticeData())[i].normal = { 0, 0, 0 };
					(*Object->getVerticeData())[i].texCoord = { 0, 0 };
					(*Object->getVerticeData())[i].position = { 0, 0, 0 };
				}
					

				currentString = "LOOP_ITERATION_FINISHED";
			}



			// -- READING AND STORING ALL VERTEX POINTS --
			else if (currentString == "v")
			{
				if (categoryHolder != "v")
				{
					indexHolder = 0;
					categoryHolder = "v";
				}


				input_file >> currentString;		// X
				float_holder[0] = std::stod(currentString);

				input_file >> currentString;		// Y
				float_holder[1] = std::stod(currentString);

				input_file >> currentString;		// Z
				float_holder[2] = std::stod(currentString);

				float_holder[2] *= -1.0f;	// Converting from right-handed to left-handed system

				this->temp_vertices[indexHolder] = { float_holder[0], float_holder[1], float_holder[2] };

				indexHolder++;
				currentString = "LOOP_ITERATION_FINISHED";
			}




			// -- READING AND STORING ALL TEX COORDS --
			else if (currentString == "vt")
			{
				if (categoryHolder != "vt")
				{
					indexHolder = 0;
					categoryHolder = "vt";
				}


				input_file >> currentString;		// U
				float_holder[0] = std::stod(currentString);

				input_file >> currentString;		// V
				float_holder[1] = std::stod(currentString);

				this->temp_texCoords[indexHolder] = { float_holder[0], float_holder[1] };

				indexHolder++;
				currentString = "LOOP_ITERATION_FINISHED";
			}




			// -- READING AND STORING ALL VERTEX NORMALS --
			else if (currentString == "vn")
			{
				if (categoryHolder != "vn")
				{
					indexHolder = 0;
					categoryHolder = "vn";
				}


				input_file >> currentString;		// X
				float_holder[0] = std::stod(currentString);

				input_file >> currentString;		// Y
				float_holder[1] = std::stod(currentString);

				input_file >> currentString;		// Z
				float_holder[2] = std::stod(currentString);

				float_holder[2] *= -1.0f;	// Converting from right-handed to left-handed system

				this->temp_normals[indexHolder] = { float_holder[0], float_holder[1], float_holder[2] };

				indexHolder++;
				currentString = "LOOP_ITERATION_FINISHED";
			}



			

			// -- READING IN ALL FACES, AND THEREBY STORING DATA --
			else if (currentString == "f")
			{
				if (categoryHolder != "f")
				{
					indexHolder = 0;
					categoryHolder = "f";
				}


				for (int i = 0; i < 3; i++)		// Reads 3 points of each face, temporarily storing data
				{
					input_file >> faceVertexIndices[i];
					faceVertexIndices[i]--;

					input_file >> currentCharacter;

					input_file >> faceTexCoordIndices[i];
					faceTexCoordIndices[i]--;

					input_file >> currentCharacter;

					input_file >> faceNormalIndices[i];
					faceNormalIndices[i]--;
				}

				for (int j = 2; j >= 0; j--)	// Storing the to-be-used data, read in backwards
				{
					(*Object->getVerticeData())[indexHolder].position = this->temp_vertices[faceVertexIndices[j]];
					(*Object->getVerticeData())[indexHolder].texCoord = this->temp_texCoords[faceTexCoordIndices[j]];
					(*Object->getVerticeData())[indexHolder].normal = this->temp_normals[faceNormalIndices[j]];
				

					// Find the greatest/lowest x-y values for the object, which will be used in QuadTree creation.
					if ((*Object->getVerticeData())[indexHolder].position.x > GreatestX) {
						GreatestX = (*Object->getVerticeData())[indexHolder].position.x;
					}
					if ((*Object->getVerticeData())[indexHolder].position.x < LowestX) {
						LowestX = (*Object->getVerticeData())[indexHolder].position.x;
					}

					if ((*Object->getVerticeData())[indexHolder].position.y > GreatestY) {
						GreatestY = (*Object->getVerticeData())[indexHolder].position.y;
					}
					if ((*Object->getVerticeData())[indexHolder].position.y < LowestY) {
						LowestY = (*Object->getVerticeData())[indexHolder].position.y;
					}

					if ((*Object->getVerticeData())[indexHolder].position.z > GreatestZ) {
						GreatestZ = (*Object->getVerticeData())[indexHolder].position.z;
					}
					if ((*Object->getVerticeData())[indexHolder].position.z < LowestZ) {
						LowestZ = (*Object->getVerticeData())[indexHolder].position.z;
					}
					

					indexHolder++;
				}



				currentString = "LOOP_ITERATION_FINISHED";
			}


			// -- READING IN ALL SIDES, CURRENTLY BEING DISCARDED --
			else if (currentString == "s")
			{
				getline(input_file, throwAwayLine);
				currentString = "LOOP_ITERATION_FINISHED";
			}




			// -- READING IN THE MATERIAL NAME --
			else if (currentString == "g")
			{
				input_file >> currentString;
				Object->getObjectData()->materialName = currentString;
				currentString = "LOOP_ITERATION_FINISHED";
			}

			
			/*
			"delete temp_X" is not needed since they conatin no memory when 
			this line is reached.
			*/
		}
	}

	Object->SetGreatestX(GreatestX);
	Object->SetLowestX(LowestX);
	Object->SetGreatestY(GreatestY);
	Object->SetLowestY(LowestY);
	Object->SetGreatestZ(GreatestZ);
	Object->SetLowestZ(LowestZ);


	if (!input_file.is_open())
		std::string currentString = "OOPS!";	// This is for DEBUGGING purposes; leave this breakpoint

	if (input_file.is_open())
		input_file.close();
}

VertexDataClass* *objLoaderClass::getVerticeData()
{
	return &this->bufferData;
}

ObjectDataClass *objLoaderClass::getObjectData()
{
	return &this->ObjectData;
}




//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
