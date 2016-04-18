#pragma once
#include "System/Exception.h"

namespace Hawk {

class ScopedConsoleCommands;
class Vec3;

namespace Gfx {

class Color;

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
	virtual void DrawAABB(const Vec3& p_Min, const Vec3& p_Max, const Color& p_Color, uint32 p_uiLineWidth, bool p_bDepthTest) = 0;
#endif

};

}
}