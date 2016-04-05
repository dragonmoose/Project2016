#include "pch.h"
#include "VertexBuffer.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

VertexBuffer::~VertexBuffer()
{
	vkFreeMemory(m_Device->GetHandle(), m_DeviceMemory, nullptr);
	vkDestroyBuffer(m_Device->GetHandle(), m_Handle, nullptr);
}

VkBuffer VertexBuffer::GetHandle() const
{
	return m_Handle;
}

void VertexBuffer::Issue(VkCommandBuffer p_CommandBufferHandle) const
{
	VkDeviceSize l_Offset = 0;
	vkCmdBindVertexBuffers(p_CommandBufferHandle, 0, 1, &m_Handle, &l_Offset);
}

}
}
}