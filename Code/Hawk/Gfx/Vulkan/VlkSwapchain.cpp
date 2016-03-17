#include "pch.h"
#include "VlkSwapchain.h"
#include "VlkConstants.h"

namespace Hawk {
namespace Gfx {

VlkSwapchain::VlkSwapchain(std::shared_ptr<VlkInstance> p_Instance, std::shared_ptr<VlkDevice> p_Device)
: m_Device(p_Device)
, m_Swapchain(VK_NULL_HANDLE)
, m_Queue(p_Device->GetPresentationQueue())
, m_uiCurrentBufferIndex(0)
{
	CreateSurface(p_Instance, p_Device->GetPhysicalDevice()->GetHandle());

	VkSwapchainCreateInfoKHR l_Info = {};
	GetCreateInfo(l_Info);

	VK_THROW_IF_NOT_SUCCESS(vkCreateSwapchainKHR(p_Device->GetHandle(), &l_Info, nullptr, &m_Swapchain), "Failed to create swapchain");

	InitPresentInfo();

	LOG("Swapchain created", "vulkan", Debug);
}

VlkSwapchain::~VlkSwapchain()
{
	// TODO: Need to make sure images are no longer in use before destroying swap chain
	vkDestroySwapchainKHR(m_Device->GetHandle(), m_Swapchain, nullptr);
	LOG("Swapchain destroyed", "vulkan", Debug);
}

void VlkSwapchain::Present()
{
	m_PresentInfo.pImageIndices = &m_uiCurrentBufferIndex;
	VkResult l_Result = vkQueuePresentKHR(m_Queue->GetHandle(), &m_PresentInfo);
	LOG_IF(l_Result == VK_SUBOPTIMAL_KHR, "Queue presentation suboptimal", "vulkan", Warning);
	VK_THROW_IF_ERR(l_Result, "Queue presentation failed");
}

void VlkSwapchain::CreateSurface(std::shared_ptr<VlkInstance> p_Instance, VkPhysicalDevice p_PhysicalDevice)
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
	m_Surface = std::make_unique<VlkWindowSurface>(p_Instance, p_PhysicalDevice, m_Queue.get());
#endif
}

void VlkSwapchain::GetCreateInfo(VkSwapchainCreateInfoKHR& p_Info) const
{
	ASSERT(m_Surface, "Surface not created");
	p_Info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	p_Info.surface = m_Surface->GetHandle();
	p_Info.minImageCount = VlkConstants::c_uiNumBackBuffers;
	p_Info.imageFormat = VlkConstants::c_BackBufferFormat;
	p_Info.imageColorSpace = VlkConstants::c_BackBufferColorSpace;
	p_Info.imageExtent = m_Surface->GetInitialExtent();
	p_Info.imageArrayLayers = 1; // Should be 1 for non-stereoscopic views
	p_Info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	p_Info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Only one queue family at a time may access, should be faster than concurrent mode. Refer to spec section 11.7 for ownership transfer info
	p_Info.preTransform = m_Surface->GetInitialTransform();
	p_Info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	p_Info.presentMode = VlkConstants::c_PresentationMode;
	p_Info.clipped = VK_TRUE; // Do not render obscured pixels, may increase performance. Pixels on presentable images should not be read back.
}

void VlkSwapchain::InitPresentInfo()
{
	ASSERT(m_Swapchain, "Internal swapchain null");

	m_PresentInfo = {};
	m_PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	m_PresentInfo.swapchainCount = 1;
	m_PresentInfo.pSwapchains = &m_Swapchain;
}

}
}