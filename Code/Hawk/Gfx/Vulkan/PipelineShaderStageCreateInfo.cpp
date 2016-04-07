#include "pch.h"
#include "PipelineShaderStageCreateInfo.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

PipelineShaderStageCreateInfo::PipelineShaderStageCreateInfo()
: m_Info {}
{
}

PipelineShaderStageCreateInfo::~PipelineShaderStageCreateInfo()
{
	delete[] m_Info.pName;
}

void PipelineShaderStageCreateInfo::Set(VkShaderStageFlagBits p_Stage, VkShaderModule p_Module, const std::string& p_Name)
{
	m_Info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_Info.stage = p_Stage;
	m_Info.module = p_Module;

	char* l_Name = new char[p_Name.size() + 1];
	std::strcpy(l_Name, p_Name.data());
	m_Info.pName = l_Name;
}

VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo::Get() const
{
	return m_Info;
}


PipelineShaderStageCreateInfoList::PipelineShaderStageCreateInfoList(uint32 p_uiNumStages)
: m_Vec(p_uiNumStages)
{
}

const VkPipelineShaderStageCreateInfo* PipelineShaderStageCreateInfoList::GetDataPtr() const
{
	return reinterpret_cast<const VkPipelineShaderStageCreateInfo*>(m_Vec.data());
}
PipelineShaderStageCreateInfo& PipelineShaderStageCreateInfoList::operator[](uint32 p_uiElem)
{
	return m_Vec[p_uiElem];
}

}
}
}