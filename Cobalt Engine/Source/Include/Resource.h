/*
	Resource.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#pragma once

#include <algorithm>
#include <cctype>
#include <string>

/**
	Stores the name of a resource to be used by a resource handle.
*/
class Resource
{
public:
	/// Default constructor sets the name of the resource
	Resource(const std::string& name)
	{
		m_Name = name;
		std::transform(m_Name.begin(), m_Name.end(), m_Name.begin(), (int(*)(int)) std::tolower);
	}

	/// Name of the resource -- all lowercase
	std::string m_Name;
};
