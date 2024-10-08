# check CMAKE version
cmake_minimum_required(VERSION 3.0)

# create project
project(Vulkan_demo)

# use C++ 20
set(CMAKE_CXX_STANDARD 20)

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

	
# Add include directories
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/src" )
# Dependencies folder
set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../libs")
include_directories(SYSTEM "${LIBS_DIR}")


############################### EXTERNAL DEPENDENCIES ################


# Vulkan
include_directories("C:/VulkanSDK/1.3.250.1/Include")
link_directories("C:/VulkanSDK/1.3.250.1/Lib")
SET(VULKAN_LIBS vulkan-1.lib)
# flag for conditional compilation
add_compile_definitions(USE_VULKAN)


# GLFW (to compile before)
set(GLFW_DIR "${LIBS_DIR}/third_party/glfw-3.4")
include_directories(${GLFW_DIR}/include)
link_directories(${GLFW_DIR}/build/src/Release)
SET(GLFW_LIBS glfw3.lib)


# GLM (Header only!)
include_directories(SYSTEM "${LIBS_DIR}/third_party/glm-1.0.1")


# STB
include_directories(SYSTEM "${LIBS_DIR}/third_party/stb")


# tinyobjloader
include_directories(SYSTEM "${LIBS_DIR}/third_party/tinyobjloader")


################################# BUILD PROJECT ######################

# Add executable for project
add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${GLFW_LIBS} ${VULKAN_LIBS})

# Install executable
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

