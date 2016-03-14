#pragma once
#include "VlkSystem.h"

namespace Hawk {
namespace Gfx {

class VlkSurface final
{
public:
#ifdef VK_USE_PLATFORM_WIN32_KHR
	VlkSurface(VkInstance p_Instance, HINSTANCE p_hInstance, HWND p_hWnd);
#endif
	~VlkSurface();
	VlkSurface(const VlkSurface&) = delete;
	VlkSurface& operator=(const VlkSurface&) = delete;
	VkSurfaceKHR GetHandle() const;

private:
	void CheckCapabilities();

	VkSurfaceKHR m_Surface;
	VkInstance m_Instance;

};
}
}