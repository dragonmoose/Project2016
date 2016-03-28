#include "pch.h"
#include "CmdImageMemoryBarrier.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

CmdImageMemoryBarrier::CmdImageMemoryBarrier(TransferOp p_TransferOp, VkImage p_Image)
: m_Barrier {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER}
{
	VkImageLayout l_OldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageLayout l_NewLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkAccessFlags l_SrcAccessMask = {};
	VkAccessFlags l_DestAccessMask = {};
	VkImageAspectFlags l_AspectMask = {};
	GetFlags(p_TransferOp, l_OldLayout, l_NewLayout, l_SrcAccessMask, l_DestAccessMask, l_AspectMask);
	
	m_Barrier.image = p_Image;
	m_Barrier.srcAccessMask = l_SrcAccessMask;
	m_Barrier.dstAccessMask = l_DestAccessMask;
	m_Barrier.oldLayout = l_OldLayout;
	m_Barrier.newLayout = l_NewLayout;
	m_Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;		// No transfer of ownership between queues
	m_Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	m_Barrier.subresourceRange.aspectMask = l_AspectMask;
	m_Barrier.subresourceRange.baseMipLevel = 0;
	m_Barrier.subresourceRange.levelCount = 1;
	m_Barrier.subresourceRange.baseArrayLayer = 0;
	m_Barrier.subresourceRange.layerCount = 1;
}

void CmdImageMemoryBarrier::SetImage(VkImage p_Image)
{
	m_Barrier.image = p_Image;
}

void CmdImageMemoryBarrier::Issue(VkCommandBuffer p_CommandBufferHandle) const
{
	ASSERT(m_Barrier.image, "No image set");
	vkCmdPipelineBarrier(p_CommandBufferHandle, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr, 0, nullptr, 1, &m_Barrier);
}

void CmdImageMemoryBarrier::GetFlags(TransferOp p_TransferOp, VkImageLayout& l_OldLayout, VkImageLayout& l_NewLayout, VkAccessFlags& l_SrcAccessMask, VkAccessFlags& l_DestAccessMask, VkImageAspectFlags& l_AspectMask) const
{
	switch (p_TransferOp)
	{
		case TransferOp::Color_UndefinedToPresent:
			l_OldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			l_NewLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			l_SrcAccessMask = 0; // TODO: Correct usage? (According to spec, needs to be a valid combination of access flags, but LunarG SDK samples sets 0 here)
			l_DestAccessMask = VK_ACCESS_MEMORY_READ_BIT; // Access is a read from a non-specific unit attached to the memory
			l_AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		case TransferOp::Color_WriteToPresent:
			l_OldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			l_NewLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			l_SrcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			l_DestAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			l_AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		case TransferOp::Color_PresentToWrite:
			l_OldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			l_NewLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			l_SrcAccessMask = VK_ACCESS_MEMORY_READ_BIT; // TODO: Correct usage? (LunarG SDK samples sets 0 here, but VK_ACCESS_MEMORY_READ_BIT when transferring to present, as above)
			l_DestAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			l_AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		case TransferOp::DepthStencil_UndefinedToWrite:
			l_OldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			l_NewLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			l_SrcAccessMask = 0; // TODO: Correct usage? (According to spec, needs to be a valid combination of access flags, but LunarG SDK samples sets 0 here)
			l_DestAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			l_AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT; // TODO: Will use stencil buffer? Otheriwise remove, perhaps make stencil buffer usage configurable
			break;
		default:
			THROW("Invalid transfer operation for image memory barrier=" << (uint32)p_TransferOp);
	}
}
}
}
}