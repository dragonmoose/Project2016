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

void DebugGeometrySubModule::RegisterEvents(EventManager& p_EventManager)
{
	p_EventManager.Register<AddDebugAABBEvent>([this](const AddDebugAABBEvent& p_Event)
	{
		Time l_EndTime(p_Event.m_Duration);
		m_AABBs.emplace_back(p_Event.m_Min, p_Event.m_Max, p_Event.m_Color, p_Event.m_uiLineWidth, p_Event.m_bDepthTest, l_EndTime);
	});
}

#ifdef HAWK_DEBUG
void DebugGeometrySubModule::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("dbgGeo.list", this, &DebugGeometrySubModule::CmdList, "Lists all currently active debug geometry", "");
}
#endif

void DebugGeometrySubModule::Update(const Duration& /*p_Duration*/)
{
	Time l_CurrTime;
	l_CurrTime.SetToNow();

	for (auto l_Itr = m_AABBs.begin(); l_Itr != m_AABBs.end();)
	{
		if (l_CurrTime >= l_Itr->m_EndTime)
		{
			l_Itr = m_AABBs.erase(l_Itr);
		}
		else
		{
			m_API->DrawAABB(l_Itr->m_Min, l_Itr->m_Max, l_Itr->m_Color, l_Itr->m_uiLineWidth, l_Itr->m_bDepthTest);
			l_Itr++;
		}
	}
}

void DebugGeometrySubModule::CmdList()
{
	CONSOLE_WRITE_SCOPE();
	std::cout << "\nAABB count:\t" << m_AABBs.size() << "\n";
}

}
}