#include "pch.h"
#include "System/Config.h"
#include "System/Duration.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace Hawk {

namespace
{
	const std::string c_Filename("config.ini");
	const Duration c_UpdateDuration(1, Duration::Precision::Second);
	const char* c_SysName("config");
}

Config& Config::Instance()
{
	static Config l_Config;
	return l_Config;
}

Config::Config()
: m_LastWriteTime(0)
{
}

bool Config::Load(bool p_bForce)
{
	try
	{
		if (boost::filesystem::exists(c_Filename))
		{
			if (!p_bForce)
			{
				std::time_t l_WriteTime = boost::filesystem::last_write_time(c_Filename);
				if (l_WriteTime > m_LastWriteTime)
				{
					m_LastWriteTime = l_WriteTime;
					PrivLoad();
				}
			}
			else
			{
				PrivLoad();
			}
		}
		return true;
	}
	catch (boost::property_tree::ini_parser_error& e)
	{
		LOG_STD_EXCEPTION(e, c_SysName, Fatal);
	}
	return false;
}

void Config::Update()
{
	if (Time::Now() > m_NextReloadTime)
	{
		m_NextReloadTime = Time(c_UpdateDuration);
		Load();
	}
}

void Config::PrivLoad()
{
	boost::property_tree::ptree l_NewProperties;
	boost::property_tree::ini_parser::read_ini(c_Filename, l_NewProperties);

	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	m_Properties.swap(l_NewProperties);
}

}