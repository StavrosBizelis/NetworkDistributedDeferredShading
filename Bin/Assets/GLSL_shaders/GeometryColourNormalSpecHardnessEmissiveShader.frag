#version 400 core

in vec3 vEyeNorm;			// Interpolated normal
in vec3 vEyeTan;	    // tangent
in vec2 vTexCoord;	  // Interpolated texture coordinate using texture coordinate from the vertex shader


uniform vec3 UDiffuse = vec3(1.);
uniform vec3 USpecular = vec3(1.);
uniform float UHardness = 1.;
uniform vec3 UEmissive = vec3(1.);


uniform sampler2D u_diffuseMap;  // The diffuse colour sampler
uniform sampler2D u_normalMap;  // The normal colour sampler
uniform sampler2D u_specularMap;  // The specular colour sampler
uniform sampler2D u_hardnessMap;  // The material hardness sampler
uniform sampler2D u_emissionMap;  // The material emission sampler



layout(location = 0) out vec4 diffuseOut;
layout(location = 1) out vec4 normalOut;
layout(location = 2) out vec4 specularOut;
layout(location = 3) out vec4 emissiveOut;

// modified CalcBumpedNormal function from http://ogldev.atspace.co.uk/www/tutorial26/tutorial26.html
vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(vEyeNorm);
    vec3 Tangent = normalize(vEyeTan);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(u_normalMap, vTexCoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

void main()
{

    vec4 texDiffuse = texture(u_diffuseMap, vTexCoord);
    vec4 texSpecular = texture(u_specularMap, vTexCoord);
    float texHardness = texture(u_hardnessMap, vTexCoord).x;
    vec4 texEmissive = texture(u_emissionMap, vTexCoord);
    
    normalOut = vec4( normalize( CalcBumpedNormal() ) , 1.); 
	 // normalOut = vec4(normalize(vEyeTan).xyz, 1);
  //  normalOut = vec4(normalize(vEyeNorm).xyz, 1);
    
    diffuseOut = vec4(UDiffuse, 1) * texDiffuse;	// set texture color
    
    specularOut = vec4( USpecular*texSpecular.xyz, UHardness*texHardness);
    emissiveOut = vec4(UEmissive, 1.) * texEmissive;

}
