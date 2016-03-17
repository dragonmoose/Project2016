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
	VlkPhysicalDevice(std::shared_ptr<VlkInstance> p_Instance, uint32 p_uiDeviceID);
	~VlkPhysicalDevice();
	VlkPhysicalDevice(const VlkPhysicalDevice&) = delete;
	VlkPhysicalDevice& operator=(const VlkPhysicalDevice&) = delete;

	static bool IsLayerAvailable(VkPhysicalDevice p_Device, const std::string& p_Name);
	static bool IsExtensionAvailable(VkPhysicalDevice p_Device, const std::string& p_Name, const std::string& p_LayerName = std::string());
	VkPhysicalDevice GetHandle() const;

	using QueueFamilyProperties = std::vector<VkQueueFamilyProperties>;
	static void GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties& p_Properties);


#ifdef HAWK_DEBUG
	void CmdPrintDevices();
	void CmdPrintQueueFamilies(uint32 p_uiDeviceIndex);
	void CmdPrintLayers(uint32 p_uiDeviceIndex, bool p_bKeepUnsupported);
	void CmdPrintExtensions(uint32 p_uiDeviceIndex, bool p_bKeepUnsupported);
#endif

private:
	using Devices = std::vector<VkPhysicalDevice>;
	using LayerProperties = std::vector<VkLayerProperties>;
	using ExtensionProperties = std::vector<VkExtensionProperties>;

	static void GetAllLayers(VkPhysicalDevice p_Device, LayerProperties& p_Layers, bool p_bKeepUnsupported);
	static void GetAllExtensions(VkPhysicalDevice p_Device, ExtensionProperties& p_Extensions, const std::string& p_LayerName = std::string());

	void GetDevices(Devices& p_Devices) const;
	VkPhysicalDevice GetDeviceByIndex(uint32 p_uiIndex) const;
	VkPhysicalDevice GetDeviceByID(uint32 p_uiDeviceID) const;

	static void GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties);	
	
	std::shared_ptr<VlkInstance> m_Instance;
	VkPhysicalDevice m_PhysicalDevice;
};

}
}