#pragma once
#ifdef HAWK_DEBUG
#include <string>
#include <vector>

namespace Hawk {

class ConsoleInputParser final
{
public:
	ConsoleInputParser(const std::string& p_Input);
	ConsoleInputParser(const ConsoleInputParser&) = delete;
	ConsoleInputParser(ConsoleInputParser&&) = default;
	ConsoleInputParser& operator=(const ConsoleInputParser&) = delete;

	const std::string& GetCommand() const { return m_Command; }
	const std::vector<std::string>& GetArgs() const { return m_Args; }
	void TryExtendArgs(std::size_t p_NumArgs);

private:
	std::string m_Command;
	std::vector<std::string> m_Args;
};

}
#endif