#pragma once

#include "System/Exception.h"

namespace Hawk {
namespace Gfx {

class IRenderingAPI
{
public:
	IRenderingAPI() = default;
	virtual ~IRenderingAPI() = default;
	IRenderingAPI(const IRenderingAPI&) = delete;
	IRenderingAPI& operator=(const IRenderingAPI&) = delete;

	virtual void Initialize() = 0;
	virtual void Render() = 0;
	virtual void SetFullscreenState(bool p_bState) = 0;
	virtual void SetDebugText(const std::string& p_Text) = 0;

#ifdef HAWK_DEBUG
	virtual void CmdListDevices() { CmdNotImplemented(); }
#endif

private:
	void CmdNotImplemented() { std::cout << "Command not implemented.\n\n"; }
};

}
}