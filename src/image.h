/*********************************************************************************************************************
 *
 * image.h
 *
 * Image class to store 2D images
 * Used to manage textures, depth buffer, color buffer for multisampling ...
 * Can load an image from a png file, using stb lib
 *
 * Based on: https://vulkan-tutorial.com/
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H


#include "utils.h"

namespace VulkanDemo
{

class Context;

class Image
{
    

public:

    Image() = default;

    Image(Image const& _other) = default;

    Image& operator=(Image const& _other)
    {
        m_image = _other.m_image;
        m_imageMemory = _other.m_imageMemory;
        m_imageView = _other.m_imageView;
        m_mipLevels = _other.m_mipLevels;
        m_sampler = _other.m_sampler;
        return *this;
    }

    Image(Image&& _other)
        : m_image(_other.m_image)
        , m_imageMemory(_other.m_imageMemory)
        , m_imageView(_other.m_imageView)
        , m_mipLevels(_other.m_mipLevels)
        , m_sampler(_other.m_sampler)
    {}

    Image& Image::operator=(Image&& _other)
    {
        m_image = _other.m_image;
        m_imageMemory = _other.m_imageMemory;
        m_imageView = _other.m_imageView;
        m_mipLevels = _other.m_mipLevels;
        m_sampler = _other.m_sampler;
        return *this;
    }

    virtual ~Image() {};


    VkImage const getImage() const { return m_image; }
    VkDeviceMemory const getImageMemory() const { return m_imageMemory; }
    VkImageView getImageView() { return m_imageView; }
    uint32_t const getMiplevels() const { return m_mipLevels; }
    VkSampler const getSampler() const { return m_sampler; }

    void cleanup(Context& _context);

    void createImageView(Context& _context, VkFormat _format, VkImageAspectFlags _aspectFlags);

    void createTextureSampler(Context& _context);
    void createTextureImage(Context& _context);
    void createTextureImageView(Context& _context);

    // called in createTextureImage()
    void createImage(Context& _context,
                     uint32_t _width, uint32_t _height,
                     VkSampleCountFlagBits _numSamples, VkFormat _format,
                     VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties);
    void transitionImageLayout(Context& _context,
                               VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout);
    void copyBufferToImage(Context& _context,
                           VkBuffer _buffer, uint32_t _width, uint32_t _height);
    void generateMipmaps(Context& _context,
                         VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight);


protected:

    VkImage m_image;
    VkDeviceMemory m_imageMemory;
    VkImageView m_imageView;
    uint32_t m_mipLevels = 1; // modified in createTextureImage() to match texture, stays 1 otherwise
    VkSampler m_sampler = nullptr;

}; // class Image

} // namespace VulkanDemo

#endif // IMAGE_H