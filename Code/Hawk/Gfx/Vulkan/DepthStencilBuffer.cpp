#include "pch.h"
#include "DepthStencilBuffer.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

DepthStencilBuffer::DepthStencilBuffer(std::shared_ptr<Device> p_Device, VkExtent2D p_Extent)
: m_Device(p_Device)
, m_ImageView(VK_NULL_HANDLE)
{
	VkFormat l_Format = p_Device->GetPhysicalDevice()->GetBackBufferDepthStencilFormat();
	VkExtent3D l_Extent = { p_Extent.width, p_Extent.height, 1 };
	m_Image = std::make_unique<Image>(m_Device, l_Format, l_Extent);

	m_ImageView = std::make_shared<ImageView>(m_Device, m_Image->GetHandle(), l_Format);
	LOG("DepthStencilBuffer created", "vulkan", Debug);

}

DepthStencilBuffer::~DepthStencilBuffer()
{
	LOG("DepthStencilBuffer destroyed", "vulkan", Debug);
}



std::shared_ptr<ImageView> DepthStencilBuffer::GetImageView() const
{
	return m_ImageView;
}

}
}
}