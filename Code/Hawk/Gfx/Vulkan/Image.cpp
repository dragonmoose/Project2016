#include "pch.h"
#include "Image.h"
#include "Util.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

Image::Image(std::shared_ptr<Device> p_Device, VkFormat p_Format, VkExtent3D p_Extent)
: m_Device(p_Device)
, m_Handle(VK_NULL_HANDLE)
, m_DeviceMemory(VK_NULL_HANDLE)
{
	CreateImage(p_Format, p_Extent);
	AllocateMemory();

	LOG("Image created", "vulkan", Debug);
}

Image::~Image()
{
	vkFreeMemory(m_Device->GetHandle(), m_DeviceMemory, nullptr);
	vkDestroyImage(m_Device->GetHandle(), m_Handle, nullptr);
	LOG("Image destroyed", "vulkan", Debug);
}

VkImage Image::GetHandle() const
{
	return m_Handle;
}

void Image::CreateImage(VkFormat p_Format, VkExtent3D p_Extent)
{
	VkImageCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	l_Info.imageType = VK_IMAGE_TYPE_2D;
	l_Info.format = p_Format;
	l_Info.extent = p_Extent;
	l_Info.mipLevels = 1;
	l_Info.arrayLayers = 1;
	l_Info.samples = VK_SAMPLE_COUNT_1_BIT;
	l_Info.tiling = VK_IMAGE_TILING_OPTIMAL;

	if (Util::IsColorFormat(p_Format))
	{
		l_Info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	else if (Util::IsDepthStencilFormat(p_Format))
	{
		l_Info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}
	else
	{
		THROW("Format not supported yet");
	}

	VK_THROW_IF_NOT_SUCCESS(vkCreateImage(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create image");
}

void Image::AllocateMemory()
{
	VkMemoryRequirements l_MemReq = {};
	vkGetImageMemoryRequirements(m_Device->GetHandle(), m_Handle, &l_MemReq);

	VkMemoryAllocateInfo l_AllocateInfo = {};
	l_AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	l_AllocateInfo.allocationSize = l_MemReq.size;
	uint32 l_uiMemoryTypeIndex = 0;
	THROW_IF_NOT(m_Device->GetPhysicalDevice()->TryGetMemoryTypeIndex(l_MemReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, l_uiMemoryTypeIndex), "Failed to find suitable memory type for image allocation");
	l_AllocateInfo.memoryTypeIndex = l_uiMemoryTypeIndex;

	VK_THROW_IF_NOT_SUCCESS(vkAllocateMemory(m_Device->GetHandle(), &l_AllocateInfo, nullptr, &m_DeviceMemory), "Failed to allocate memory for image");
	VK_THROW_IF_NOT_SUCCESS(vkBindImageMemory(m_Device->GetHandle(), m_Handle, m_DeviceMemory, 0), "Failed to bind memory to image");

	LOG("Allocated memory for image. SizeInBytes=" << l_MemReq.size << " MemoryTypeIndex=" << l_uiMemoryTypeIndex, "vulkan", Debug);
}

}
}
}