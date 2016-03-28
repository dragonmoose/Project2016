#include "Device.h"
#include "WindowSurface.h"
#include "Queue.h"
#include "ImageView.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Semaphore;

class Swapchain final
{
public:
	Swapchain(std::shared_ptr<Instance> p_Instance, std::shared_ptr<Device> p_Device);
	~Swapchain();
	Swapchain(const Swapchain&) = delete;
	Swapchain& operator=(const Swapchain&) = delete;
	void Present();
	void AcquireNextImage(Semaphore* p_Semaphore);
	VkImage GetImage(uint32 p_uiIndex) const;
	uint32 GetCurrImageIndex() const;
	VkImage GetCurrImage() const;

	using ImageViews_t = std::vector<std::shared_ptr<ImageView>>;
	std::shared_ptr<ImageView> GetImageView(uint32 p_uiIndex) const;
	VkExtent2D GetExtent() const;

private:
	void CreateSurface(std::shared_ptr<Instance> p_Instance, const PhysicalDevice* p_PhysicalDevice);
	void GetCreateInfo(VkSwapchainCreateInfoKHR& p_Info) const;
	void InitPresentInfo();
	void GetImages();
	void CreateSwapchain();
	void CreateImageViews();

	VkSwapchainKHR m_Handle;
	std::shared_ptr<Device> m_Device;
	std::unique_ptr<WindowSurface> m_Surface;
	std::shared_ptr<Queue> m_Queue;
	VkPresentInfoKHR m_PresentInfo;
	std::vector<VkImage> m_Images;
	uint32 m_uiCurrentBufferIndex;
	std::vector<std::shared_ptr<ImageView>> m_ImageViews;
	VkExtent2D m_Extent;
};

}
}
}