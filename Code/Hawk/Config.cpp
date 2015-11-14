#include "pch.h"
#include "Config.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace Hawk {

namespace Config
{
	bool Read();
	const std::string c_Filename = "config.ini";
	boost::property_tree::ptree m_Properties;
	std::time_t m_LastWriteTime = 0;
}

void Config::Initialize()
{
	LOG_INFO_IF(Read(), "Successfully read contents of " << c_Filename);
}

boost::property_tree::ptree& Config::GetProperties()
{
	return m_Properties;
}

bool Config::Read()
{
	try
	{
		if (boost::filesystem::exists(c_Filename))
		{
			std::time_t l_WriteTime = boost::filesystem::last_write_time(c_Filename);
			if (l_WriteTime > m_LastWriteTime)
			{
				m_LastWriteTime = l_WriteTime;
				boost::property_tree::ini_parser::read_ini(c_Filename, m_Properties);
				return true;
			}
		}
	}
	catch (boost::property_tree::ini_parser_error& e)
	{
		LOG_ERROR("Failed to parse config. Exception: " << e.what());
	}
	return false;
}

void Config::Update()
{
	Read();
}

}