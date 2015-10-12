/*
	StringUtil.cpp
*/

#include "StringUtil.h"

#include "EngineStd.h"

// The following function was found on http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html, where it was attributed to 
// the C/C++ Users Journal, written by Mike Cornelison. It is a little ugly, but it is FAST. Use this as an excercise in not reinventing the
// wheel, even if you see gotos. 
bool WildcardMatch(const char *pat, const char *str) 
{
	int i, star;

new_segment:

	star = 0;
	if (*pat == '*') {
		star = 1;
		do { pat++; } while (*pat == '*'); /* enddo */
	} /* endif */

test_match:

	for (i = 0; pat[i] && (pat[i] != '*'); i++) {
		//if (mapCaseTable[str[i]] != mapCaseTable[pat[i]]) {
		if (str[i] != pat[i]) {
			if (!str[i]) return 0;
			if ((pat[i] == '?') && (str[i] != '.')) continue;
			if (!star) return 0;
			str++;
			goto test_match;
		}
	}
	if (pat[i] == '*') {
		str += i;
		pat += i;
		goto new_segment;
	}
	if (!str[i]) return 1;
	if (i && pat[i - 1] == '*') return 1;
	if (!star) return 0;
	str++;
	goto test_match;
}

HRESULT AnsiToWideCch(WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar)
{
	if (wstrDestination == NULL || strSource == NULL || cchDestChar < 1)
		return E_INVALIDARG;

	int nResult = MultiByteToWideChar(CP_ACP, 0, strSource, -1,
		wstrDestination, cchDestChar);
	wstrDestination[cchDestChar - 1] = 0;

	if (nResult == 0)
		return E_FAIL;
	return S_OK;
}

std::string ToStr(int num, int base)
{
	// turn an int into a std::string
	char str[MAX_DIGITS_IN_INT];
	memset(str, 0, MAX_DIGITS_IN_INT);
	_itoa_s(num, str, MAX_DIGITS_IN_INT, base);
	return std::string(str);
}

std::string ToStr(unsigned int num, int base)
{
	char str[MAX_DIGITS_IN_INT];
	memset(str, 0, MAX_DIGITS_IN_INT);
	_ultoa_s((unsigned long)num, str, MAX_DIGITS_IN_INT, base);
	return std::string(str);
}

std::string ToStr(unsigned long num, int base)
{
	char str[MAX_DIGITS_IN_INT];
	memset(str, 0, MAX_DIGITS_IN_INT);
	_ultoa_s((unsigned long)num, str, MAX_DIGITS_IN_INT, base);
	return std::string(str);
}

std::string ToStr(float num)
{
	char str[64];
	memset(str, 0, 64);
	_sprintf_p(str, 65, "%f", num);
	return std::string(str);
}

std::string ws2s(const std::wstring& s)
{
	int slength = (int)s.length() + 1;
	int len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0) - 1;
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

std::wstring s2ws(const std::string &s)
{
	int slength = (int)s.length() + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0) - 1;
	std::wstring r(len, '\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}

void Split(const std::string& str, StringVec& vec, char delimiter)
{
	vec.clear();
	size_t strLen = str.size();
	if (strLen == 0)
		return;

	size_t startIndex = 0;
	size_t indexOfDel = str.find_first_of(delimiter, startIndex);
	while (indexOfDel != std::string::npos)
	{
		vec.push_back(str.substr(startIndex, indexOfDel - startIndex));
		startIndex = indexOfDel + 1;
		if (startIndex >= strLen)
			break;
		indexOfDel = str.find_first_of(delimiter, startIndex);
	}
	if (startIndex < strLen)
		vec.push_back(str.substr(startIndex));
}



//====================================================
//	HashedString method definitions
//====================================================
HashedString::HashedString(const char* pIdentString)
{
	m_Ident = Hash_Name(pIdentString);
	m_IdentStr = pIdentString;
}

unsigned long HashedString::GetHashValue() const
{
	return reinterpret_cast<unsigned long>(m_Ident);
}

const std::string& HashedString::GetStr() const
{
	return m_IdentStr;
}

bool HashedString::operator<(const HashedString& rhs) const
{
	return (GetHashValue() < rhs.GetHashValue());
}

bool HashedString::operator==(const HashedString& rhs) const
{
	return (GetHashValue() == rhs.GetHashValue());
}

void* HashedString::Hash_Name(char const* pIdentStr)
{
	// Relatively simple hash of arbitrary text string into a
	// 32-bit identifier Output value is
	// input-valid-deterministic, but no guarantees are made
	// about the uniqueness of the output per-input
	//
	// Input value is treated as lower-case to cut down on false
	// separations cause by human mistypes. Sure, it could be
	// construed as a programming error to mix up your cases, and
	// it cuts down on permutations, but in Real World Usage
	// making this text case-sensitive will likely just lead to
	// Pain and Suffering.
	//
	// This code lossely based upon the adler32 checksum by Mark
	// Adler and published as part of the zlib compression
	// library sources.

	// largest prime smaller than 65536
	unsigned long BASE = 65521L;

	// NMAX is the largest n such that 255n(n+1)/2 +
	// (n+1)(BASE-1) <= 2^32-1
	unsigned long NMAX = 5552;

#define DO1(buf,i)  {s1 += tolower(buf[i]); s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

	if (pIdentStr == NULL)
		return NULL;

	unsigned long s1 = 0;
	unsigned long s2 = 0;

	for (size_t len = strlen(pIdentStr); len > 0;)
	{
		unsigned long k = len < NMAX ? len : NMAX;

		len -= k;

		while (k >= 16)
		{
			DO16(pIdentStr);
			pIdentStr += 16;
			k -= 16;
		}

		if (k != 0) do
		{
			s1 += tolower(*pIdentStr++);
			s2 += s1;
		} while (--k);

		s1 %= BASE;
		s2 %= BASE;
	}

#pragma warning(push)
#pragma warning(disable : 4312)

	return reinterpret_cast<void *>((s2 << 16) | s1);

#pragma warning(pop)
#undef DO1
#undef DO2
#undef DO4
#undef DO8
#undef DO16
}
