#pragma once
#include "System.h"
#include "Device.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;

class IndexBuffer final
{
public:
	IndexBuffer(std::shared_ptr<Device> p_Device, const std::vector<uint16>& p_Indices);
	IndexBuffer(std::shared_ptr<Device> p_Device, const std::vector<uint32>& p_Indices);
	~IndexBuffer();

	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	VkBuffer GetHandle() const;
	uint32 GetCount() const;
	VkIndexType GetIndexType() const;

private:
	void Init(VkDeviceSize p_BufferSize, void* p_Data);

	std::shared_ptr<Device> m_Device;
	VkBuffer m_Handle;
	VkDeviceMemory m_DeviceMemory;
	uint32 m_uiCount;
	VkIndexType m_IndexType;
};

}
}
}