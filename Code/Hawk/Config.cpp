#include "pch.h"
#include "Config.h"
#include "Time.h"
#include "Duration.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace Hawk {

namespace Config
{
	bool Load();

	const std::string c_Filename = "config.ini";
	const Duration c_UpdateDuration = Duration(1, Duration::Precision::Second);

	boost::property_tree::ptree n_Properties;
	std::time_t n_LastWriteTime = 0;
	Time n_NextReloadTime;
}

void Config::Initialize()
{
	LOG_INFO_IF(Load(), "Successfully read contents of " << c_Filename);
}

boost::property_tree::ptree& Config::GetProperties()
{
	return n_Properties;
}

bool Config::Load()
{
	try
	{
		if (boost::filesystem::exists(c_Filename))
		{
			std::time_t l_WriteTime = boost::filesystem::last_write_time(c_Filename);
			if (l_WriteTime > n_LastWriteTime)
			{
				n_LastWriteTime = l_WriteTime;
				boost::property_tree::ini_parser::read_ini(c_Filename, n_Properties);
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
	if (Time::Now() > n_NextReloadTime)
	{
		n_NextReloadTime = Time(c_UpdateDuration);
		LOG_INFO("loading config. next time=" << n_NextReloadTime);
		Load();
	}
}

}