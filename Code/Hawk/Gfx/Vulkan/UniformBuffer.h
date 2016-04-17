#pragma once
#include "System.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;

template <class T>
class UniformBuffer
{
public:
	UniformBuffer(std::shared_ptr<Device> p_Device)
	: m_Device(p_Device)
	, m_BufferInfo {}
	{
		m_BufferInfo.range = VK_WHOLE_SIZE;

		VkBufferCreateInfo l_Info = {};
		l_Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		l_Info.size = T::Size();
		l_Info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		// Note: Queue family index params ignored if sharingMode not set to VK_SHARING_MODE_CONCURRENT

		VK_THROW_IF_NOT_SUCCESS(vkCreateBuffer(m_Device->GetHandle(), &l_Info, nullptr, &m_BufferInfo.buffer), "Failed to create uniform buffer");

		VkMemoryRequirements l_MemReq = {};
		vkGetBufferMemoryRequirements(m_Device->GetHandle(), m_BufferInfo.buffer, &l_MemReq);

		VkMemoryAllocateInfo l_AllocateInfo = {};
		l_AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		l_AllocateInfo.allocationSize = l_MemReq.size;

		THROW_IF_NOT(m_Device->GetPhysicalDevice()->TryGetMemoryTypeIndex(l_MemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, l_AllocateInfo.memoryTypeIndex), "Failed to find suitable memory type for uniform buffer allocation");
		VK_THROW_IF_NOT_SUCCESS(vkAllocateMemory(m_Device->GetHandle(), &l_AllocateInfo, nullptr, &m_DeviceMemory), "Failed to allocate memory for uniform buffer");

		VK_THROW_IF_NOT_SUCCESS(vkBindBufferMemory(m_Device->GetHandle(), m_BufferInfo.buffer, m_DeviceMemory, 0), "Failed to bind memory to uniform buffer");
		
		LOG("UniformBuffer created. AllocatedMemory=" << l_MemReq.size << " BufferSize=" << T::Size(), "vulkan", Debug);
	}

	~UniformBuffer()
	{
		vkFreeMemory(m_Device->GetHandle(), m_DeviceMemory, nullptr);
		vkDestroyBuffer(m_Device->GetHandle(), m_BufferInfo.buffer, nullptr);
	}

	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;

	void Update()
	{
		void* l_Dest = nullptr;
		VK_THROW_IF_NOT_SUCCESS(vkMapMemory(m_Device->GetHandle(), m_DeviceMemory, 0, VK_WHOLE_SIZE, 0, &l_Dest), "Failed to map uniform buffer memory");
		std::memcpy(l_Dest, &m_Data, static_cast<size_t>(T::Size()));
		vkUnmapMemory(m_Device->GetHandle(), m_DeviceMemory);
	}

	const VkDescriptorBufferInfo* GetDescriptorBufferInfo() const
	{
		return &m_BufferInfo;
	}

	T& GetData()
	{
		return m_Data;
	}

private:
	T m_Data;
	std::shared_ptr<Device> m_Device;
	VkDescriptorBufferInfo m_BufferInfo;
	VkDeviceMemory m_DeviceMemory;
};

}
}
}