#include "pch.h"
#include "RenderingModule.h"
#include "RenderingAPISubModule.h"
#include "Debug/Assert.h"
#ifdef HAWK_RENDERER_D3D12
#include "D3D12/Renderer.h"
#endif

namespace Hawk {
namespace Gfx {

RenderingModule::RenderingModule()
{
#ifdef HAWK_RENDERER_D3D12
	AddSubModule(std::make_unique<D3D12::Renderer>());
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
}

void RenderingModule::RegisterEvents(EventManager& p_EventManager)
{
}

void RenderingModule::Update(const Duration& p_Duration)
{
}

}
}