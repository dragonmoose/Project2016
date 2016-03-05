#include "pch.h"
#include "Config.h"
#include "Duration.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace Hawk {

namespace
{
	const Duration c_UpdateDuration(1, Duration::Precision::Second);
}

void Config::SetFilename(const std::string& p_Filename)
{
	m_Filename = p_Filename;
	LOG("Using config file: " << m_Filename, "config", Info);
}

Config& Config::Instance()
{
	static Config l_Config;
	return l_Config;
}

Config::Config()
: m_LastWriteTime(0)
, m_Mutex("Config")
{
}

bool Config::Load(bool p_bForce)
{
	try
	{
		if (!m_Filename.empty())
		{
			if (boost::filesystem::exists(m_Filename))
			{
				if (p_bForce)
				{
					PrivLoad();
				}
				else
				{
					std::time_t l_WriteTime = boost::filesystem::last_write_time(m_Filename);
					if (l_WriteTime > m_LastWriteTime)
					{
						m_LastWriteTime = l_WriteTime;
						PrivLoad();
					}
				}
			}
			else
			{
				THROW("The specified config file was not found: " << m_Filename);
			}
		}
		return true;
	}
	catch (boost::property_tree::ini_parser_error& e)
	{
		FATAL_STD_EXCEPTION(e, "config");
	}
	catch (Exception& e)
	{
		FATAL_EXCEPTION(e, "config");
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
	boost::property_tree::ini_parser::read_ini(m_Filename, l_NewProperties);

	MutexScope_t l_MutexScope(m_Mutex);
	m_Properties.swap(l_NewProperties);
}

}