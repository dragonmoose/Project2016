#pragma once
#include <string>
#include <deque>

namespace Hawk {

class ConsoleHistory final
{
public:
	ConsoleHistory(unsigned int p_uiMaxSize);
	~ConsoleHistory();

	ConsoleHistory(const ConsoleHistory&) = delete;
	ConsoleHistory& operator=(const ConsoleHistory&) = delete;

	bool Back();
	bool Forward();
	const std::string& GetCurrRecord() const;
	void Add(const std::string& p_Cmd);

private:
	void Load();
	void Save();

	const unsigned int m_uiMaxSize;
	static const std::string c_Filename;

	std::deque<std::string> m_Cmds;
	int m_uiCurrRecord;
	bool m_bStartState;
};
}