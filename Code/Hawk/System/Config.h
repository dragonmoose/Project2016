#pragma once
#include "Console/Logger.h"
#include "System/DllExport.h"
#include "System/Time.h"
#include <boost/property_tree/ptree.hpp>
#include <mutex>

namespace Hawk {

class Config final
{
public:
	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;

	HAWK_DLL_EXPORT static Config& Instance();
	void Update();

	template<class T>
	T Get(const std::string& p_Key, const T& p_Default) const
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		return m_Properties.get<T>(p_Key, p_Default);
	}

	void Set(const std::string& p_Key, const std::string& p_Value)
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		m_Properties.put(p_Key, p_Value);
	}
	bool Load(bool p_bForce = false);

private:
	Config();

	void PrivLoad();

	boost::property_tree::ptree m_Properties;
	std::time_t m_LastWriteTime;
	Time m_NextReloadTime;
	mutable std::mutex m_Mutex;
};

}