#pragma once
#include "Base/SubModule.h"
#include "System/Time.h"
#include <array>

namespace Hawk {
namespace Gfx {

class DebugTextSubModule;

class FPSTrackerSubModule : public SubModule
{
public:
	FPSTrackerSubModule(std::shared_ptr<DebugTextSubModule> p_DebugTextSubModule);
	std::string GetName() const override;
	void Update(const Duration& p_Duration);

#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
#endif

private:
	void Reset();

	std::shared_ptr<DebugTextSubModule> m_DebugTextSubModule;
	
	static const int32 sc_iNumSamples = 100;
	int32 m_iCurrSampleIndex;
	std::array<float, sc_iNumSamples> m_Samples;
	Time m_PrevTime;
};

}}