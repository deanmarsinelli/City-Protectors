/*
	ResourceZipFile.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#include "ResourceZipFile.h"

#include "EngineStd.h"
#include "Resource.h"
#include "ZipFile.h"

ResourceZipFile::ResourceZipFile(const std::wstring& resFileName) :
m_pZipFile(nullptr),
m_resFileName(resFileName)
{}

ResourceZipFile::~ResourceZipFile()
{
	CB_SAFE_DELETE(m_pZipFile);
}

bool ResourceZipFile::Open()
{
	m_pZipFile = CB_NEW ZipFile;
	if (m_pZipFile)
	{
		return m_pZipFile->Init(m_resFileName);
	}
	return false;
}

int ResourceZipFile::GetRawResourceSize(const Resource& r)
{
	int resourceNum = m_pZipFile->Find(r.m_Name);
	if (resourceNum == -1)
		return -1;

	return m_pZipFile->GetFileLength(resourceNum);
}

int ResourceZipFile::GetRawResource(const Resource& r, char* buffer)
{
	int size = 0;
	int resourceNum = m_pZipFile->Find(r.m_Name);
	if (resourceNum >= 0)
	{
		size = m_pZipFile->GetFileLength(resourceNum);
		m_pZipFile->ReadFile(resourceNum, buffer);
	}

	return size;
}

int ResourceZipFile::GetNumResources() const
{
	return (m_pZipFile == nullptr) ? 0 : m_pZipFile->GetNumFiles();
}

std::string ResourceZipFile::GetResourceName(int n) const
{
	std::string resourceName = "";
	if (m_pZipFile != nullptr && n >= 0 && n < m_pZipFile->GetNumFiles())
	{
		resourceName = m_pZipFile->GetFileName(n);
	}

	return resourceName;
}