#pragma once
#include "Base/SubModule.h"
#include <memory>

namespace Hawk {
namespace Gfx {

class IRenderingAPI;

class DebugGeometrySubModule : public SubModule
{
public:
	DebugGeometrySubModule(std::shared_ptr<IRenderingAPI>& p_API);

	std::string GetName() const override;

	void RegisterEvents(EventManager& p_EventManager) override;
#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
#endif
	void Update(const Duration& p_Duration) override;

private:
	void CmdList();

	std::shared_ptr<IRenderingAPI> m_API;
};

}
}