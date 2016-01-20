#include "pch.h"
#include "RenderingModule.h"
#include "RenderingAPISubModule.h"
#include "Debug/Assert.h"
#ifdef HAWK_RENDERER_D3D12
#include "D3D12/D3D12Renderer.h"
#endif

namespace Hawk {
namespace Gfx {

RenderingModule::RenderingModule()
{
#ifdef HAWK_RENDERER_D3D12
	AddSubModule(std::make_unique<D3D12Renderer>());
#endif
}

std::string RenderingModule::GetName() const
{
	return "RenderingModule";
}

void RenderingModule::Initialize()
{
	try
	{
		if (Config::Instance().Get("gfx.setFixedFPS", false))
		{
			SetFixedTimeStep(Config::Instance().Get("gfx.fixedFPS", 60), Module::FixedTimeStepDecl::FramesPerSecond);
		}

		LOG("Init done", GetLogDesc(), Info);
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION(e, "gfx", Fatal);
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