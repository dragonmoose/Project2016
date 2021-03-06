#pragma once
#include <string>
#include <deque>

namespace Hawk {

class ConsoleHistory final
{
public:
	ConsoleHistory(uint32 p_uiMaxSize);

	ConsoleHistory(const ConsoleHistory&) = delete;
	ConsoleHistory& operator=(const ConsoleHistory&) = delete;

	bool Back();
	bool Forward();
	const std::string& GetCurrRecord() const;
	void Add(const std::string& p_Cmd);

	void Print() const;
	void Clear();

private:
	void Load();
	void Save();

	const uint32 m_uiMaxSize;
	static const std::string c_Filename;

	std::deque<std::string> m_Cmds;
	uint32 m_uiCurrRecord;
	bool m_bStartState;
};
}