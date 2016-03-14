#include "pch.h"
#include "VlkSurface.h"

namespace Hawk {
namespace Gfx {

VlkSurface::VlkSurface(VkInstance p_Instance, HINSTANCE p_hInstance, HWND p_hWnd)
: m_Surface(VK_NULL_HANDLE)
, m_Instance(p_Instance)
{
	ASSERT(m_Instance, "Instance null");
	VkWin32SurfaceCreateInfoKHR l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	l_Info.hinstance = p_hInstance;
	l_Info.hwnd = p_hWnd;

	VK_THROW_IF_NOT_SUCCESS(vkCreateWin32SurfaceKHR(p_Instance, &l_Info, nullptr, &m_Surface), "Failed to create win32 surface");
	CheckCapabilities();
}

VlkSurface::~VlkSurface()
{
	// All VkSwapchainKHR must be destroyed prior to destroying surface
	ASSERT(m_Instance, "Instance null");
	ASSERT(m_Surface, "No surface");
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
}

VkSurfaceKHR VlkSurface::GetHandle() const
{
	return m_Surface;
}

void VlkSurface::CheckCapabilities()
{
	//VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities);
}

}
}