#pragma once
#include "VlkSystem.h"
#include "VlkInstance.h"
#include <vector>
#include <memory>

namespace Hawk {
namespace Gfx {

class VlkPhysicalDevice final
{
public:
	VlkPhysicalDevice(std::shared_ptr<VlkInstance> p_Instance);
	VlkPhysicalDevice(std::shared_ptr<VlkInstance> p_Instance, uint32_t p_uiDeviceID);
	~VlkPhysicalDevice();
	VlkPhysicalDevice(const VlkPhysicalDevice&) = delete;
	VlkPhysicalDevice& operator=(const VlkPhysicalDevice&) = delete;

	static bool IsLayerAvailable(VkPhysicalDevice p_Device, const std::string& p_Name);
	static bool IsExtensionAvailable(VkPhysicalDevice p_Device, const std::string& p_Name, const std::string& p_LayerName = std::string());
	VkPhysicalDevice GetHandle() const;

	using QueueFamilyProperties_t = std::vector<VkQueueFamilyProperties>;
	static void GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties_t& p_Properties);


#ifdef HAWK_DEBUG
	void CmdPrintDevices();
	void CmdPrintQueueFamilies(uint32_t p_uiDeviceIndex);
	void CmdPrintLayers(uint32_t p_uiDeviceIndex, bool p_bKeepUnsupported);
	void CmdPrintExtensions(uint32_t p_uiDeviceIndex, bool p_bKeepUnsupported);
#endif

private:
	using Devices_t = std::vector<VkPhysicalDevice>;
	using LayerProperties_t = std::vector<VkLayerProperties>;
	using ExtensionProperties_t = std::vector<VkExtensionProperties>;

	static void GetAllLayers(VkPhysicalDevice p_Device, LayerProperties_t& p_Layers, bool p_bKeepUnsupported);
	static void GetAllExtensions(VkPhysicalDevice p_Device, ExtensionProperties_t& p_Extensions, const std::string& p_LayerName = std::string());

	void GetDevices(Devices_t& p_Devices) const;
	VkPhysicalDevice GetDeviceByIndex(uint32_t p_uiIndex) const;
	VkPhysicalDevice GetDeviceByID(uint32_t p_uiDeviceID) const;

	static void GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties);	
	
	std::shared_ptr<VlkInstance> m_Instance;
	VkPhysicalDevice m_PhysicalDevice;
};

}
}