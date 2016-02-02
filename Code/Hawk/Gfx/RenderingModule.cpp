#include "pch.h"
#include "RenderingModule.h"
#include "DebugTextSubModule.h"
#include "FPSTrackerSubModule.h"
#include "Debug/Assert.h"
#ifdef HAWK_RENDERER_D3D12
#include "D3D12/D3D12API.h"
#endif

namespace Hawk {
namespace Gfx {

RenderingModule::RenderingModule()
{
#ifdef HAWK_RENDERER_D3D12
	m_API = std::make_shared<D3D12::D3D12API>();
#endif
	m_DebugTextSubModule = std::make_shared<DebugTextSubModule>(m_API);
	AddSubModule(std::make_shared<FPSTrackerSubModule>(m_DebugTextSubModule));
	AddSubModule(m_DebugTextSubModule);
}

std::string RenderingModule::GetName() const
{
	return "RenderingModule";
}

void RenderingModule::Initialize()
{
	if (Config::Instance().Get("gfx.setFixedFPS", false))
	{
		SetFixedTimeStep(Config::Instance().Get("gfx.fixedFPS", 60), Module::FixedTimeStepDecl::FramesPerSecond);
	}
	m_API->Initialize();
}

void RenderingModule::RegisterEvents(EventManager& p_EventManager)
{
}

#ifdef HAWK_DEBUG
void RenderingModule::InitializeConsole()
{
	RegisterConsole("gfx.listDevices", m_API.get(), &IRenderingAPI::CmdListDevices, "Lists the available hardware adapters", "");
}
#endif

void RenderingModule::Update(const Duration& p_Duration)
{
	m_API->Render();
}

}
}