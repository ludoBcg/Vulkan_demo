/*********************************************************************************************************************
 *
 * main.cpp
 *
 * Based on: https://vulkan-tutorial.com/
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/



#include <stdexcept>

#include "hellotriangleapp.h"


int main() 
{

    VulkanDemo::HelloTriangleApp app;

    try 
    {
        app.run();
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    //glfwInit();

    //glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    //uint32_t extensionCount = 0;
    //vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    //std::cout << extensionCount << " extensions supported\n";

    //glm::mat4 matrix;
    //glm::vec4 vec;
    //auto test = matrix * vec;

    //while (!glfwWindowShouldClose(window)) {
    //    glfwPollEvents();
    //}

    //glfwDestroyWindow(window);

    //glfwTerminate();

    //return 0;
}