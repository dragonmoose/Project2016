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
			boost::property_tree::ini_parser::read_ini(c_Filename, m_Properties);
			return true;
		}
		LOG_INFO("No config file found in path: " << boost::filesystem::current_path());
	}
	catch (boost::property_tree::ini_parser_error& e)
	{
		LOG_ERROR("Failed to parse config. Exception: " << e.what());
	}
	return false;
}

}