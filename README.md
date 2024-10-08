# Vulkan_demo
Base code for a Vulkan project

Based on: https://vulkan-tutorial.com


## Compilation

Developed with VisualStudio 2022


Requires the installation of Vulkan SDK: https://vulkan.lunarg.com/sdk


GLtools.h and other libraries are provided in the [libs](https://github.com/ludoBcg/libs) repository. Make sure that the CMake scripts provided points to the correct directory, then use it to generate a project.

External dependencies used for this project are:

* [GLFW (Graphics Library Framework)](https://www.glfw.org/)
  
* [GLM (OpenGL Mathematics)](https://github.com/g-truc/glm)

* [stb library](https://github.com/nothings/stb)
  
* [tinyobjloader](https://github.com/syoyo/tinyobjloader)


To compile the shaders, a *compile.bat* script can be created in the *src/shaders* folder, containing the following commands:

```
Your/Path/To/VulkanSDK/1.3.250.1/Bin/glslc.exe vert_shader.vert -o vert.spv
Your/Path/To/VulkanSDK/1.3.250.1/Bin/glslc.exe frag_shader.frag -o frag.spv
pause
```


## Other resources

https://renderdoc.org/vulkan-in-30-minutes.html


Data:

[Viking room model](https://sketchfab.com/3d-models/viking-room-a49f1b8e4f5c4ecf9e1fe7d81915ad38) by [nigelgoh](https://sketchfab.com/nigelgoh)
