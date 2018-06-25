#version 400 core

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;


out vec2 vTexCoord;	// Texture coordinate


// This is the entry point into the vertex shader
void main()
{	

	// Transform the vertex spatial position using 
	gl_Position = vec4(inPosition, 1.0f);
	vTexCoord = inCoord;
} 
	