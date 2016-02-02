#include "pch.h"
#include "RenderingModule.h"
#include "Debug/Assert.h"
#ifdef HAWK_RENDERER_D3D12
#include "D3D12/D3D12API.h"
#endif

namespace Hawk {
namespace Gfx {

RenderingModule::RenderingModule()
{
#ifdef HAWK_RENDERER_D3D12
	m_API = std::make_unique<D3D12::D3D12API>();
#endif
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
	unsigned int l_uiFPS = static_cast<unsigned int>((1.0f / p_Duration.Get(Duration::Precision::Millisecond)) * 1000.0f);
	m_API->Render();
}

}
}