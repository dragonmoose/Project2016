#include "pch.h"
#include "Logger.h"
#include <iostream>

namespace Hawk {

void Logger::Log(const std::string& p_Msg)
{
	std::cout << "LOG: " << p_Msg << std::endl;
}

}
