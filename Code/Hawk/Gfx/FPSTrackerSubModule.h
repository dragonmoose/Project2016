#pragma once
#include "Base/SubModule.h"

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
	int32_t m_iMin;
	int32_t m_iMax;
	
	static const int32_t sc_iNumSamples = 100;
	int32_t m_iCurrSampleIndex;
	std::vector<int32_t> m_Samples;
};

}}