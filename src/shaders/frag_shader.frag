#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragLightDir;

// sampler uniform (texture)
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = vec4(fragColor * texture(texSampler, fragTexCoord/* * 2.0*/).rgb, 1.0);
    //outColor = vec4(0.5 * fragNormal + 0.5, 1.0); // display normals

    vec4 amb = outColor * 0.05; // ambient color
    vec4 diff = outColor * max(0.0, dot(fragNormal, fragLightDir)); // diffuse color
    outColor = amb + diff;
    outColor.a = 1.0;
}