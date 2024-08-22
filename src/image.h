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
        //m_mipLevels = _other.m_mipLevels;
        m_sampler = _other.m_sampler;
        return *this;
    }

    Image(Image&& _other)
        : m_image(_other.m_image)
        , m_imageMemory(_other.m_imageMemory)
        , m_imageView(_other.m_imageView)
        //, m_mipLevels(_other.m_mipLevels)
        , m_sampler(_other.m_sampler)
    {}

    Image& Image::operator=(Image&& _other)
    {
        m_image = _other.m_image;
        m_imageMemory = _other.m_imageMemory;
        m_imageView = _other.m_imageView;
        //m_mipLevels = _other.m_mipLevels;
        m_sampler = _other.m_sampler;
        return *this;
    }

    virtual ~Image() {};


    VkImage const& getImage() const { return m_image; }
    VkDeviceMemory const& getImageMemory() const { return m_imageMemory; }
    VkImageView /*const&*/ getImageView() /*const*/ { return m_imageView; }
    //uint32_t const& getMiplevels() const { return m_mipLevels; }
    VkSampler const& getSampler() const { return m_sampler; }
    //VkSampler& getSampler() { return m_sampler; }
    //VkSampler* getSamplerPtr() { return &m_sampler; }


    void cleanup(VkDevice& _device);

    void createImage(VkPhysicalDevice& _physicalDevice, VkDevice& _device,
                     uint32_t _width, uint32_t _height, uint32_t _mipLevels,
                     VkSampleCountFlagBits _numSamples, VkFormat _format,
                     VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties);

    void createImageView(VkDevice& _device, VkFormat _format, VkImageAspectFlags _aspectFlags, uint32_t _mipLevels);

    void createTextureSampler(VkPhysicalDevice& _physicalDevice, VkDevice& _device, uint32_t _mipLevels);

protected:

    VkImage m_image;
    VkDeviceMemory m_imageMemory;
    VkImageView m_imageView;
    //uint32_t m_mipLevels;
    VkSampler m_sampler = nullptr;

}; // class Image

} // namespace VulkanDemo

#endif // IMAGE_H