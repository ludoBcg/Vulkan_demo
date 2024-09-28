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

#include <string>

namespace VulkanDemo
{

class Context;


/*
* Structure for vertex attributes
*/
struct Vertex 
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;


    bool operator==(const Vertex& _other) const 
    {
        return pos == _other.pos && color == _other.color && texCoord == _other.texCoord && normal == _other.normal;
    }

    static VkVertexInputBindingDescription getBindingDescription() 
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        // VK_VERTEX_INPUT_RATE_VERTEX = Move to the next data entry after each vertex
        // VK_VERTEX_INPUT_RATE_INSTANCE = Move to the next data entry after each instance

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() 
    {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

        // Attribute description for position
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        // Attribute description for color
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        // Attribute description for UVs
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        // Attribute description for normals
        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, normal);

        return attributeDescriptions;
    }

};


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

    Mesh& operator=(Mesh&& _other)
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


    void cleanup(Context& _context);

    void createQuads();
    void loadModel();

    void createVertexBuffer(Context& _context);
    void createIndexBuffer(Context& _context);

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


//namespace std {
//    template<> struct hash<VulkanDemo::Vertex> {
//        size_t operator()(VulkanDemo::Vertex const& vertex) const {
//            return ((hash<glm::vec3>()(vertex.pos) ^
//                    (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
//                    (hash<glm::vec2>()(vertex.texCoord) << 1);
//        }
//    };
//}
namespace std {
    template<> struct hash<VulkanDemo::Vertex> {
        size_t operator()(VulkanDemo::Vertex const& vertex) const 
        {
            std::size_t h1 = hash<glm::vec3>()(vertex.pos);
            std::size_t h2 = hash<glm::vec3>()(vertex.color);
            std::size_t h3 = hash<glm::vec2>()(vertex.texCoord);
            std::size_t h4 = hash<glm::vec3>()(vertex.normal);
            std::string stg = std::to_string(h1) + std::to_string(h2) + std::to_string(h3) + std::to_string(h4);
            return std::hash<std::string>()(stg);
        }
    };
}

#endif // MESH_H