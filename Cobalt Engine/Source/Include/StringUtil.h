/**
	StringUtil.h
*/

#pragma once

#include <string>

#define MAX_DIGITS_IN_INT 12

extern std::string ToStr(int num, int base = 10);

class HashedString
{

public:
	static void* hash_name(char const* pStr);
};