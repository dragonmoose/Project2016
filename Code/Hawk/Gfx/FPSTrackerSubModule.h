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
	void InitializeConsole() override;
#endif

private:
	void Reset();

	std::shared_ptr<DebugTextSubModule> m_DebugTextSubModule;
	INT32 m_iMin;
	INT32 m_iMax;
	
	static const INT32 sc_iNumSamples = 100;
	INT32 m_iCurrSampleIndex;
	std::vector<INT32> m_Samples;
};

}}