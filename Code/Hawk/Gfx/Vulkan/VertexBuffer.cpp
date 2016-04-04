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

}
}
}