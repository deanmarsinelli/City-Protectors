/*
	StringUtil.h

	Useful string utilities.
	
	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <string>

#define MAX_DIGITS_IN_INT 12


/// Does a classic * & ? pattern match on a file name - this is case sensitive!
extern bool WildcardMatch(const char *pat, const char *str);

extern std::string ToStr(int num, int base = 10);

class HashedString
{

public:
	static void* hash_name(char const* pStr);
};