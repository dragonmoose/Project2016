#pragma once
#ifdef HAWK_DEBUG
#include <string>
#include <vector>

namespace Hawk {

class ConsoleCommand final
{
public:
	ConsoleCommand(const std::string& p_Input);
	ConsoleCommand(const ConsoleCommand&) = delete;
	ConsoleCommand(ConsoleCommand&&) = default;
	ConsoleCommand& operator=(const ConsoleCommand&) = delete;

	const std::string& GetName() const { return m_Name; }
	const std::vector<std::string>& GetArgs() const { return m_Args; }
	void TryExtendArgs(std::size_t p_NumArgs);

private:
	std::string m_Name;
	std::vector<std::string> m_Args;
};

}
#endif