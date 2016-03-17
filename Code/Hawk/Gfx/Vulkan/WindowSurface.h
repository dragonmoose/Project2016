#pragma once
#include "System.h"
#include "DeviceCreateInfo.h"
#include "Instance.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Queue;

class WindowSurface final
{
public:
#ifdef VK_USE_PLATFORM_WIN32_KHR
	WindowSurface(std::shared_ptr<Instance> p_Instance, VkPhysicalDevice p_PhysicalDevice, const Queue* p_PresentationQueue);
#endif
	~WindowSurface();
	WindowSurface(const WindowSurface&) = delete;
	WindowSurface& operator=(const WindowSurface&) = delete;
	VkSurfaceKHR GetHandle() const;
	VkExtent2D GetInitialExtent() const;
	VkSurfaceTransformFlagBitsKHR GetInitialTransform() const;

private:
	void CheckWSISupport(VkPhysicalDevice p_PhysicalDevice, const Queue* p_PresentationQueue) const;
	void CheckAndSetCapabilities(VkPhysicalDevice p_PhysicalDevice);
	void CheckColorFormats(VkPhysicalDevice p_PhysicalDevice) const;
	void CheckPresentationModes(VkPhysicalDevice p_PhysicalDevice) const;

	VkSurfaceKHR m_Surface;
	std::shared_ptr<Instance> m_Instance;
	VkExtent2D m_InitialExtent;
	VkSurfaceTransformFlagBitsKHR m_InitialTransform;

};
}
}
}