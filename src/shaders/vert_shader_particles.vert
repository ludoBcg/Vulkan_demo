/*********************************************************************************************************************
 *
 * vert_shader_particles.vert
 *
 * Vertex shader for point particles
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/


#version 450


// UNIFORMS INPUT  (set = 0 is optionnal, only used in case of multiple descriptor sets)
layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    float deltaTime;
    float windX;
} ubo;


// ATTRIBUTE INPUT (i.e., vertex buffer data)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;


// OUTPUT 
layout(location = 0) out vec3 fragColor;


void main() 
{
    // size of rasterized points in pixels
	gl_PointSize = 10.0;

    gl_Position = vec4(inPosition.xyz, 1.0);

    // use depth as color
    fragColor = vec3(1.0 - gl_Position.z) * 300.0;
}
