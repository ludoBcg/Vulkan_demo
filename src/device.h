/*********************************************************************************************************************
 *
 * device.h
 *
 * Class representation of a Vulkan Device
 *
 * Based on: https://vulkan-tutorial.com/
 *
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/

#ifndef DEVICE_H
#define DEVICE_H


#include "utils.h"

namespace VulkanDemo
{


class Device
{
    

public:

    Device() = default;

    Device(Device const& _other) = default;

    Device(VkPhysicalDevice& _physicalDevice, VkDevice& _device, VkCommandPool& _commandPool, VkQueue& _graphicsQueue, VkQueue& _presentQueue)
        : m_physicalDevice(m_physicalDevice)
        , m_device(m_device)                  
        , m_graphicsQueue(m_graphicsQueue)
        , m_presentQueue(m_presentQueue)
        , m_commandPool(m_commandPool)
    {}

    Device& operator=(Device const& _other)
    {
        m_physicalDevice = _other.m_physicalDevice;
        m_device = _other.m_device;                                 
        m_graphicsQueue = _other.m_graphicsQueue;
        m_presentQueue = _other.m_presentQueue;
        m_commandPool = _other.m_commandPool;
        return *this;
    }

    Device(Device&& _other)
        : m_physicalDevice(_other.m_physicalDevice)
        , m_device(_other.m_device)                  
        , m_graphicsQueue(_other.m_graphicsQueue)
        , m_presentQueue(_other.m_presentQueue)
        , m_commandPool(_other.m_commandPool)
    {}

    Device& Device::operator=(Device&& _other)
    {
        m_physicalDevice = _other.m_physicalDevice;
        m_device = _other.m_device;                                 
        m_graphicsQueue = _other.m_graphicsQueue;
        m_presentQueue = _other.m_presentQueue;
        m_commandPool = _other.m_commandPool;
        return *this;
    }

    virtual ~Device() {};


    VkPhysicalDevice& getPhysicalDevice() { return m_physicalDevice; }
    VkDevice& getDevice() { return m_device; }
    VkQueue& getGraphicsQueue() { return m_graphicsQueue; }
    VkQueue& getPresentQueue() { return m_presentQueue; }
    VkCommandPool& getCommandPool() { return m_commandPool; }

    void setPhysicalDevice(VkPhysicalDevice _physicalDevice) { m_physicalDevice = _physicalDevice; }


protected:

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE; // the graphic device
    VkDevice m_device;                                  // logical device handle (i.e., similar to OpenGL context)
    VkQueue m_graphicsQueue;                            // graphics queue handle
    VkQueue m_presentQueue;                             // presentation queue handle
    VkCommandPool m_commandPool;                        // command pool handle


}; // class Device

} // namespace VulkanDemo

#endif // DEVICE_H