#pragma once
#include "VlkSystem.h"
#include "VlkDeviceCreateInfo.h"
#include "VlkInstance.h"

namespace Hawk {
namespace Gfx {

class VlkQueue;

class VlkWindowSurface final
{
public:
#ifdef VK_USE_PLATFORM_WIN32_KHR
	VlkWindowSurface(std::shared_ptr<VlkInstance> p_Instance, VkPhysicalDevice p_PhysicalDevice, const VlkQueue* p_PresentationQueue);
#endif
	~VlkWindowSurface();
	VlkWindowSurface(const VlkWindowSurface&) = delete;
	VlkWindowSurface& operator=(const VlkWindowSurface&) = delete;
	VkSurfaceKHR GetHandle() const;
	VkExtent2D GetInitialExtent() const;
	VkSurfaceTransformFlagBitsKHR GetInitialTransform() const;

private:
	void CheckWSISupport(VkPhysicalDevice p_PhysicalDevice, const VlkQueue* p_PresentationQueue) const;
	void CheckAndSetCapabilities(VkPhysicalDevice p_PhysicalDevice);
	void CheckColorFormats(VkPhysicalDevice p_PhysicalDevice) const;
	void CheckPresentationModes(VkPhysicalDevice p_PhysicalDevice) const;

	VkSurfaceKHR m_Surface;
	std::shared_ptr<VlkInstance> m_Instance;
	VkExtent2D m_InitialExtent;
	VkSurfaceTransformFlagBitsKHR m_InitialTransform;

};
}
}