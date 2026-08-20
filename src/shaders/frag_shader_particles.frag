/*********************************************************************************************************************
 *
 * frag_shader_particles.frag
 *
 * Fragment shader for point particles
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/


#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() 
{
    // gl_PointCoord contains the coordinate of a fragment within a point 
    // vec2 in range [0, 1]
    vec2 coord = gl_PointCoord - vec2(0.5);

    // discard out-of-radius fragments to paint circular points instead of squares
    if( 0.5 - length(coord) < 0.0)
        discard;

    outColor = vec4(fragColor.rgb, 1.0);
}