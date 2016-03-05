#include "pch.h"
#include "VkUtil.h"
#include "VkSystem.h"
#include <unordered_map>
#include <algorithm>
#include <iomanip>

namespace Hawk {
namespace Gfx {
namespace VkUtil
{
	void PopulateInstanceLayers();
	void PopulateInstanceExtensions();

	struct LayerInfo
	{
		LayerInfo(const std::string& p_Name, const std::string& p_Desc)
			: m_Name(p_Name)
			, m_Desc(p_Desc) {}

		std::string m_Name;
		std::string m_Desc;
	};
	using LayerInfoVec_t = std::vector<LayerInfo>;
	LayerInfoVec_t n_InstanceLayers;
	bool n_bInitialized = false;
}

void VkUtil::Initialize()
{
	ASSERT(!n_bInitialized, "VkUtil already initialized");
	PopulateInstanceLayers();
	n_bInitialized = true;
}

bool VkUtil::IsInstanceLayerAvailable(const std::string& p_Name)
{
	ASSERT(n_bInitialized, "VkUtil not initialized");
	return std::find_if(n_InstanceLayers.begin(), n_InstanceLayers.end(),
		[p_Name](const LayerInfo& p_Info) { return p_Name == p_Info.m_Name; }) != n_InstanceLayers.end();
}

bool VkUtil::IsInstanceExtensionAvailable(const std::string& /*p_Name*/)
{
	return true;
}

void VkUtil::PopulateInstanceLayers()
{
	uint32_t l_uiCount = 0;
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceLayerProperties(&l_uiCount, nullptr), "Failed to get instance layer count");
	
	std::vector<VkLayerProperties> l_Layers;
	l_Layers.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceLayerProperties(&l_uiCount, l_Layers.data()), "Failed to get instance layers");

	for (const auto& l_Layer : l_Layers)
	{
		if (l_Layer.specVersion <= VK_API_VERSION)
		{
			n_InstanceLayers.emplace_back(l_Layer.layerName, l_Layer.description);
		}
		else
		{
			LOG("Removing layer not supported by API. Name" << l_Layer.layerName << " RequiredAPI=" << l_Layer.specVersion << " CurrAPI=" << VK_API_VERSION, "vulkan", Debug);
		}
	}
}

void VkUtil::PopulateInstanceExtensions()
{
	/*uint32_t l_uiCount = 0;
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceExtensionProperties(&l_uiCount, nullptr), "Failed to get instance layer count");

	std::vector<VkLayerProperties> l_Layers;
	l_Layers.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceLayerProperties(&l_uiCount, l_Layers.data()), "Failed to get instance layers");

	for (const auto& l_Layer : l_Layers)
	{
		if (l_Layer.specVersion <= VK_API_VERSION)
		{
			n_InstanceLayers.emplace_back(l_Layer.layerName, l_Layer.description);
			LOG("Added instance layer. Name=" << l_Layer.layerName << " Desc=" << l_Layer.description, "vulkan", Debug);
		}
		else
		{
			LOG("Removing layer not supported by API. Name" << l_Layer.layerName << " RequiredAPI=" << l_Layer.specVersion << " CurrAPI=" << VK_API_VERSION, "vulkan", Debug);
		}
	}*/
}

void VkUtil::PrintAvailableInstanceLayers()
{
	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	for (const auto& l_Layer : n_InstanceLayers)
	{
		std::cout << std::left << std::setw(35) << l_Layer.m_Name << "(" << l_Layer.m_Desc << ")\n";
	}
	std::cout << "\n";
}


}
}