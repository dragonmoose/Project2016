#pragma once
#include "DllExport.h"
#include "Time.h"
#include "Console/Logger.h"
#include "Threading/Mutex.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>

namespace Hawk {

class Config final
{
public:
	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;

	void SetFilename(const std::string& p_Filename);

	HAWK_DLL_EXPORT static Config& Instance();
	void Update();

	template<class T>
	T Get(const std::string& p_Key, const T& p_Default) const
	{
		MutexScope_t l_MutexScope(m_Mutex);
		return m_Properties.get<T>(p_Key, p_Default);
	}

	template<class T>
	bool TryGet(const std::string& p_Key, const T& p_Default, T& p_Value) const
	{
		MutexScope_t l_MutexScope(m_Mutex);
		boost::optional<T> l_Node = m_Properties.get_optional<T>(p_Key);
		if (l_Node)
		{
			p_Value = l_Node.get();
			return true;
		}
		p_Value = p_Default;
		return false;
	}

	void Set(const std::string& p_Key, const std::string& p_Value)
	{
		MutexScope_t l_MutexScope(m_Mutex);
		m_Properties.put(p_Key, p_Value);
	}
	bool Load(bool p_bForce = false);

private:
	Config();

	void PrivLoad();

	boost::property_tree::ptree m_Properties;
	std::time_t m_LastWriteTime;
	Time m_NextReloadTime;
	mutable Mutex m_Mutex;
	std::string m_Filename;
};

}