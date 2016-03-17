#include "VlkDevice.h"
#include "VlkWindowSurface.h"
#include "VlkQueue.h"
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
	void InitPresentInfo();

	VkSwapchainKHR m_Swapchain;
	std::shared_ptr<VlkDevice> m_Device;
	std::unique_ptr<VlkWindowSurface> m_Surface;
	std::shared_ptr<VlkQueue> m_Queue;
	VkPresentInfoKHR m_PresentInfo;
	uint32_t m_uiCurrentBufferIndex;
};

}
}