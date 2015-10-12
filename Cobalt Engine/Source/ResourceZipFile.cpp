/*
	ResourceZipFile.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#include "ResourceZipFile.h"

#include "EngineStd.h"
#include "Resource.h"
#include "StringUtil.h"
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


//====================================================
//	Development Resource Zip File definitions
//====================================================
DevelopmentResourceZipFile::DevelopmentResourceZipFile(const std::wstring resourceFileName, const Mode mode) :
ResourceZipFile(resourceFileName)
{
	m_Mode = mode;

	TCHAR dir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, dir);

	// set the assets directory to the current directory
	m_AssetsDir = dir;

	// get the position of the last slash in the directory
	int lastSlash = m_AssetsDir.find_last_of(L"\\");
	m_AssetsDir = m_AssetsDir.substr(0, lastSlash);
	m_AssetsDir += L"\\Assets\\";
}

bool DevelopmentResourceZipFile::Open()
{
	if (m_Mode != Mode::Editor)
	{
		ResourceZipFile::Open();
	}

	if (m_Mode == Mode::Editor)
	{
		// read the entire directory (all non hidden files)
		ReadAssetsDirectory(L"*");
	}

	return true;
}

int DevelopmentResourceZipFile::GetRawResourceSize(const Resource& r)
{
	int size = 0;

	if (m_Mode = Mode::Editor)
	{
		int num = Find(r.m_Name.c_str());
		if (num == -1)
		{
			return -1;
		}

		return m_AssetFileInfo[num].nFileSizeLow;
	}

	return ResourceZipFile::GetRawResourceSize(r);
}

int DevelopmentResourceZipFile::GetRawResource(const Resource& r, char* buffer)
{
	if (m_Mode = Mode::Editor)
	{
		int num = Find(r.m_Name.c_str());
		if (num == -1)
		{
			return -1;
		}

		std::string fullFileSpec = ws2s(m_AssetsDir) + r.m_Name.c_str();
		FILE* f = fopen(fullFileSpec.c_str(), "rb");
		size_t bytes = fread(buffer, 1, m_AssetFileInfo[num].nFileSizeLow, f);
		fclose(f);

		return bytes;
	}

	return ResourceZipFile::GetRawResource(r, buffer);
}

int DevelopmentResourceZipFile::GetNumResources() const
{
	return (m_Mode == Mode::Editor) ? m_AssetFileInfo.size() : ResourceZipFile::GetNumResources();
}

std::string DevelopmentResourceZipFile::GetResourceName(int n) const
{
	if (m_Mode == Mode::Editor)
	{
		std::wstring wideName = m_AssetFileInfo[n].cFileName;
		return ws2s(wideName);
	}

	return ResourceZipFile::GetResourceName(n);
}

int DevelopmentResourceZipFile::Find(const std::string& path)
{
	// transform the file path to lowercase
	std::string lowerCase = path;
	std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), (int(*)(int))std::tolower);

	ZipContentsMap::const_iterator it = m_DirectoryContentsMap.find(lowerCase);

	if (it != m_DirectoryContentsMap.end())
	{
		return it->second;
	}

	return -1;
}

void DevelopmentResourceZipFile::ReadAssetsDirectory(std::wstring fileSpec)
{
	HANDLE fileHandle;
	WIN32_FIND_DATA findData;

	// get first file
	std::wstring pathSpec = m_AssetsDir + fileSpec;
	fileHandle = FindFirstFile(pathSpec.c_str(), &findData);

	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		// loop through the remaining entries in the directory
		while (FindNextFile(fileHandle, &findData))
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
				continue;

			std::wstring fileName = findData.cFileName;
			if (findData.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
			{
				if (fileName != L".." && fileName != L".")
				{
					fileName = fileSpec.substr(0, fileSpec.length() - 1) + fileName + L"\\*";
					ReadAssetsDirectory(fileName);
				}
			}
			else
			{
				fileName = fileSpec.substr(0, fileSpec.length() - 1) + fileName;
				std::wstring lower = fileName;
				std::transform(lower.begin(), lower.end(), lower.begin(), (int(*)(int)) std::tolower);
				wcscpy_s(&findData.cFileName[0], MAX_PATH, lower.c_str());
				m_DirectoryContentsMap[ws2s(lower)] = m_AssetFileInfo.size();
				m_AssetFileInfo.push_back(findData);
			}
		}
	}

	FindClose(fileHandle);
}
