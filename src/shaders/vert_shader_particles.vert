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


// ATTRIBUTE INPUT (i.e., vertex buffer data)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;


// OUTPUT 
layout(location = 0) out vec3 fragColor;


void main() 
{
    // size of rasterized points in pixels
	gl_PointSize = 100.0;

    gl_Position = vec4(inPosition.xyz, 1.0);
    fragColor = inColor.rgb;
}
