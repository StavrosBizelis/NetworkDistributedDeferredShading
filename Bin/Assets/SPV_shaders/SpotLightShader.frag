#version 450 core
layout (std140, set = 0, binding = 1) uniform GlobalVars
{
  uniform mat4 UInverseViewProjectionMatrix;
  uniform vec3 UCamPos;
  uniform vec2 UScreenResDiv;
} globalVars;


layout (std140, set = 0, binding = 2) uniform SpotLight
{
  vec3 m_position;
  vec3 m_direction;
  float m_spotCutoff;
  float m_spotExponent;
  
  //vec3 m_ambient;
  vec3 m_diffuse;
  vec3 m_specular;
  
  // attenuation
  float m_constantAtt;
  float m_linearAtt;
  float m_quadraticAtt;
} ULightData;




layout(location = 0) out vec4 vOutputColour;		// The output colour


layout (set = 0, binding = 3) uniform sampler2D UColor;
layout (set = 0, binding = 4) uniform sampler2D UNormal;
layout (set = 0, binding = 5) uniform sampler2D USpecularIntensityPower;
layout (set = 0, binding = 6) uniform sampler2D UDepth;





void HandleSpotLight(in vec3 a_camPos, in vec3 a_fragmentNormal, in vec4 a_fragmentPos, in vec3 a_fragmentSpecIntensity, in float a_fragmentSpecPower,out  vec4 a_diffuse, out  vec4 a_specular)
{
  a_diffuse = vec4(0);
  a_specular = vec4(0);
  vec3 l_lightDirection = normalize(  a_fragmentPos.xyz - ULightData.m_position);
  
  // compute spot effect
  float l_spotEffect = dot(normalize(-ULightData.m_direction), normalize(-l_lightDirection));
  if (l_spotEffect <= ULightData.m_spotCutoff)
	return;

  l_spotEffect = pow(l_spotEffect, ULightData.m_spotExponent ); 
   
   
  // compute attenuation
  float l_dist = length(ULightData.m_position - a_fragmentPos.xyz);
  float l_att = l_spotEffect / (ULightData.m_constantAtt +   
                ULightData.m_linearAtt  * l_dist +    
                ULightData.m_quadraticAtt * l_dist * l_dist);   
  
  // a_ambient += l_att * vec4( ULightData.m_ambient ,1) * vec4( vec3(0.2), 1);//gl_FrontMaterial.ambient;
  
  vec3 l_normal = a_fragmentNormal;
  float l_diffuseFactor = dot(l_normal, -l_lightDirection);

  if (l_diffuseFactor > 0) 
  {
    l_spotEffect = pow(l_spotEffect, ULightData.m_spotExponent );
  
    a_diffuse = l_att * l_diffuseFactor * vec4(ULightData.m_diffuse, 1.0f)  * vec4( vec3(0.3), 1);//gl_FrontMaterial.diffuse;
    
    vec3 l_vertexToEye  = normalize( a_camPos - a_fragmentPos.xyz);
    vec3 l_lightReflect = normalize(reflect(l_lightDirection, l_normal));
    float l_specularFactor = dot(l_vertexToEye, l_lightReflect);
    l_specularFactor = pow(l_specularFactor, a_fragmentSpecPower*100);
    if( l_specularFactor > 0.0)
      a_specular = l_att * l_specularFactor * vec4(ULightData.m_specular * a_fragmentSpecIntensity, 1.0f);
  }
  
}




void main()
{
   // screen space texture coordinates
  vec2 l_screenTextureCoord = gl_FragCoord.xy * globalVars.UScreenResDiv.xy;

  vec4 vTexColour = texture(UColor, l_screenTextureCoord);	
  vec4 vTexNormal = texture(UNormal, l_screenTextureCoord);	
  vec4 vTexSpecular = texture(USpecularIntensityPower, l_screenTextureCoord);	
  vec4 vTexDepth = texture(UDepth, l_screenTextureCoord);	
  
  
  // screen space to world space
  vec4 l_fragWorldSpacePoint = globalVars.UInverseViewProjectionMatrix *  vec4( vec3( l_screenTextureCoord, vTexDepth.x ) *2 - 1,  1);
  l_fragWorldSpacePoint.w = 1/l_fragWorldSpacePoint.w;
  l_fragWorldSpacePoint.xyz *= l_fragWorldSpacePoint.w;
  
  
  vec4 l_diffuse;
  vec4 l_specular;
  HandleSpotLight( globalVars.UCamPos, vTexNormal.xyz, l_fragWorldSpacePoint, vTexSpecular.xyz, vTexSpecular.a, l_diffuse, l_specular);
   vOutputColour = vec4( l_diffuse*vTexColour ) +l_specular;
  
//  if( (l_diffuse.x == l_diffuse.y) && (l_diffuse.x == l_diffuse.z) && (l_diffuse.x == 0.0f))
//	vOutputColour = vec4(1,0,0,1);
  
  // vOutputColour.xyz = vTexDepth.xyz;
  //vOutputColour.a = 1;
  // vOutputColour = ( vTexColour );
  //vOutputColour = vec4(l_screenTextureCoord.xy, 0, 1);
  // vOutputColour = vec4( l_fragWorldSpacePoint.xyz ,1.0f ) ;
 // vOutputColour = vec4(1.0);

}