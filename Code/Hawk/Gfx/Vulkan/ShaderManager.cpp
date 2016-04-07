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

void ShaderManager::GetShader(const std::string& p_Filename, const std::string& p_EntryPoint, VkShaderStageFlagBits p_Stage, PipelineShaderStageCreateInfo& p_CreateInfo)
{
	auto l_Itr = hwk::find_first(m_Modules, p_Filename);
	ShaderModule* l_Module = nullptr;
	if (l_Itr == m_Modules.end())
	{
		l_Module = m_Modules.insert(Modules::value_type(p_Filename, std::make_unique<ShaderModule>(m_Device, GetPath(p_Filename)))).first->second.get();
	}
	GetCreateInfo(l_Module, p_EntryPoint, p_Stage, p_CreateInfo); // TODO: Flags support?
}

std::string ShaderManager::GetPath(const std::string& p_Filename)
{
	std::ostringstream l_Stream;
	l_Stream << "content/shaders/" << p_Filename;
	return l_Stream.str();
}

void ShaderManager::GetCreateInfo(const ShaderModule* p_Module, const std::string& p_EntryPoint, VkShaderStageFlagBits p_Stage, PipelineShaderStageCreateInfo& p_CreateInfo)
{
	THROW_IF_NOT(p_Module, "ShaderModule is null");
	p_CreateInfo.Set(p_Stage, p_Module->GetHandle(), p_EntryPoint);
}

}
}
}