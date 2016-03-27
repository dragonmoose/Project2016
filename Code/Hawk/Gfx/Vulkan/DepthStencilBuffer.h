#pragma once
#include "System.h"
#include "Device.h"
#include "ImageView.h"
#include "Image.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class DepthStencilBuffer final
{
public:
	DepthStencilBuffer(std::shared_ptr<Device> p_Device, VkExtent2D p_Extent);
	~DepthStencilBuffer();
	DepthStencilBuffer(const DepthStencilBuffer&) = delete;
	DepthStencilBuffer& operator=(const DepthStencilBuffer&) = delete;

	std::shared_ptr<ImageView> GetImageView() const;
	VkImage GetImage() const;

private:
	std::shared_ptr<Device> m_Device;
	std::shared_ptr<ImageView> m_ImageView;
	std::unique_ptr<Image> m_Image;
};

}
}
}