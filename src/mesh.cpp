/*********************************************************************************************************************
 *
 * mesh.cpp
 *
 * Based on: https://vulkan-tutorial.com/
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "mesh.h"
#include "context.h"


namespace VulkanDemo
{


/*
 * Destroyes buffers and frees memory 
 */
void Mesh::cleanup(Context& _context)
{
    vkDestroyBuffer(_context.getDevice(), m_indexBuffer, nullptr);
    vkFreeMemory(_context.getDevice(),m_indexBufferMemory, nullptr);

    vkDestroyBuffer(_context.getDevice(),m_vertexBuffer, nullptr);
    vkFreeMemory(_context.getDevice(), m_vertexBufferMemory, nullptr);
}

/*
 * Creates 2 colored quads
 */
void Mesh::createQuads()
{
    m_vertices.clear();
    m_indices.clear();

    // list of vertices for 2 quads, made of 2 triangles each
    // 2 -- 3
    // | \  |
    // |  \ |
    // 1 -- 0 
    m_vertices = {
        { { 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} }, // R
        { {-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} }, // G
        { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} }, // B
        { { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} }, // W

        { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} }, // R
        { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} }, // G
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} }, // B
        { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} }  // W
    };

    // list of indices
    m_indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };
}


/*
 * Loads wavefront model
 */
void Mesh::loadModel()
{
    m_vertices.clear();
    m_indices.clear();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) 
    {
        for (const auto& index : shape.mesh.indices) 
        {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.normal = {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                m_vertices.push_back(vertex);
            }
            
            m_indices.push_back(uniqueVertices[vertex]);
        }
    }
    infoLog() << "number of unique vertices: " + std::to_string(uniqueVertices.size());
}


/*
 * Creation of vertex buffer
 */
void Mesh::createVertexBuffer(Context& _context)
{
    VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

    // Init temporary CPU buffer (stagingBuffer) with associated memory storage (stagingBufferMemory)
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer( _context.getPhysicalDevice(), _context.getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    // map memory buffer (data) with stagingBufferMemory
    void* data;
    vkMapMemory(_context.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    // fill-in data  with m_vertices content
    memcpy(data, m_vertices.data(), (size_t)bufferSize);
    // unmap, now that stagingBufferMemory contains m_vertices data
    vkUnmapMemory(_context.getDevice(), stagingBufferMemory);

    // Init actual vertex buffer (m_vertexBuffer) with associated memory storage (m_vertexBufferMemory)
    createBuffer( _context.getPhysicalDevice(), _context.getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 m_vertexBuffer, m_vertexBufferMemory);
    // data is copied from stagingBuffer to m_vertexBuffer
    copyBuffer(_context.getDevice(), _context.getCommandPool(), _context.getGraphicsQueue(), stagingBuffer, m_vertexBuffer, bufferSize);

    // cleanup temporary data after copy
    vkDestroyBuffer(_context.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(_context.getDevice(), stagingBufferMemory, nullptr);
}


/*
 * Creation of index buffer
 */
void Mesh::createIndexBuffer(Context& _context)
{
    VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

    // temporary CPU buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer( _context.getPhysicalDevice(), _context.getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(_context.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_indices.data(), (size_t)bufferSize);
    vkUnmapMemory(_context.getDevice(), stagingBufferMemory);

    // actual index buffer
    createBuffer( _context.getPhysicalDevice(), _context.getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 m_indexBuffer, m_indexBufferMemory);
    // data is copied from staging buffer
    copyBuffer(_context.getDevice(), _context.getCommandPool(), _context.getGraphicsQueue(), stagingBuffer, m_indexBuffer, bufferSize);

    // cleanup data after copy
    vkDestroyBuffer(_context.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(_context.getDevice(), stagingBufferMemory, nullptr);
}

} // namespace VulkanDemo