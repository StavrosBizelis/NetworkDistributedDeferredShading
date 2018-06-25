#version 400 core


in vec2 vTexCoord;	// Texture coordinate
out vec4 ColourOut;

uniform float UBrightnessThresh = 0.9f;
uniform sampler2D UTexture;

void main()
{            

    vec4 l_textCol = texture( UTexture, vTexCoord);

    /// brightness algorithm taken from: https://learnopengl.com/Advanced-Lighting/Bloom
    float l_brightness = dot(l_textCol.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(l_brightness >= UBrightnessThresh )
        ColourOut = vec4(l_textCol.rgb, 1.0);
    else
        ColourOut = vec4(0.0, 0.0, 0.0, 1.0);

}