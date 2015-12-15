#pragma once
#include <string>
#include <vector>

namespace Hawk {

class ConsoleCommand final
{
public:
	ConsoleCommand(const std::string& p_Input);
	ConsoleCommand(const ConsoleCommand&) = delete;
	ConsoleCommand& operator=(const ConsoleCommand&) = delete;

	const std::string& GetName() const { return m_Name; }
	const std::vector<std::string>& GetArgs() const { return m_Args; }
private:
	std::string m_Name;
	std::vector<std::string> m_Args;
};

}