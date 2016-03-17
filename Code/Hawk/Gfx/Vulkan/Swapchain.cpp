#include "pch.h"
#include "Swapchain.h"
#include "Constants.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

Swapchain::Swapchain(std::shared_ptr<Instance> p_Instance, std::shared_ptr<Device> p_Device)
: m_Device(p_Device)
, m_Handle(VK_NULL_HANDLE)
, m_Queue(p_Device->GetPresentationQueue())
, m_uiCurrentBufferIndex(0)
{
	CreateSurface(p_Instance, p_Device->GetPhysicalDevice()->GetHandle());

	VkSwapchainCreateInfoKHR l_Info = {};
	GetCreateInfo(l_Info);

	VK_THROW_IF_NOT_SUCCESS(vkCreateSwapchainKHR(p_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create swapchain");

	InitPresentInfo();

	LOG("Swapchain created", "vulkan", Debug);
}

Swapchain::~Swapchain()
{
	// TODO: Need to make sure images are no longer in use before destroying swap chain
	vkDestroySwapchainKHR(m_Device->GetHandle(), m_Handle, nullptr);
	LOG("Swapchain destroyed", "vulkan", Debug);
}

void Swapchain::Present()
{
	// TODO: Act upon error messages
	m_PresentInfo.pImageIndices = &m_uiCurrentBufferIndex;
	VkResult l_Result = vkQueuePresentKHR(m_Queue->GetHandle(), &m_PresentInfo);
	LOG_IF(l_Result == VK_SUBOPTIMAL_KHR, "Queue presentation suboptimal", "vulkan", Warning);
	VK_THROW_IF_ERR(l_Result, "Queue presentation failed");
}

void Swapchain::CreateSurface(std::shared_ptr<Instance> p_Instance, VkPhysicalDevice p_PhysicalDevice)
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
	m_Surface = std::make_unique<WindowSurface>(p_Instance, p_PhysicalDevice, m_Queue.get());
#endif
}

void Swapchain::GetCreateInfo(VkSwapchainCreateInfoKHR& p_Info) const
{
	ASSERT(m_Surface, "Surface not created");
	p_Info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	p_Info.surface = m_Surface->GetHandle();
	p_Info.minImageCount = Constants::c_uiNumBackBuffers;
	p_Info.imageFormat = Constants::c_BackBufferFormat;
	p_Info.imageColorSpace = Constants::c_BackBufferColorSpace;
	p_Info.imageExtent = m_Surface->GetInitialExtent();
	p_Info.imageArrayLayers = 1; // Should be 1 for non-stereoscopic views
	p_Info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	p_Info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Only one queue family at a time may access, should be faster than concurrent mode. Refer to spec section 11.7 for ownership transfer info
	p_Info.preTransform = m_Surface->GetInitialTransform();
	p_Info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	p_Info.presentMode = Constants::c_PresentationMode;
	p_Info.clipped = VK_TRUE; // Do not render obscured pixels, may increase performance. Pixels on presentable images should not be read back.
}

void Swapchain::InitPresentInfo()
{
	ASSERT(m_Handle, "Internal swapchain null");

	m_PresentInfo = {};
	m_PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	m_PresentInfo.swapchainCount = 1;
	m_PresentInfo.pSwapchains = &m_Handle;
}

}
}
}