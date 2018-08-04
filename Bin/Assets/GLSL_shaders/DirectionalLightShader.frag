#version 400 core

in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

uniform mat4 UInverseViewProjectionMatrix;
uniform vec3 UCamPos;
uniform vec2 UScreenResDiv;
uniform vec2 UScreenResDiv2;
uniform vec2 UOffset;


uniform sampler2D UColor;
uniform sampler2D UNormal;
uniform sampler2D USpecularIntensityPower;
uniform sampler2D UDepth;

out vec4 vOutputColour;		// The output colour

uniform struct DirectionalLight
{
  vec3 m_direction;

  vec3 m_ambient;
  vec3 m_diffuse;
  vec3 m_specular;
} ULightData;




void HandleDirectionalLight(in DirectionalLight a_lightSrc, in vec3 a_camPos, in vec3 a_fragmentNormal, in vec4 a_fragmentPos, in vec3 a_fragmentSpecIntensity, in float a_fragmentSpecPower, out  vec4 a_diffuse, out  vec4 a_specular)
{ 
  a_diffuse = vec4(0);
  a_specular = vec4(0);
  // a_ambient += /* l_att * */ vec4( a_lightSrc.m_ambient ,1);
  
  vec3 l_lightDirection = a_lightSrc.m_direction;
  vec3 l_normal = normalize(a_fragmentNormal);
  float l_diffuseFactor = dot(l_normal, -l_lightDirection);

  if (l_diffuseFactor > 0) 
  {
    a_diffuse = l_diffuseFactor * vec4(a_lightSrc.m_diffuse, 1.0f);
    
    vec3 l_vertexToEye  = normalize(  a_fragmentPos.xyz - a_camPos);
    vec3 l_lightReflect = normalize(reflect(a_lightSrc.m_direction, l_normal));
    float l_specularFactor = dot(l_vertexToEye, l_lightReflect);
    l_specularFactor = pow(l_specularFactor, a_fragmentSpecPower*500 );
    if( l_specularFactor > 0.0)
      a_specular = l_specularFactor * vec4(a_lightSrc.m_specular * a_fragmentSpecIntensity, 1.0);
  }
}






void main()
{

  vec2 l_screenTextureCoord = gl_FragCoord.xy * UScreenResDiv.xy;
  vec2 l_screenTextureCoord2 = (gl_FragCoord.xy - UOffset) * UScreenResDiv2.xy;
	
  vec4 vTexColour = texture(UColor, l_screenTextureCoord);	
  vec4 vTexNormal = texture(UNormal, l_screenTextureCoord);	
  vec4 vTexSpecular = texture(USpecularIntensityPower, l_screenTextureCoord);	
  vec4 vTexDepth = texture(UDepth, l_screenTextureCoord);	

  // screen space to world space


  vec4 l_fragWorldSpacePoint = UInverseViewProjectionMatrix *  vec4( vec3( l_screenTextureCoord2, vTexDepth.x ) *2 - 1,  1);
  l_fragWorldSpacePoint.w = 1/l_fragWorldSpacePoint.w;
  l_fragWorldSpacePoint.xyz *= l_fragWorldSpacePoint.w;


  //vOutputColour = vec4( 0.5f); // test
  //vOutputColour = vec4( l_screenTextureCoord , 0, 1); // test coords
  //vOutputColour = vTexColour;
  vOutputColour = l_fragWorldSpacePoint;

  vec4 l_diffuse;
  vec4 l_specular; 
  HandleDirectionalLight( ULightData, UCamPos, vTexNormal.xyz, vec4(l_fragWorldSpacePoint.xyz,1), vTexSpecular.xyz, vTexSpecular.a, l_diffuse, l_specular);
  vOutputColour = vec4( length(l_diffuse) > 0 ? ULightData.m_ambient : vec3(0),  1.f) + l_diffuse * vTexColour + l_specular;
  // vOutputColour = vTexNormal.xyzz;
    
//  if( length( vTexNormal.xyz ) == 0)
//	vOutputColour  = vec4( 1);
//  else
//    vOutputColour = vTexNormal;

//  if( length( vTexNormal.xyz ) == 0)
//	vOutputColour  = vec4( 1);
//  else
   // vOutputColour = vTexNormal;
   // vOutputColour = vec4(l_screenTextureCoord.xy, 0, 1);
   // vOutputColour = l_fragWorldSpacePoint;
}
