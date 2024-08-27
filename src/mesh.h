/*********************************************************************************************************************
 *
 * mesh.h
 *
 * Mesh class to store geometry and handle vertex and index buffers
 * Can create a mesh from a Wavefront (.obj) file using tinyobjloader lib, or build a default geometry (quads)
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

class Device;

class Mesh
{
    

public:

    Mesh() = default;

    Mesh(Mesh const& _other) = default;

    Mesh& operator=(Mesh const& _other)
    {
        m_vertices = _other.m_vertices;
        m_indices = _other.m_indices;
        m_vertexBuffer = _other.m_vertexBuffer;
        m_vertexBufferMemory = _other.m_vertexBufferMemory;
        m_indexBuffer = _other.m_indexBuffer;
        m_indexBufferMemory = _other.m_indexBufferMemory;
        return *this;
    }

    Mesh(Mesh&& _other)
        : m_vertices(std::move(_other.m_vertices))
        , m_indices(std::move(_other.m_indices))
        , m_vertexBuffer(_other.m_vertexBuffer)
        , m_vertexBufferMemory(_other.m_vertexBufferMemory)
        , m_indexBuffer(_other.m_indexBuffer)
        , m_indexBufferMemory(_other.m_indexBufferMemory)
    {}

    Mesh& Mesh::operator=(Mesh&& _other)
    {
        m_vertices = std::move(_other.m_vertices);
        m_indices = std::move(_other.m_indices);
        m_vertexBuffer = _other.m_vertexBuffer;
        m_vertexBufferMemory = _other.m_vertexBufferMemory;
        m_indexBuffer = _other.m_indexBuffer;
        m_indexBufferMemory = _other.m_indexBufferMemory;
        return *this;
    }

    virtual ~Mesh() {};


    std::vector<Vertex> const& getVertices() const { return m_vertices; }
    std::vector<uint32_t> const& getIndices() const { return m_indices; }
    VkBuffer const getVertexBuffer() const { return m_vertexBuffer; }
    VkDeviceMemory const& getVertexBufferMemory() const { return m_vertexBufferMemory; }
    VkBuffer const getIndexBuffer() const { return m_indexBuffer; }
    VkDeviceMemory const getIndexBufferMemory() const { return m_indexBufferMemory; }


    void cleanup(Device& _device);

    void createQuads();
    void loadModel();

    void createVertexBuffer(Device& _device);
    void createIndexBuffer(Device& _device);

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