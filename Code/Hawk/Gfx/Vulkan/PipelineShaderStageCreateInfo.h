#pragma once
#include "System.h"
#include <string>
#include <vector>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class PipelineShaderStageCreateInfo final
{
public:
	PipelineShaderStageCreateInfo();
	PipelineShaderStageCreateInfo(const PipelineShaderStageCreateInfo&) = delete;
	PipelineShaderStageCreateInfo& operator=(const PipelineShaderStageCreateInfo&) = delete;
	~PipelineShaderStageCreateInfo();

	void Set(VkShaderStageFlagBits p_Stage, VkShaderModule p_Module, const std::string& p_Name);
	VkPipelineShaderStageCreateInfo Get() const;

private:
	VkPipelineShaderStageCreateInfo m_Info;
};

class PipelineShaderStageCreateInfoList final
{
public:
	PipelineShaderStageCreateInfoList(uint32 p_uiNumStages);
	PipelineShaderStageCreateInfoList(const PipelineShaderStageCreateInfoList&) = delete;
	PipelineShaderStageCreateInfoList& operator=(const PipelineShaderStageCreateInfoList&) = delete;

	const VkPipelineShaderStageCreateInfo* GetDataPtr() const;
	PipelineShaderStageCreateInfo& operator[](uint32 p_uiElem);

private:
	std::vector<PipelineShaderStageCreateInfo> m_Vec;
};

}
}
}