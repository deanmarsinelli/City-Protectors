/*
	StringUtil.h

	Useful string utilities.
	
	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <string>
#include <vector>

#define MAX_DIGITS_IN_INT 12

typedef std::vector<std::string> StringVec;

/// Does a classic * & ? pattern match on a file name - this is case sensitive!
extern bool WildcardMatch(const char *pat, const char *str);

/// Convert an ascii string to unicode string
extern HRESULT AnsiToWideCch(WCHAR* dest, const CHAR* src, int charCount);

/// Convert an integer to string
extern std::string ToStr(int num, int base = 10);

/// Splits a string by the delimeter into a vector of strings.
void Split(const std::string& str, StringVec& vec, char delimiter);

class HashedString
{
public:
	static void* hash_name(char const* pStr);
};