#pragma once
#include "VlkSystem.h"
#include "VlkDeviceCreateInfo.h"
#include "VlkInstance.h"

namespace Hawk {
namespace Gfx {

class VlkSurface final
{
public:
#ifdef VK_USE_PLATFORM_WIN32_KHR
	VlkSurface(std::shared_ptr<VlkInstance> p_Instance, HINSTANCE p_hInstance, HWND p_hWnd, const VlkDeviceCreateInfo& p_DeviceCreateInfo);
#endif
	~VlkSurface();
	VlkSurface(const VlkSurface&) = delete;
	VlkSurface& operator=(const VlkSurface&) = delete;
	VkSurfaceKHR GetHandle() const;

private:
	void CheckWSISupport(VkPhysicalDevice p_PhysicalDevice, const VlkDeviceCreateInfo::QueueCreateInfoMap_t& p_QueueCreateInfoMap) const;
	void CheckCapabilities(VkPhysicalDevice p_PhysicalDevice) const;
	void CheckColorFormats(VkPhysicalDevice p_PhysicalDevice) const;
	void CheckPresentationModes(VkPhysicalDevice p_PhysicalDevice) const;

	VkSurfaceKHR m_Surface;
	std::shared_ptr<VlkInstance> m_Instance;
};
}
}