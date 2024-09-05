# check CMAKE version
cmake_minimum_required(VERSION 3.0)

# create project
project(Vulkan_demo)

set(CMAKE_CXX_STANDARD 17)

# add files
set(SRCS
	src/main.cpp
	src/context.cpp
	src/mesh.cpp
	src/image.cpp
	src/demoapp.cpp
    )
    
set(HEADERS
	src/utils.h
	src/context.h
	src/mesh.h
	src/image.h
	src/demoapp.h
    )

	
################################# EXTERNAL LIBS ######################

# Add include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src" )

include_directories("C:/VulkanSDK/1.3.250.1/Include")
link_directories("C:/VulkanSDK/1.3.250.1/Lib")
SET(VULKAN_LIBS vulkan-1.lib)


# GLFW (to compile before)
set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw-3.3.8")
include_directories(${GLFW_DIR}/include)
link_directories(${GLFW_DIR}/build/src/Release)
SET(GLFW_LIBS glfw3.lib)


# GLM (Header only!)
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/glm")


# STB
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")


# tinyobjloader
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/tinyobjloader")


################################# BUILD PROJECT ######################

# Add executable for project
add_executable(${PROJECT_NAME} ${PROJECT_SRCS} ${SRCS} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${GLFW_LIBS} ${VULKAN_LIBS})

# Install executable
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

