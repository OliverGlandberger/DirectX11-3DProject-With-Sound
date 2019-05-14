#ifndef OBJECTHANDLERCLASS_HPP
#define OBJECTHANDLERCLASS_HPP

#include <d3d11.h>

#include "objLoader.hpp"
#include "HeightMapClass.hpp"
#include "HeightMapLoaderClass.hpp"
#include "Globals.hpp"		// OBJECT_COUNT

#include "Object.hpp"

class ObjectHandlerClass {
private:
	// Needs to work with array_count per unique object and needs
	// to adapt SetObjectData so that it accounts for these changes.

	ObjectClass			*AllObjects;
	HeightMapClass		HeightMap;

	ID3D11Buffer*		*VertexBufferArray;
	ID3D11Buffer*		HeightMapVertexBuffer;





public:
	ObjectHandlerClass();
	~ObjectHandlerClass();
	void ReleaseAll();





	// SET FUNCTIONS

	/* ------------- COMMENTS -------------
	Takes all data from ObjectArray and packs it into the internal VertexBufferArray
	index by index.
	*/
	void SetAllObjectData(
		ID3D11Device*	*Device,
		ObjectClass		*ObjectArray
	);

	void SetHeightMapVertexData(
		ID3D11Device*			*Device,
		HeightMapVertexData*	*HeightMapVertexDataArray,
		HeightMapLoaderClass*	HeightMapData
	);





	// GET FUNCTIONS

	ObjectClass*		*GetAllObjects();

	HeightMapClass*		GetHeightMap();

	/* ------------- COMMENTS -------------
	Returns the adress to the array!
	*/
	ID3D11Buffer**		*GetVertexBuffers();

	ID3D11Buffer*		*GetHeightMapVertexBuffer();
};

#endif