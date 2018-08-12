#version 450 core

layout (location = 0) in vec2 vTexCoord;	  // Interpolated texture coordinate using texture coordinate from the vertex shader
layout (location = 1) in vec3 vEyeNorm;			// Interpolated normal
layout (location = 2) in vec3 vEyeTan;	    // tangent

layout (set = 0, binding = 3) uniform sampler2D u_diffuseMap;  // The diffuse colour sampler
layout (set = 0, binding = 4) uniform sampler2D u_normalMap;  // The normal colour sampler
layout (set = 0, binding = 5) uniform sampler2D u_specularMap;  // The specular colour sampler


layout (std140, set = 0, binding = 2) uniform MaterialData
{
  vec4 UDiffuse;
  vec4 USpecular;
  vec4 UHardness;
  vec4 UEmissive;
} material;

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


    normalOut = vec4( normalize( CalcBumpedNormal() ) , 1.); 
	  
    diffuseOut = vec4(material.UDiffuse.xyz, 1 ) * texDiffuse;	// set texture color
    specularOut = vec4( material.USpecular.xyz * texSpecular.xyz, material.UHardness.x);
    emissiveOut = vec4(material.UEmissive.xyz, 1.);
    
}
