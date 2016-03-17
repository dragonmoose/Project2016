#include "Device.h"
#include "WindowSurface.h"
#include "Queue.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Swapchain final
{
public:
	Swapchain(std::shared_ptr<Instance> p_Instance, std::shared_ptr<Device> p_Device);
	~Swapchain();
	Swapchain(const Swapchain&) = delete;
	Swapchain& operator=(const Swapchain&) = delete;
	void Present();

private:
	void CreateSurface(std::shared_ptr<Instance> p_Instance, VkPhysicalDevice p_PhysicalDevice);
	void GetCreateInfo(VkSwapchainCreateInfoKHR& p_Info) const;
	void InitPresentInfo();

	VkSwapchainKHR m_Swapchain;
	std::shared_ptr<Device> m_Device;
	std::unique_ptr<WindowSurface> m_Surface;
	std::shared_ptr<Queue> m_Queue;
	VkPresentInfoKHR m_PresentInfo;
	uint32 m_uiCurrentBufferIndex;
};

}
}
}