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
	CreateSurface(p_Instance, p_Device->GetPhysicalDevice().get());
	m_Extent = m_Surface->GetInitialExtent();

	CreateSwapchain();
	GetImages();
	CreateImageViews();
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

void Swapchain::SetCurrImage()
{
	// TODO: Handle the different error messages here
	// TODO: Make use of semaphore to avoid waiting for image
	uint32 l_uiIndex = 0;
	VK_THROW_IF_NOT_SUCCESS(vkAcquireNextImageKHR(m_Device->GetHandle(), m_Handle, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &l_uiIndex), "Failed to acquire next image");
	m_CurrImage = m_Images[l_uiIndex];
}

VkImage Swapchain::GetCurrImage() const
{
	return m_CurrImage;
}

std::shared_ptr<ImageView> Swapchain::GetImageView(uint32 p_uiIndex) const
{
	ASSERT(p_uiIndex < m_ImageViews.size(), "ImageView index out of bounds. Index=" << p_uiIndex << " Size=" << m_ImageViews.size());
	return m_ImageViews[p_uiIndex];
}

VkExtent2D Swapchain::GetExtent() const
{
	return m_Extent;
}

void Swapchain::CreateSurface(std::shared_ptr<Instance> p_Instance, const PhysicalDevice* p_PhysicalDevice)
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
	p_Info.imageFormat = m_Device->GetPhysicalDevice()->GetBackBufferColorFormat();
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

void Swapchain::GetImages()
{
	uint32 l_uiNumImages = 0;
	VK_THROW_IF_NOT_SUCCESS(vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Handle, &l_uiNumImages, nullptr), "Failed to get swapchain image count");
	THROW_IF_NOT(l_uiNumImages == Constants::c_uiNumBackBuffers, "Backbuffer count mismatch");

	m_Images.resize(l_uiNumImages);
	VK_THROW_IF_NOT_SUCCESS(vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Handle, &l_uiNumImages, m_Images.data()), "Failed to get swapchain images");
}

void Swapchain::CreateSwapchain()
{
	VkSwapchainCreateInfoKHR l_Info = {};
	GetCreateInfo(l_Info);
	VK_THROW_IF_NOT_SUCCESS(vkCreateSwapchainKHR(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create swapchain");
}

void Swapchain::CreateImageViews()
{
	m_ImageViews.resize(Constants::c_uiNumBackBuffers);
	for (uint32 i = 0; i < Constants::c_uiNumBackBuffers; i++)
	{
		m_ImageViews[i] = std::make_shared<ImageView>(m_Device, m_Images[i], m_Device->GetPhysicalDevice()->GetBackBufferColorFormat());
	}
}

}
}
}