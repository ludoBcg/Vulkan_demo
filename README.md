# Vulkan_demo
Base code for a Vulkan project

Based on: https://vulkan-tutorial.com


## Compilation

Developed with VisualStudio 2022


Requires the installation of Vulkan SDK: https://vulkan.lunarg.com/sdk


Create an *external* directory and copy into it the following libraries:

* [GLFW (Graphics Library Framework)](https://www.glfw.org/)

  Download latest version. The library is provided with a Cmake file, use it to generated a VisualStudio solution, then open it, and build
  
* [GLM (OpenGL Mathematics)](https://github.com/g-truc/glm)

  Just download the libary (header only)

* [stb library](https://github.com/nothings/stb) for image loading:

  Just download and include the stb_image.h header
  
* [tinyobjloader](https://github.com/syoyo/tinyobjloader)

  Just download and include the tiny_obj_loader.h header


You can then use the CMakeList provided to generate a project.


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