#pragma once

#include <string>

#define CB_ASSERT(x) assert(x)

#define CB_LOG(tag, str) \
	do \
	{ \
		std::string s(str); \
	} \
	while (0)