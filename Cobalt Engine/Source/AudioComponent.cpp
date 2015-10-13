/*
	AudioComponent.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "AudioComponent.h"
#include "EngineStd.h"
#include "FadeProcess.h"
#include "HumanView.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "Resource.h"
#include "ResourceCache.h"
#include "SoundProcess.h"

const char* AudioComponent::g_Name = "AudioComponent";

AudioComponent::AudioComponent()
{
	m_AudioResource = "";
	m_Looping = false;
	m_FadeInTime = 0.0f;
	m_Volume = 100;
}

bool AudioComponent::Init(TiXmlElement* pData)
{
	TiXmlElement* pSound = pData->FirstChildElement("Sound");
	if (pSound)
	{
		m_AudioResource = pSound->FirstChild()->Value();
	}

	TiXmlElement* pLooping = pData->FirstChildElement("Looping");
	if (pLooping)
	{
		std::string value = pLooping->FirstChild()->Value();
		m_Looping = (value == "0") ? false : true;
	}

	TiXmlElement* pFadeIn = pData->FirstChildElement("FadeIn");
	if (pFadeIn)
	{
		std::string value = pFadeIn->FirstChild()->Value();
		m_FadeInTime = (float)atof(value.c_str());
	}

	TiXmlElement* pVolume = pData->FirstChildElement("Volume");
	if (pVolume)
	{
		std::string value = pVolume->FirstChild()->Value();
		m_Volume = atoi(value.c_str());
	}

	return true;
}

void AudioComponent::PostInit()
{
	HumanView* humanView = g_pApp->GetHumanView();
	if (!humanView)
	{
		CB_ERROR("Need a human view for sounds to be played");
		return;
	}

	ProcessManager* processManager = humanView->GetProcessManager();
	if (!processManager)
	{
		CB_ERROR("Sounds need a process manager to play");
		return;
	}

	if (g_pApp->IsRunning())
	{
		Resource resource(m_AudioResource);
		shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);
		shared_ptr<SoundProcess> sound(CB_NEW SoundProcess(handle, 0, true));
		processManager->AttachProcess(sound);

		// fade process
		if (m_FadeInTime > 0.0f)
		{
			shared_ptr<FadeProcess> fade(new FadeProcess(sound, 10000, 100));
			processManager->AttachProcess(fade);
		}
	}
}

TiXmlElement* AudioComponent::GenerateXml()
{
	TiXmlElement* pBaseElement = CB_NEW TiXmlElement(GetName());

	TiXmlElement* pSoundNode = CB_NEW TiXmlElement("Sound");
	TiXmlText* pSoundText = CB_NEW TiXmlText(m_AudioResource.c_str());
	pSoundNode->LinkEndChild(pSoundText);
	pBaseElement->LinkEndChild(pSoundNode);

	TiXmlElement* pLoopingNode = CB_NEW TiXmlElement("Looping");
	TiXmlText* pLoopingText = CB_NEW TiXmlText(m_Looping ? "1" : "0");
	pLoopingNode->LinkEndChild(pLoopingText);
	pBaseElement->LinkEndChild(pLoopingNode);

	TiXmlElement* pFadeInNode = CB_NEW TiXmlElement("FadeIn");
	TiXmlText* pFadeInText = CB_NEW TiXmlText(ToStr(m_FadeInTime).c_str());
	pFadeInNode->LinkEndChild(pFadeInText);
	pBaseElement->LinkEndChild(pFadeInNode);

	TiXmlElement* pVolumeNode = CB_NEW TiXmlElement("Volume");
	TiXmlText* pVolumeText = CB_NEW TiXmlText(ToStr(m_Volume).c_str());
	pVolumeNode->LinkEndChild(pVolumeText);
	pBaseElement->LinkEndChild(pVolumeNode);

	return pBaseElement;
}

const char* AudioComponent::GetName() const
{
	return g_Name;
}
