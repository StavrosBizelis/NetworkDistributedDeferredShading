#version 400 core

uniform struct PointLight
{
  vec3 m_position;

  //vec3 m_ambient;
  vec3 m_diffuse;
  vec3 m_specular;
  
  // attenuation
  float m_constantAtt;
  float m_linearAtt;
  float m_quadraticAtt;
} ULightData;

uniform vec2 UScreenResDiv;
uniform vec2 UScreenResDiv2;
uniform vec2 UOffset;

out vec4 vOutputColour;		// The output colour

uniform sampler2D UColor;
uniform sampler2D UNormal;
uniform sampler2D USpecularIntensityPower;
uniform sampler2D UDepth;

uniform mat4 UInverseViewProjectionMatrix;
uniform vec3 UCamPos;



void HandlePointLight(in PointLight a_lightSrc, in vec3 a_camPos, in vec3 a_fragmentNormal,  in vec4 a_fragmentPos, in vec3 a_fragmentSpecIntensity, in float a_fragmentSpecPower, out  vec4 a_diffuse, out  vec4 a_specular)
{
  a_diffuse = vec4(0);
  a_specular = vec4(0);
  // compute attenuation
  float l_dist = length(a_lightSrc.m_position - a_fragmentPos.xyz);
  float l_att = 1. / (a_lightSrc.m_constantAtt +   
                a_lightSrc.m_linearAtt  * l_dist +    
                a_lightSrc.m_quadraticAtt * l_dist * l_dist);   
  

  
  vec3 l_lightDirection =  normalize(  a_fragmentPos.xyz - a_lightSrc.m_position);
  vec3 l_normal = normalize(a_fragmentNormal);
  float l_diffuseFactor = dot(l_normal, -l_lightDirection);

  if (l_diffuseFactor > 0) 
  {
    a_diffuse = l_att * l_diffuseFactor * vec4(a_lightSrc.m_diffuse, 1.0f);
    
   
    vec3 l_vertexToEye  = normalize( a_camPos - a_fragmentPos.xyz);
    vec3 l_lightReflect = normalize(reflect(l_lightDirection, l_normal));
    float l_specularFactor = dot(l_vertexToEye, l_lightReflect);
    l_specularFactor = pow(l_specularFactor, a_fragmentSpecPower*500 );
    if( l_specularFactor > 0.0)
      a_specular = l_att * l_specularFactor *  vec4(a_lightSrc.m_specular * a_fragmentSpecIntensity, 1.0f);
  }

}




void main()
{
   // screen space texture coordinates
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
  
  vec4 l_specIntensityPlusPower = texture2D(USpecularIntensityPower, l_screenTextureCoord );
  
  vec4 l_diffuse;
  vec4 l_specular;
  HandlePointLight( ULightData, UCamPos, vTexNormal.xyz, l_fragWorldSpacePoint, vTexSpecular.xyz, vTexSpecular.a, l_diffuse, l_specular);
  vOutputColour = vec4( l_diffuse*vTexColour + l_specular ) ;
  
  // if( (l_diffuse.x == l_diffuse.y) && (l_diffuse.x == l_diffuse.z) && (l_diffuse.x == 0.0f))
  //   vOutputColour = vec4(1,0,0,1);
  
  //vOutputColour.xyz = vTexDepth.xyz;
  //vOutputColour.a = 1;
  //vOutputColour = ( vTexColour );
  //vOutputColour = vec4(l_screenTextureCoord.xy, 0, 1);
  //vOutputColour = vec4( l_fragWorldSpacePoint.xyz ,1.0f ) ;
  //vOutputColour = vec4(1.0);

}