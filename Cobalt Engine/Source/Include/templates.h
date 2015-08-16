/*
	templates.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

/**
	An object creation function template for creating a derived object.
*/
template<class BaseType, class SubType>
BaseType* GenericObjectCreationFunction()
{
	return new SubType;
}

/**
	An object factory template for any type.
*/
template<class BaseClass, class IdType>
class GenericObjectFactory
{
public:
	/// Register a creation function to an Id type
	template <class SubClass>
	bool Register(IdType id)
	{
		// make sure a function for this type doesn't exist yet
		auto findIt = m_CreationFunctions.find(id);
		if (findIt = m_CreationFunctions.end())
		{
			// register the function in the map
			m_CreationFunctions[id] = &GenericObjectCreationFunction<BaseClass, SubClass>;
			return true;
		}
		return false;
	}

	/// Create an object from the factory
	BaseClass* Create(IdType id)
	{
		// find the creation function in the map
		auto findIt = m_CreationFunctions.find(id);
		if (findIt != m_CreationFunctions.end())
		{
			auto func = findIt->second;
			return func();
		}

		return nullptr;
	}

private:
	// map of Id's to object creation functions
	std::unordered_map<IdType, std::function<BaseClass*()>> m_CreationFunctions;
};


/// Convert a weak ptr to a strong ptr
template<typename T>
shared_ptr<T> MakeStrongPtr(weak_ptr<T> pWeakPtr)
{
	if (!pWeakPtr.expired())
		return shared_ptr<T>(pWeakPtr);
	else
		return nullptr;
}