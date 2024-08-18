/*********************************************************************************************************************
 *
 * mesh.h
 *
 * The Vulkan application
 *
 * Based on: https://vulkan-tutorial.com/
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/

#ifndef MESH_H
#define MESH_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "utils.h"

namespace VulkanDemo
{

class Mesh
{
    

public:

    Mesh() = default;

    Mesh(Mesh const& _other) = default;

    Mesh& operator=(Mesh const& _other)
    {
        m_vertices = _other.m_vertices;
        m_indices = _other.m_indices;
        return *this;
    }

    Mesh(Mesh&& _other)
        : m_vertices(std::move(_other.m_vertices))
        , m_indices(std::move(_other.m_indices))
    {}

    Mesh& Mesh::operator=(Mesh&& _other)
    {
        m_vertices = std::move(_other.m_vertices);
        m_indices = std::move(_other.m_indices);
        return *this;
    }

    virtual ~Mesh() {};


    std::vector<Vertex> const& Mesh::getVertices() const { return m_vertices; }
    std::vector<uint32_t> const& Mesh::getIndices() const { return m_indices; }
    VkBuffer const& Mesh::getVertexBuffer() const { return m_vertexBuffer; }
    VkDeviceMemory const& Mesh::getVertexBufferMemory() const { return m_vertexBufferMemory; }
    VkBuffer const& Mesh::getIndexBuffer() const { return m_indexBuffer; }
    VkDeviceMemory const& Mesh::getIndexBufferMemory() const { return m_indexBufferMemory; }


    void cleanup(VkDevice& _device);

    void createQuads();
    void loadModel();

    void createVertexBuffer(VkPhysicalDevice& _physicalDevice, VkDevice& _device, VkCommandPool& _commandPool, VkQueue& _graphicsQueue);
    void createIndexBuffer(VkPhysicalDevice& _physicalDevice, VkDevice& _device, VkCommandPool& _commandPool, VkQueue& _graphicsQueue);


protected:

    // List of vertices
    std::vector<Vertex> m_vertices;
    // List of indices
    std::vector<uint32_t> m_indices;

    // Vertex buffer
    VkBuffer m_vertexBuffer;
    // Handle to the vertex buffer memory
    VkDeviceMemory m_vertexBufferMemory;

    // Index buffer
    VkBuffer m_indexBuffer;
    // Handle to the index buffer memory
    VkDeviceMemory m_indexBufferMemory;


}; // class Mesh

} // namespace VulkanDemo

#endif // MESH_H