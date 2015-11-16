#pragma once
#include "DllExport.h"
#include "Time.h"
#include <boost/property_tree/ptree.hpp>
#include <mutex>

namespace Hawk {

class Config final
{
public:
	HAWK_DLL_EXPORT static Config& Instance();
	void Update();

	template<typename T>
	T Get(const std::string& p_Name, const T& p_Default) const
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		return m_Properties.get<T>(p_Name, p_Default);
	}
	void Load();

private:
	Config();
	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;

	boost::property_tree::ptree m_Properties;
	std::time_t m_LastWriteTime;
	Time m_NextReloadTime;
	mutable std::mutex m_Mutex;
};

}