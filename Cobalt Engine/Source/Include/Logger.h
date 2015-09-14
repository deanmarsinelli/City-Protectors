#pragma once

#include <assert.h>
#include <string>

#define CB_ASSERT(x)

#define CB_LOG(tag, str) \
	do \
	{ \
		std::string s(str); \
	} \
	while (0)

#define CB_ERROR(str)

#define CB_WARNING(str)

#define EXIT_ASSERT CB_ASSERT(0)
