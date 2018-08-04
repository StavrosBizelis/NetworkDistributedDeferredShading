#version 450 core

layout(location = 0) in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

layout (std140, set = 0, binding = 0) uniform GlobalVars
{
  uniform mat4 UInverseViewProjectionMatrix;
  uniform vec4 UCamPos;
  uniform vec4 UScreenResDiv;
} globalVars;


layout (std140, set = 0, binding = 1) uniform DirectionalLight
{
  vec4 m_direction;

  vec4 m_ambient;
  vec4 m_diffuse;
  vec4 m_specular;
} ULightData;

layout (set = 0, binding = 2) uniform sampler2D UColor;
layout (set = 0, binding = 3) uniform sampler2D UNormal;
layout (set = 0, binding = 4) uniform sampler2D USpecularIntensityPower;
layout (set = 0, binding = 5) uniform sampler2D UDepth;

layout(location = 0) out vec4 vOutputColour;		// The output colour





void HandleDirectionalLight( in vec3 a_camPos, in vec3 a_fragmentNormal, in vec4 a_fragmentPos, in vec3 a_fragmentSpecIntensity, in float a_fragmentSpecPower, out  vec4 a_diffuse, out vec4 a_specular)
{ 
  a_diffuse = vec4(0);
  a_specular = vec4(0);
  // a_ambient += /* l_att * */ vec4( ULightData.m_ambient ,1);
  
  vec3 l_lightDirection = ULightData.m_direction.xyz;
  vec3 l_normal = normalize(a_fragmentNormal);
  float l_diffuseFactor = dot(l_normal, -l_lightDirection);

  if (l_diffuseFactor > 0) 
  {
    a_diffuse = l_diffuseFactor * ULightData.m_diffuse;
    
    vec3 l_vertexToEye  = normalize(  a_fragmentPos.xyz - a_camPos);
    vec3 l_lightReflect = normalize(reflect(ULightData.m_direction.xyz, l_normal));
    float l_specularFactor = dot(l_vertexToEye, l_lightReflect);
    l_specularFactor = pow(l_specularFactor, a_fragmentSpecPower*500 );
    if( l_specularFactor > 0.0)
      a_specular = l_specularFactor * vec4(ULightData.m_specular.xyz * a_fragmentSpecIntensity, 1.0);
    
  }
}






void main()
{
//	vec2 l_screenTextureCoord = gl_FragCoord.xy * UScreenResDiv.xy;
	vec2 l_screenTextureCoord = vTexCoord;
	
  vec4 vTexColour = texture(UColor, l_screenTextureCoord);	
  vec4 vTexNormal = texture(UNormal, l_screenTextureCoord);	
  vec4 vTexSpecular = texture(USpecularIntensityPower, l_screenTextureCoord);	
  vec4 vTexDepth = texture(UDepth, l_screenTextureCoord);	

  // screen space to world space


  vec4 l_fragWorldSpacePoint = globalVars.UInverseViewProjectionMatrix *  vec4( vec3( l_screenTextureCoord, vTexDepth.x ) *2 - 1,  1);
  l_fragWorldSpacePoint.w = 1/l_fragWorldSpacePoint.w;
  l_fragWorldSpacePoint.xyz *= l_fragWorldSpacePoint.w;


  //vOutputColour = vec4( 0.5f); // test
  //vOutputColour = vec4( l_screenTextureCoord , 0, 1); // test coords
  //vOutputColour = vTexColour;
  //vOutputColour = l_fragWorldSpacePoint;

  vec4 l_diffuse;
  vec4 l_specular; 
  HandleDirectionalLight( globalVars.UCamPos.xyz, vTexNormal.xyz, vec4(l_fragWorldSpacePoint.xyz,1), vTexSpecular.xyz, vTexSpecular.a, l_diffuse, l_specular);
  vOutputColour = vec4( length(l_diffuse) > 0 ? ULightData.m_ambient.xyz : vec3(0),  1.f) + l_diffuse * vTexColour + l_specular;
  // vOutputColour = vTexNormal.xyzz;
    
//  if( length( vTexNormal.xyz ) == 0)
//	vOutputColour  = vec4( 1);
//  else
   // vOutputColour = vTexNormal;
   // vOutputColour = vec4(l_screenTextureCoord.xy, 0, 1);
   // vOutputColour = l_fragWorldSpacePoint;


}
