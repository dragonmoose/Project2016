#include "pch.h"
#include "ImageView.h"
#include "Util.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

ImageView::ImageView(std::shared_ptr<Device> p_Device, VkImage p_Image, VkFormat p_Format)
: m_Device(p_Device)
, m_Handle(VK_NULL_HANDLE)
{
	// TODO: Lots of requirements with regards to formats here etc. See spec 11.5
	VkImageViewCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	l_Info.image = p_Image;
	l_Info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	l_Info.format = p_Format;
	// Note: Not specifying l_Info.components since COMPONENT_SWIZZLE_IDENTITY (0) is equivalent to identity mapping for each component

	VkImageSubresourceRange& l_SubresourceRange = l_Info.subresourceRange;
	if (Util::IsColorFormat(p_Format))
	{
		l_SubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if (Util::IsDepthStencilFormat(p_Format))
	{
		l_SubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else
	{
		THROW("Unsupported format for ImageView. Format=" << p_Format);
	}
	l_SubresourceRange.baseMipLevel = 0;
	l_SubresourceRange.levelCount = 1;
	l_SubresourceRange.baseArrayLayer = 0;
	l_SubresourceRange.layerCount = 1;

	VK_THROW_IF_NOT_SUCCESS(vkCreateImageView(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create ImageView");
	LOG("ImageView created", "vulkan", Debug);
}

ImageView::~ImageView()
{
	vkDestroyImageView(m_Device->GetHandle(), m_Handle, nullptr);
	LOG("ImageView destroyed", "vulkan", Debug);
}

VkImageView ImageView::GetHandle() const
{
	return m_Handle;
}

}
}
}