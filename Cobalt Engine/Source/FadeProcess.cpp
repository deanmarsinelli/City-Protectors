/*
	FadeProcess.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "FadeProcess.h"

FadeProcess::FadeProcess(shared_ptr<SoundProcess> sound, float fadeTime, int endVolume)
{
	m_Sound = sound;
	m_TotalFadeTime = fadeTime;
	m_StartVolume = sound->GetVolume();
	m_EndVolume = endVolume;
	m_ElapsedTime = 0.0f;

	OnUpdate(0.0f);
}

void FadeProcess::OnUpdate(float deltaTime)
{
	// fade in (or out) the sound volume over time
	m_ElapsedTime += deltaTime;

	if (m_Sound->IsDead())
		Succeed();

	float coeff = m_ElapsedTime / m_TotalFadeTime;
	if (coeff > 1.0f)
	{
		coeff = 1.0f;
	}
	if (coeff < 0.0f)
	{
		coeff = 0.0f;
	}

	int newVolume = m_StartVolume + (int)(float(m_EndVolume - m_StartVolume) * coeff);

	if (m_ElapsedTime >= m_TotalFadeTime)
	{
		newVolume = m_EndVolume;
		Succeed();
	}

	m_Sound->SetVolume(newVolume);
}