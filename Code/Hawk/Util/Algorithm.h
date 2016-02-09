#pragma once

#include <algorithm>
#include <numeric>

namespace Hawk {
namespace hwk {

template<class Range_t, class OutIt_t, class Pred_t>
void transform_first_if(const Range_t& p_Range, OutIt_t p_OutIt, Pred_t p_Pred)
{
	for (auto& l_Itr = p_Range.cbegin(); l_Itr != p_Range.cend(); l_Itr++)
	{
		if (p_Pred(*l_Itr))
		{
			*p_OutIt = l_Itr->first;
			p_OutIt++;
		}
	}
}

template<class Range_t, class Pred_t>
void erase_if(Range_t& p_Range, Pred_t p_Pred)
{
	p_Range.erase(std::remove_if(p_Range.begin(), p_Range.end(), p_Pred), p_Range.end());
}

template<class Range_t>
decltype(auto) average(Range_t& p_Range)
{
	return (std::accumulate(p_Range.begin(), p_Range.end(), 0) / p_Range.size());
}

}
}