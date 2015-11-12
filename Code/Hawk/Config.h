#pragma once

#include "DllExport.h"
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace Hawk {

namespace Config
{
	HAWK_DLL_EXPORT void Initialize();
	HAWK_DLL_EXPORT boost::property_tree::ptree& GetProperties();

	template<typename T>
	T Get(const std::string& p_Name, const T& p_Default)
	{
		return GetProperties().get<T>(p_Name, p_Default);
	}
}

}