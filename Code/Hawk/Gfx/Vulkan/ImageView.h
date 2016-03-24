#pragma once
#include "System.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class ImageView final
{
public:
	ImageView(std::shared_ptr<Device> p_Device, VkImage p_Image, VkFormat p_Format);
	~ImageView();
	ImageView(const ImageView&) = delete;
	ImageView& operator=(const ImageView&) = delete;

	VkImageView GetHandle() const;

private:
	std::shared_ptr<Device> m_Device;
	VkImageView m_Handle;
};

}
}
}