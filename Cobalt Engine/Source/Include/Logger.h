#pragma once

#include <assert.h>
#include <string>

#define CB_ASSERT(x) assert(x)

#define CB_LOG(tag, str) \
	do \
	{ \
		std::string s(str); \
	} \
	while (0)

#define CB_ERROR(str)

#define CB_WARNING(str)