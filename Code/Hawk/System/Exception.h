#pragma once
#include "DllExport.h"
#include <comdef.h>
#include <stdexcept>
#include <sstream>

namespace Hawk {

class HAWK_DLL_EXPORT Exception : public std::runtime_error
{
public:
	explicit Exception(const std::string& p_Msg, const std::string& p_File, int32_t p_iLine);
	const std::string& GetSourceInfo() const;
private:
	std::string m_SourceInfo;
};

}

#define THROW(msg)													\
{																	\
	std::ostringstream l_Stream;									\
	l_Stream << msg;												\
	throw Hawk::Exception(l_Stream.str(), __FILE__, __LINE__);		\
}
#define THROW_IF(p, msg)						if ((p)) THROW(msg)
#define THROW_IF_NOT(p, msg)					if (!(p)) THROW(msg)
#define THROW_COMERR(p, msg)					THROW(msg << " [" << _com_error(p).ErrorMessage() << "]")
#define THROW_IF_COMERR(p, msg)					if ((p < 0)) THROW_COMERR(p, msg)