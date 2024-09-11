#version 450


// UNIFORMS INPUT  (set = 0 is optionnal, only used in case of multiple descriptor sets)
layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

// ATTRIBUTE INPUT (i.e., vertex buffer data)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

// OUTPUT 
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragLightDir;

void main() 
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    //gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;

    fragNormal = inNormal; // normal in model space
    vec4 lightPos = inverse(ubo.view * ubo.model) * vec4(2.0, 0.0, 0.0, 1.0); // light position (next to the camera) in model space
    fragLightDir = normalize(lightPos.rgb - inPosition); // light direction vector
    
}
