#include "pch.h"
#include "IndexBuffer.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

IndexBuffer::IndexBuffer(std::shared_ptr<Device> p_Device, const std::vector<uint32>& p_Indices)
: m_Device(p_Device)
, m_uiCount(static_cast<uint32>(p_Indices.size()))
{
	const VkDeviceSize l_OriginalSize = m_uiCount * sizeof(uint32);

	VkBufferCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	l_Info.size = l_OriginalSize;
	l_Info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	// Note: Queue family index params ignored if sharingMode not set to VK_SHARING_MODE_CONCURRENT

	VK_THROW_IF_NOT_SUCCESS(vkCreateBuffer(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create index buffer");

	VkMemoryRequirements l_MemReq = {};
	vkGetBufferMemoryRequirements(m_Device->GetHandle(), m_Handle, &l_MemReq);

	VkMemoryAllocateInfo l_AllocateInfo = {};
	l_AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	l_AllocateInfo.allocationSize = l_MemReq.size;

	THROW_IF_NOT(m_Device->GetPhysicalDevice()->TryGetMemoryTypeIndex(l_MemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, l_AllocateInfo.memoryTypeIndex), "Failed to find suitable memory type for index buffer allocation");
	VK_THROW_IF_NOT_SUCCESS(vkAllocateMemory(m_Device->GetHandle(), &l_AllocateInfo, nullptr, &m_DeviceMemory), "Failed to allocate memory for index buffer");

	void* l_Data = nullptr;
	VK_THROW_IF_NOT_SUCCESS(vkMapMemory(m_Device->GetHandle(), m_DeviceMemory, 0, VK_WHOLE_SIZE, 0, &l_Data), "Failed to map index buffer memory");
	std::memcpy(l_Data, p_Indices.data(), l_MemReq.size);
	vkUnmapMemory(m_Device->GetHandle(), m_DeviceMemory);

	VK_THROW_IF_NOT_SUCCESS(vkBindBufferMemory(m_Device->GetHandle(), m_Handle, m_DeviceMemory, 0), "Failed to bind memory to index buffer");
	LOG("IndexBuffer created of size=" << l_MemReq.size << " original size=" << l_OriginalSize << " num indices=" << p_Indices.size(), "vulkan", Debug);
}

IndexBuffer::~IndexBuffer()
{
	vkFreeMemory(m_Device->GetHandle(), m_DeviceMemory, nullptr);
	vkDestroyBuffer(m_Device->GetHandle(), m_Handle, nullptr);
}

VkBuffer IndexBuffer::GetHandle() const
{
	return m_Handle;
}
uint32 IndexBuffer::GetCount() const
{
	return m_uiCount;
}

}
}
}