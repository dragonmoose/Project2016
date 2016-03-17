#pragma once

#include <algorithm>
#include <numeric>

namespace Hawk {
namespace hwk {

template<class Range, class OutIt, class Pred>
void transform_first_if(const Range& p_Range, OutIt p_OutIt, Pred p_Pred)
{
	for (auto l_Itr = p_Range.cbegin(); l_Itr != p_Range.cend(); l_Itr++)
	{
		if (p_Pred(*l_Itr))
		{
			*p_OutIt = l_Itr->first;
			p_OutIt++;
		}
	}
}

template<class Range, class Pred>
void erase_if(Range& p_Range, Pred p_Pred)
{
	p_Range.erase(std::remove_if(p_Range.begin(), p_Range.end(), p_Pred), p_Range.end());
}

template<class Range>
decltype(auto) average(Range& p_Range)
{
	return (std::accumulate(p_Range.begin(), p_Range.end(), 0) / p_Range.size());
}

template<class Range, class T>
decltype(auto) contains(Range& p_Range, const T& p_Value)
{
	return std::find(p_Range.cbegin(), p_Range.cend(), p_Value) != p_Range.cend();
}

template<class Range>
void unique(Range& p_Range)
{
	p_Range.erase(std::unique(p_Range.begin(), p_Range.end()), p_Range.end());
}

}
}