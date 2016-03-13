#pragma once
#include <ostream>
#include <vector>

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& p_Vec)
{
	if (!p_Vec.empty())
	{
		os << "[";
		for (uint32_t i = 0; i < p_Vec.size(); i++)
		{
			os << p_Vec[i];
			if (i < p_Vec.size() - 1)
			{
				os << ", ";
			}
		}
		os << "]";
	}
	return os;
}