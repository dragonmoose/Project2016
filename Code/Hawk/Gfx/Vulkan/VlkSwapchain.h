#include "VlkDevice.h"
#include "VlkWindowSurface.h"
#include <memory>

namespace Hawk {
namespace Gfx {

class VlkSwapchain final
{
public:
	VlkSwapchain(std::shared_ptr<VlkInstance> p_Instance, std::shared_ptr<VlkDevice> p_Device);
	~VlkSwapchain();
	VlkSwapchain(const VlkSwapchain&) = delete;
	VlkSwapchain& operator=(const VlkSwapchain&) = delete;
	void Present();

private:
	void CreateSurface(std::shared_ptr<VlkInstance> p_Instance, VkPhysicalDevice p_PhysicalDevice);
	void GetCreateInfo(VkSwapchainCreateInfoKHR& p_Info) const;

	VkSwapchainKHR m_Swapchain;
	std::shared_ptr<VlkDevice> m_Device;
	std::unique_ptr<VlkWindowSurface> m_Surface;
};

}
}