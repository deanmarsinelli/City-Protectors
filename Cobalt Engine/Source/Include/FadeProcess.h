/*
	FadeProcess.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "Process.h"
#include "SoundProcess.h"

/**
	This class is used to fade sound volume in or out over time 
	and then kills itself.
*/
class FadeProcess : public Process
{
public:
	/// Constructor taking in a sound process, fade time, and ending volume
	FadeProcess(shared_ptr<SoundProcess> sound, float fadeTime, int endVolume);
	
	/// Update method called once per frame
	virtual void OnUpdate(float deltaTime) override;

protected:
	/// The sound to be faded in or out
	shared_ptr<SoundProcess> m_Sound;

	/// Total time it takes to fade the sound
	float m_TotalFadeTime;

	/// How much time has elapsed since the start of the fade
	float m_ElapsedTime;

	/// Starting volume of the sound
	int m_StartVolume;

	/// Ending volume of the sound
	int m_EndVolume;
};