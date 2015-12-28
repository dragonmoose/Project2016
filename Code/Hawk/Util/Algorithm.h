#pragma once

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

}
}