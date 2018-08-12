#version 400 core

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;


out vec2 vTexCoord[13];	// Texture coordinates


uniform float UBlurSize = 1;

uniform vec2 UScreenSize = vec2(800,600);
// This is the entry point into the vertex shader
void main()
{	
	// Transform the vertex spatial position using 
	gl_Position = vec4(inPosition, 1.0f);
	
	vTexCoord[0] = clamp( inCoord + vec2(.0, -6/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[1] = clamp( inCoord + vec2(.0, -5/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[2] = clamp( inCoord + vec2(.0, -4/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[3] = clamp( inCoord + vec2(.0, -3/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[4] = clamp( inCoord + vec2(.0, -2/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[5] = clamp( inCoord + vec2(.0, -1/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[6] = inCoord;
	vTexCoord[7] = clamp( inCoord + vec2(.0, 1/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[8] = clamp( inCoord + vec2(.0, 2/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[9] = clamp( inCoord + vec2(.0, 3/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[10] = clamp( inCoord + vec2(.0, 4/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[11] = clamp( inCoord + vec2(.0, 5/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
	vTexCoord[12] = clamp( inCoord + vec2(.0, 6/UScreenSize.y)*UBlurSize, vec2(0.), vec2(1.) );
} 