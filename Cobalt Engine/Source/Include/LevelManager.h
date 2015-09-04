/*
	LevelManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <string>
#include <vector>

typedef std::string Level;

/**
	Used to manage the different levels of a game.
*/
class LevelManager
{
public:
	/// Initialize the level manager with a vector of level names
	bool Initialize(std::vector<std::string>& levels);

	/// Return the vector of level names
	const std::vector<Level>& GetLevels() const;

	/// Return the current level in the game
	const int GetCurrentLevel() const;

protected:
	/// Vector of all the levels managed by this object
	std::vector<Level> m_Levels;

	/// The current active level
	int m_CurrentLevel;
};