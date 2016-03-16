#include "pch.h"
#include "VlkWindowSurface.h"
#include "VlkPhysicalDevice.h"
#include "VlkQueue.h"
#include "VlkConstants.h"
#include "Util/Algorithm.h"
#include "Base/WindowManager.h"

namespace Hawk {
namespace Gfx {

VlkWindowSurface::VlkWindowSurface(std::shared_ptr<VlkInstance> p_Instance, VkPhysicalDevice p_PhysicalDevice, const VlkQueue* p_PresentationQueue)
: m_Surface(VK_NULL_HANDLE)
, m_Instance(p_Instance)
{
	ASSERT(m_Instance, "Instance null");
	VkWin32SurfaceCreateInfoKHR l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	l_Info.hinstance = WindowManager::GetHInstance();
	l_Info.hwnd = WindowManager::GetHWND();

	VK_THROW_IF_NOT_SUCCESS(vkCreateWin32SurfaceKHR(p_Instance->GetHandle(), &l_Info, nullptr, &m_Surface), "Failed to create win32 surface");

	CheckWSISupport(p_PhysicalDevice, p_PresentationQueue);
	CheckAndSetCapabilities(p_PhysicalDevice);
	CheckColorFormats(p_PhysicalDevice);
	CheckPresentationModes(p_PhysicalDevice);
	LOG("Window surface created", "vulkan", Debug);
}

VlkWindowSurface::~VlkWindowSurface()
{
	// All VkSwapchainKHR objects must be destroyed prior to destroying surface
	ASSERT(m_Instance, "Instance null");
	ASSERT(m_Surface, "No surface");
	vkDestroySurfaceKHR(m_Instance->GetHandle(), m_Surface, nullptr);
	LOG("Surface destroyed", "vulkan", Debug);
}

VkSurfaceKHR VlkWindowSurface::GetHandle() const
{
	return m_Surface;
}

VkExtent2D VlkWindowSurface::GetInitialExtent() const
{
	return m_InitialExtent;
}

VkSurfaceTransformFlagBitsKHR VlkWindowSurface::GetInitialTransform() const
{
	return m_InitialTransform;
}

void VlkWindowSurface::CheckWSISupport(VkPhysicalDevice p_PhysicalDevice, const VlkQueue* p_PresentationQueue) const
{
	ASSERT(m_Surface, "Surface not created yet");
	ASSERT(p_PresentationQueue, "PresentationQueue null");

	VkBool32 l_bResult = {};

	uint32_t l_uiFamilyIndex = p_PresentationQueue->GetFamilyIndex();

	VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceSupportKHR(p_PhysicalDevice, l_uiFamilyIndex, m_Surface, &l_bResult), "Failed to check support for WSI");
	THROW_IF_NOT(l_bResult, "WSI-check failed: Queue family " << l_uiFamilyIndex << " does not support the specified surface");

	l_bResult = vkGetPhysicalDeviceWin32PresentationSupportKHR(p_PhysicalDevice, l_uiFamilyIndex);
	THROW_IF_NOT(l_bResult, "WSI-check failed: Queue family " << l_uiFamilyIndex << " does not support presentation on Windows Desktop");
}

void VlkWindowSurface::CheckAndSetCapabilities(VkPhysicalDevice p_PhysicalDevice)
{
	VkSurfaceCapabilitiesKHR l_Capabilities = {};
	VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_PhysicalDevice, m_Surface, &l_Capabilities), "Failed to get surface capabilities");

	THROW_IF_NOT(l_Capabilities.maxImageCount >= VlkConstants::c_uiNumBackBuffers, "Surface does not support the required number of backbuffers. Required=" << VlkConstants::c_uiNumBackBuffers);
	THROW_IF_NOT(l_Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, "Vulkan specification requires surfaces to support color attachment usage");

	m_InitialExtent = l_Capabilities.currentExtent;
	m_InitialTransform = l_Capabilities.currentTransform;
}

void VlkWindowSurface::CheckColorFormats(VkPhysicalDevice p_PhysicalDevice) const
{
	std::vector<VkSurfaceFormatKHR> l_Formats;
	uint32_t l_uiCount = 0;
	VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, m_Surface, &l_uiCount, nullptr), "Failed to get format count");

	l_Formats.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, m_Surface, &l_uiCount, l_Formats.data()), "Failed to get formats");

	THROW_IF(std::find_if(l_Formats.cbegin(), l_Formats.cend(), [](const VkSurfaceFormatKHR& p_Format) { return p_Format.format == VlkConstants::c_BackBufferFormat && p_Format.colorSpace == VlkConstants::c_BackBufferColorSpace; }) == l_Formats.cend(), "Required backbuffer format not supported by surface");
}

void VlkWindowSurface::CheckPresentationModes(VkPhysicalDevice p_PhysicalDevice) const
{
	std::vector<VkPresentModeKHR> l_Modes;
	uint32_t l_uiCount = 0;
	VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR(p_PhysicalDevice, m_Surface, &l_uiCount, nullptr), "Failed to get presentation mode count");

	l_Modes.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR(p_PhysicalDevice, m_Surface, &l_uiCount, l_Modes.data()), "Failed to get presentation modes");

	THROW_IF(std::find_if(l_Modes.cbegin(), l_Modes.cend(), [](const VkPresentModeKHR& p_Mode) { return p_Mode == VK_PRESENT_MODE_FIFO_KHR; }) == l_Modes.cend(), "Vulkan specification requires surfaces to support the FIFO presentation mode");
	THROW_IF(std::find_if(l_Modes.cbegin(), l_Modes.cend(), [](const VkPresentModeKHR& p_Mode) { return p_Mode == VlkConstants::c_PresentationMode; }) == l_Modes.cend(), "Required presentation mode not available for surface: " << VlkConstants::c_PresentationMode);
}
}
}