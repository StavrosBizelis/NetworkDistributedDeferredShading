#pragma once
#include <vector>


#include "gl/include/glew.h"
#include <gl/gl.h>

// This class provides a wrapper around an OpenGL Vertex Buffer Object
class CVertexBufferObject
{
public:
	CVertexBufferObject();
	~CVertexBufferObject();

	void Create();									// Creates a VBO
	void Bind() const;									// Binds the VBO
	void Release();									// Releases the VBO

	void AddData(void* ptrData, unsigned int dataSize);	// Adds data to the VBO
	void UploadDataToGPU(int usageHint);			// Uploads the VBO to the GPU

	
private:
	unsigned int m_vbo;									// VBO id
	std::vector<char> m_data;							// Data to be put in the VBO
	bool m_dataUploaded;							// A flag indicating if the data has been sent to the GPU
};