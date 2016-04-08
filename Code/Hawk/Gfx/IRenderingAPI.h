#pragma once
#include "System/Exception.h"

namespace Hawk {

class ScopedConsoleCommands;

namespace Gfx {

class IRenderingAPI
{
public:
	IRenderingAPI() = default;
	virtual ~IRenderingAPI() = default;
	IRenderingAPI(const IRenderingAPI&) = delete;
	IRenderingAPI& operator=(const IRenderingAPI&) = delete;

	virtual void Initialize() = 0;
	virtual void Update(Duration p_Duration) = 0;
	virtual void Render() = 0;
	virtual void SetFullscreenState(bool p_bState) = 0;
	virtual void OnWindowSizeChanged(UINT32 p_uiWidth, UINT32 p_uiHeight) = 0;
	virtual void SetDebugText(const std::string& p_Text) = 0;

#ifdef HAWK_DEBUG
	virtual void InitializeConsole(ScopedConsoleCommands* /*p_Console*/) { }
#endif

};

}
}