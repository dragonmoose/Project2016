#pragma once
#include "System.h"
#include "VertexDecl.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class VertexBuffer final
{
public:
	template<class T>
	VertexBuffer(std::shared_ptr<Device> p_Device, const std::vector<T>& p_Vertices)
	: m_Device(p_Device)
	, m_Handle(VK_NULL_HANDLE)
	, m_DeviceMemory(VK_NULL_HANDLE)
	{
		const VkDeviceSize l_BufferSize = p_Vertices.size() * T::Size();
		VkBufferCreateInfo l_Info = {};
		l_Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		l_Info.size = l_BufferSize;
		l_Info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		// Note: Queue family index params ignored if sharingMode not set to VK_SHARING_MODE_CONCURRENT
		
		VK_THROW_IF_NOT_SUCCESS(vkCreateBuffer(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create vertex buffer");
		
		VkMemoryRequirements l_MemReq = {};
		vkGetBufferMemoryRequirements(m_Device->GetHandle(), m_Handle, &l_MemReq);

		VkMemoryAllocateInfo l_AllocateInfo = {};
		l_AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		l_AllocateInfo.allocationSize = l_MemReq.size;

		THROW_IF_NOT(m_Device->GetPhysicalDevice()->TryGetMemoryTypeIndex(l_MemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, l_AllocateInfo.memoryTypeIndex), "Failed to find suitable memory type for vertex buffer allocation");
		VK_THROW_IF_NOT_SUCCESS(vkAllocateMemory(m_Device->GetHandle(), &l_AllocateInfo, nullptr, &m_DeviceMemory), "Failed to allocate memory for vertex buffer");

		void* l_Data = nullptr;
		VK_THROW_IF_NOT_SUCCESS(vkMapMemory(m_Device->GetHandle(), m_DeviceMemory, 0, VK_WHOLE_SIZE, 0, &l_Data), "Failed to map vertex buffer memory");
		std::memcpy(l_Data, p_Vertices.data(), static_cast<size_t>(l_BufferSize));
		vkUnmapMemory(m_Device->GetHandle(), m_DeviceMemory);

		VK_THROW_IF_NOT_SUCCESS(vkBindBufferMemory(m_Device->GetHandle(), m_Handle, m_DeviceMemory, 0), "Failed to bind memory to vertex buffer");
		LOG("VertexBuffer created. AllocatedMemory=" << l_MemReq.size << " BufferSize=" << l_BufferSize << " Num vertices=" << p_Vertices.size(), "vulkan", Debug);
	}

	~VertexBuffer();

	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	VkBuffer GetHandle() const;
	void Issue(VkCommandBuffer p_CommandBufferHandle) const;

private:

	std::shared_ptr<Device> m_Device;
	VkBuffer m_Handle;
	VkDeviceMemory m_DeviceMemory;
};

}
}
}