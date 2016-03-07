#pragma once
#include "Gfx/IRenderingAPI.h"
#include "VlkInstance.h"
#include <memory>

namespace Hawk {
namespace Gfx {

class VlkAPI final : public IRenderingAPI
{
public:
	VlkAPI() = default;

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
	static const std::string& GetLogDesc();

	std::unique_ptr<VlkInstance> m_Instance;
};

}
}