#include "pch.h"
#include "VlkSurface.h"
#include "VlkPhysicalDevice.h"
#include "VlkConstants.h"
#include "Util/Algorithm.h"

namespace Hawk {
namespace Gfx {

VlkSurface::VlkSurface(std::shared_ptr<VlkInstance> p_Instance, HINSTANCE p_hInstance, HWND p_hWnd, const VlkDeviceCreateInfo& p_DeviceCreateInfo)
: m_Surface(VK_NULL_HANDLE)
, m_Instance(p_Instance)
{
	ASSERT(m_Instance, "Instance null");
	VkWin32SurfaceCreateInfoKHR l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	l_Info.hinstance = p_hInstance;
	l_Info.hwnd = p_hWnd;

	VK_THROW_IF_NOT_SUCCESS(vkCreateWin32SurfaceKHR(p_Instance->GetHandle(), &l_Info, nullptr, &m_Surface), "Failed to create win32 surface");

	VkPhysicalDevice l_PhysicalDevice = p_DeviceCreateInfo.GetPhysicalDevice()->GetHandle();

	CheckWSISupport(l_PhysicalDevice, p_DeviceCreateInfo.GetQueueCreateInfoMap());
	CheckCapabilities(l_PhysicalDevice);
	CheckColorFormats(l_PhysicalDevice);
	CheckPresentationModes(l_PhysicalDevice);
}

VlkSurface::~VlkSurface()
{
	// All VkSwapchainKHR must be destroyed prior to destroying surface
	ASSERT(m_Instance, "Instance null");
	ASSERT(m_Surface, "No surface");
	vkDestroySurfaceKHR(m_Instance->GetHandle(), m_Surface, nullptr);
	LOG("Surface destroyed", "vulkan", Debug);
}

VkSurfaceKHR VlkSurface::GetHandle() const
{
	return m_Surface;
}

void VlkSurface::CheckWSISupport(VkPhysicalDevice p_PhysicalDevice, const VlkDeviceCreateInfo::QueueCreateInfoMap_t& p_QueueCreateInfoMap) const
{
	ASSERT(m_Surface, "Surface not created yet");

	auto l_Itr = p_QueueCreateInfoMap.find(VlkQueueType::Graphics);
	ASSERT(l_Itr != p_QueueCreateInfoMap.end(), "No queues requiring graphics found when checking for WSI support");

	std::vector<uint32_t> l_FamilyIndices;
	for (const auto& l_Info : l_Itr->second)
	{
		l_FamilyIndices.push_back(l_Info.m_uiFamilyIndex);
	}

	std::sort(l_FamilyIndices.begin(), l_FamilyIndices.end());
	hwk::unique(l_FamilyIndices);

	for (uint32_t l_uiFamilyIndex : l_FamilyIndices)
	{
		VkBool32 l_bResult = {};
		VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceSupportKHR(p_PhysicalDevice, l_uiFamilyIndex, m_Surface, &l_bResult), "Failed to check support for WSI");
		THROW_IF_NOT(l_bResult, "WSI-check failed: Queue family " << l_uiFamilyIndex << " does not support the specified surface");

		l_bResult = vkGetPhysicalDeviceWin32PresentationSupportKHR(p_PhysicalDevice, l_uiFamilyIndex);
		THROW_IF_NOT(l_bResult, "WSI-check failed: Queue family " << l_uiFamilyIndex << " does not support presentation on Windows Desktop");
	}
}

void VlkSurface::CheckCapabilities(VkPhysicalDevice p_PhysicalDevice) const
{
	VkSurfaceCapabilitiesKHR l_Capabilities = {};
	VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_PhysicalDevice, m_Surface, &l_Capabilities), "Failed to get surface capabilities");

	THROW_IF_NOT(l_Capabilities.maxImageCount >= VlkConstants::c_uiNumBackBuffers, "Surface does not support the required number of backbuffers. Required=" << VlkConstants::c_uiNumBackBuffers);
	THROW_IF_NOT(l_Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, "Vulkan specification requires surfaces to support color attachment usage");
}

void VlkSurface::CheckColorFormats(VkPhysicalDevice p_PhysicalDevice) const
{
	std::vector<VkSurfaceFormatKHR> l_Formats;
	uint32_t l_uiCount = 0;
	VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, m_Surface, &l_uiCount, nullptr), "Failed to get format count");

	l_Formats.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, m_Surface, &l_uiCount, l_Formats.data()), "Failed to get formats");

	THROW_IF(std::find_if(l_Formats.cbegin(), l_Formats.cend(), [](const VkSurfaceFormatKHR& p_Format) { return p_Format.format == VlkConstants::c_BackBufferFormat; }) == l_Formats.cend(), "Required backbuffer format not supported by surface");
}

void VlkSurface::CheckPresentationModes(VkPhysicalDevice p_PhysicalDevice) const
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