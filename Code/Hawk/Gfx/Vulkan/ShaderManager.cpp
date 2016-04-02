#include "pch.h"
#include "ShaderManager.h"
#include "ShaderModule.h"
#include "Util/Algorithm.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

ShaderManager::ShaderManager(std::shared_ptr<Device> p_Device)
: m_Device(p_Device)
{
}

ShaderManager::~ShaderManager()
{
}

VkPipelineShaderStageCreateInfo ShaderManager::GetShader(const std::string& p_Filename, const std::string& p_EntryPoint, VkShaderStageFlagBits p_Stage)
{
	auto l_Itr = hwk::find_first(m_Modules, p_Filename);
	ShaderModule* l_Module = nullptr;
	if (l_Itr == m_Modules.end())
	{
		l_Module = m_Modules.insert(Modules::value_type(p_Filename, std::make_unique<ShaderModule>(m_Device, GetPath(p_Filename)))).first->second.get();
	}
	return GetCreateInfo(l_Module, p_EntryPoint, p_Stage);
}

std::string ShaderManager::GetPath(const std::string& p_Filename)
{
	std::ostringstream l_Stream;
	l_Stream << "content/shaders/" << p_Filename;
	return l_Stream.str();
}

VkPipelineShaderStageCreateInfo ShaderManager::GetCreateInfo(const ShaderModule* p_Module, const std::string& p_EntryPoint, VkShaderStageFlagBits p_Stage)
{
	THROW_IF_NOT(p_Module, "ShaderModule is null");

	VkPipelineShaderStageCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	l_Info.stage = p_Stage;
	l_Info.module = p_Module->GetHandle();
	l_Info.pName = p_EntryPoint.c_str();
	l_Info.pSpecializationInfo = nullptr;
	return l_Info;
}

}
}
}