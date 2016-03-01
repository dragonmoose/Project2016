#pragma once
#include "System/Exception.h"

#define VK_THROW_IF_ERR(p, msg)	THROW_IF(p < 0, msg)
#define VK_THROW_IF_NOT_SUCCESS(p, msg) THROW_IF_NOT(p == 0, msg)