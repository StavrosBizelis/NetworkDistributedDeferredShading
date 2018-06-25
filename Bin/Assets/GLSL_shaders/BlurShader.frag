#version 400 core

in vec2 vTexCoord[13];	// Texture coordinates

layout(location = 0) out vec4 ColorOut;

uniform sampler2D UTexture;
uniform float UWeights[7] = float[] (0.055908, 0.065113, 0.073762,0.081275,0.087105,0.090803, 0.09207);
void main()
{
	ColorOut = vec4(0.0);
	ColorOut += texture(UTexture, vTexCoord[0])*UWeights[0];
	ColorOut += texture(UTexture, vTexCoord[1])*UWeights[1];
	ColorOut += texture(UTexture, vTexCoord[2])*UWeights[2];
	ColorOut += texture(UTexture, vTexCoord[3])*UWeights[3];
	ColorOut += texture(UTexture, vTexCoord[4])*UWeights[4];
	ColorOut += texture(UTexture, vTexCoord[5])*UWeights[5];
	ColorOut += texture(UTexture, vTexCoord[6])*UWeights[6];
	ColorOut += texture(UTexture, vTexCoord[7])*UWeights[5];
	ColorOut += texture(UTexture, vTexCoord[8])*UWeights[4];
	ColorOut += texture(UTexture, vTexCoord[9])*UWeights[3];
	ColorOut += texture(UTexture, vTexCoord[10])*UWeights[2];
	ColorOut += texture(UTexture, vTexCoord[11])*UWeights[1];
	ColorOut += texture(UTexture, vTexCoord[12])*UWeights[0];
}