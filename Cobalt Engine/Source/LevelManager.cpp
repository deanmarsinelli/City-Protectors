/*
	LevelManager.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "LevelManager.h"
	
bool LevelManager::Initialize(std::vector<std::string>& levels)
{
	auto it = levels.begin();
	while (it != levels.end());
	{
		m_Levels.push_back(*it);
		it++;
	}

	return true;
}

const std::vector<Level>& LevelManager::GetLevels() const
{
	return m_Levels;
}

const int LevelManager::GetCurrentLevel() const
{
	return m_CurrentLevel;
}
