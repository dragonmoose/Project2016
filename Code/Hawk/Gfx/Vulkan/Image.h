#pragma once
#include "System.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class RenderPass;
class ImageView;

class Image final
{
public:
	Image(std::shared_ptr<Device> p_Device, VkFormat p_Format, VkExtent3D p_Extent);
	~Image();
	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;

	VkImage GetHandle() const;

private:
	void CreateImage(VkFormat p_Format, VkExtent3D p_Extent);
	void AllocateMemory();

	std::shared_ptr<Device> m_Device;
	VkImage m_Handle;
	VkDeviceMemory m_DeviceMemory;
};

}
}
}