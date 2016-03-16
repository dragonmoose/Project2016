#pragma once
#include "Gfx/IRenderingAPI.h"
#include "VlkInstance.h"
#include "VlkDevice.h"
#include "VlkSystem.h"
#include "VlkWindowSurface.h"
#include "VlkSwapchain.h"
#include <memory>

namespace Hawk {
namespace Gfx {

class VlkAPI final : public IRenderingAPI
{
public:
	VlkAPI() = default;
	~VlkAPI();

	void Initialize() override;
	void Render() override;
	void SetFullscreenState(bool p_bState) override;
	void OnWindowSizeChanged(UINT32 p_uiWidth, UINT32 p_uiHeight);
	void SetDebugText(const std::string& p_Text) override;

#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
	void CmdListDevices() override;
#endif

private:
	void CreateInstance();
	void CreatePhysicalDevice();
	void SetupQueues(VlkDeviceCreateInfo& p_CreateInfo);
	void CreateDevice(const VlkDeviceCreateInfo& p_CreateInfo);
	void CreateSwapchain();
	static const std::string& GetLogDesc();

	std::shared_ptr<VlkInstance> m_Instance;
	std::shared_ptr<VlkPhysicalDevice> m_PhysicalDevice;
	std::shared_ptr<VlkDevice> m_Device;
	std::shared_ptr<VlkWindowSurface> m_WindowSurface;
	std::shared_ptr<VlkSwapchain> m_Swapchain;
};

}
}