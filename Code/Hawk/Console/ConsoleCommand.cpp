#include "pch.h"

#ifdef HAWK_DEBUG
#include "Console/ConsoleCommand.h"
#include <boost/tokenizer.hpp>

namespace Hawk {

ConsoleCommand::ConsoleCommand(const std::string& p_Input)
{
	using Separator_t = boost::char_separator<char>;
	Separator_t l_Sep(" ");
	boost::tokenizer<Separator_t> l_Tokenizer(p_Input, l_Sep);
	const int l_iNumTokens = std::distance(l_Tokenizer.begin(), l_Tokenizer.end());
	THROW_IF_NOT(l_iNumTokens > 0, "Invalid number of input tokens for console command. Num=" << l_iNumTokens);

	auto& l_Itr = l_Tokenizer.begin();
	m_Name = *l_Itr++;
	for (; l_Itr != l_Tokenizer.end(); l_Itr++)
	{
		m_Args.push_back(*l_Itr);
	}
}
}
#endif