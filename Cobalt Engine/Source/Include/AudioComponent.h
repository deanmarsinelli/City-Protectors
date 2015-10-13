/*
	AudioComponent.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "Component.h"

/**
	This component allows game objects to trigger sound effects.
*/
class AudioComponent : public Component
{
public:
	/// Default constructor
	AudioComponent();

	// Component Interface
	virtual bool Init(TiXmlElement* pData) override;
	virtual void PostInit() override;
	virtual TiXmlElement* GenerateXml() override;
	virtual const char* GetName() const override;

public:
	/// Name of the component
	static const char* g_Name;

private:
	/// Name of the audio resource for this component
	std::string m_AudioResource;

	/// Does the sound effect loop?
	bool m_Looping;

	/// Time it should take to fade the effect in
	float m_FadeInTime;
	
	/// Volume the effect should be played at
	int m_Volume;
};