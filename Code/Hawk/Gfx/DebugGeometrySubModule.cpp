#include "pch.h"
#include "DebugGeometrySubModule.h"
#include "DebugEvents.h"
#include "IRenderingAPI.h"

namespace Hawk {
namespace Gfx {

DebugGeometrySubModule::DebugGeometrySubModule(std::shared_ptr<IRenderingAPI>& p_API)
: m_API(p_API)
{
}

std::string DebugGeometrySubModule::GetName() const
{
	return "debug_geometry";
}

void DebugGeometrySubModule::RegisterEvents(EventManager& /*p_EventManager*/)
{
}

#ifdef HAWK_DEBUG
void DebugGeometrySubModule::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("dbgGeo.list", this, &DebugGeometrySubModule::CmdList, "Lists all currently active debug geometry", "");
}
#endif

void DebugGeometrySubModule::Update(const Duration& /*p_Duration*/)
{
}

void DebugGeometrySubModule::CmdList()
{
	CONSOLE_WRITE_SCOPE();
}

}
}