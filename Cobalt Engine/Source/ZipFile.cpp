/*
	ZipFile.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
	Original Author: Javier Arevalo
*/

#include "ZipFile.h"

#include <algorithm>
#include <cctype>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include "EngineStd.h"
#include "Logger.h"

typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char byte;

// --------------------------------------------
//  ZipFile struct definitions, must be packed
// --------------------------------------------
#pragma pack(1)
struct ZipFile::TZipLocalHeader
{
	enum
	{
		SIGNATURE = 0x04034b50,
	};
	dword	sig;
	word	version;
	word	flag;
	word	compression; // Z_NO_COMPRESSION or Z_DEFLATED
	word	modTime;
	word	modDate;
	dword	crc32;
	dword	cSize;
	dword	ucSize;
	word	fnameLen;
	word	xtraLen;
};

struct ZipFile::TZipDirHeader
{
	enum
	{
		SIGNATURE = 0x06054b50
	};
	dword	sig;
	word	nDisk;
	word	nStartDisk;
	word	nDirEntries;
	dword	dirSize;
	dword	dirOffset;
	word	cmntLen;
};

struct ZipFile::TZipDirFileHeader
{
	enum
	{
		SIGNATURE = 0x02014b50
	};

	dword	sig;
	word	verMade;
	word	verNeeded;
	word	flag;
	word	compression;
	word	modTime;
	word	modDate;
	dword	crc32;
	dword	cSize;	// compressed size
	dword	ucSize; // uncompressed size
	word	fnameLen;
	word	xtraLen;
	word	cmntLen;
	word	diskStart;
	word	intAttr;
	dword	extAttr;
	dword	hdrOffset;

	char* GetName() const { return (char*)(this + 1); }
	char* GetExtra() const { return GetName() + fnameLen; }
	char* GetComment() const { return GetExtra() + xtraLen; }
};

#pragma pack()

ZipFile::ZipFile()
{
	m_nEntries = 0;
	m_pFile = nullptr;
	m_pDirData = nullptr;
}

ZipFile::~ZipFile()
{
	End();
	fclose(m_pFile);
}

bool ZipFile::Init(const std::wstring& resourceFileName)
{
	End();

	// get a pointer to the file
	_wfopen_s(&m_pFile, resourceFileName.c_str(), L"rb");

	TZipDirHeader dh;

	// seek backwards from the end of the file to get the dirHeader
	fseek(m_pFile, -(int)sizeof(dh), SEEK_END);
	long dhOffset = ftell(m_pFile); // store header's location in the file
	ZeroMemory(&dh, sizeof(dh));
	fread(&dh, sizeof(dh), 1, m_pFile);

	// check to make sure it worked
	if (dh.sig != TZipDirHeader::SIGNATURE)
		return false;

	// go to the beginning of the directory
	fseek(m_pFile, dhOffset - dh.dirSize, SEEK_SET);

	// allocate a buffer for the raw data and read
	m_pDirData = CB_NEW char[dh.dirSize + dh.nDirEntries * sizeof(*m_ppDir)];
	if (!m_pDirData)
		return false;
	ZeroMemory(m_pDirData, dh.dirSize + dh.nDirEntries * sizeof(*m_ppDir));
	fread(m_pDirData, dh.dirSize, 1, m_pFile);

	// now handle each directory entry
	char* pfh = m_pDirData;
	// point to the dirFileHeaders
	m_ppDir = (const TZipDirFileHeader **)(m_pDirData + dh.dirSize);

	bool success = true;
	
	// Fill in the ppDir array with pointers to each dirFileHeader
	for (int i = 0; i < dh.nDirEntries && success; i++)
	{
		// reference to the current dirFileHeader
		TZipDirFileHeader& fh = *(TZipDirFileHeader*)pfh;
		m_ppDir[i] = &fh;

		if (fh.sig != TZipDirFileHeader::SIGNATURE)
			success = false;
		else
		{
			pfh += sizeof(fh);

			// convert UNIX slashes to DOS backslashes
			for (int j = 0; j < fh.fnameLen; j++)
			{
				if (pfh[j] == '/')
					pfh[j] = '\\';
			}

			char fileName[_MAX_PATH];
			memcpy(fileName, pfh, fh.fnameLen);
			fileName[fh.fnameLen] = 0;
			_strlwr_s(fileName, _MAX_PATH); // force lower case
			std::string spath = fileName;
			m_ZipContentsMap[spath] = i;   // store the file index in the map

			// skip the rest of the fields in this header
			pfh += fh.fnameLen + fh.xtraLen + fh.cmntLen;
		}
	}
	if (!success)
	{
		CB_SAFE_DELETE(m_pDirData);
	}
	else
	{
		m_nEntries = dh.nDirEntries;
	}

	return success;
}

void ZipFile::End()
{
	m_ZipContentsMap.clear();
	CB_SAFE_DELETE_ARRAY(m_pDirData);
	m_nEntries = 0;
}

inline int ZipFile::GetNumFiles() const
{
	return m_nEntries;
}

std::string ZipFile::GetFileName(int index) const
{
	std::string fileName = "";
	if (index >= 0 && index < m_nEntries)
	{
		char pDest[_MAX_PATH];
		memcpy(pDest, m_ppDir[index]->GetName(), m_ppDir[index]->fnameLen);
		fileName = pDest;
	}
	return fileName;
}

int ZipFile::GetFileLength(int index) const
{
	if (index < 0 || index >= m_nEntries)
		return -1;
	else
		return m_ppDir[index]->ucSize;
}

bool ZipFile::ReadFile(int index, void* pBuffer)
{
	if (pBuffer == nullptr || index < 0 || index >= m_nEntries)
		return false;

	// seek to the actual files location and read the local header
	fseek(m_pFile, m_ppDir[index]->hdrOffset, SEEK_SET);

	TZipLocalHeader h;
	ZeroMemory(&h, sizeof(h));
	fread(&h, sizeof(h), 1, m_pFile);
	if (h.sig != TZipLocalHeader::SIGNATURE)
		return false;

	// skip extra fields
	fseek(m_pFile, h.fnameLen + h.xtraLen, SEEK_CUR);

	// if the file is uncompressed, read it into the buffer and return
	if (h.compression == Z_NO_COMPRESSION)
	{
		fread(pBuffer, h.cSize, 1, m_pFile);
		return true;
	}
	else if (h.compression != Z_DEFLATED)
		return false;

	// allocate a compressed buffer, read the data, then uncompress
	char* pcData = CB_NEW char[h.cSize];
	if (!pcData)
		return false;
	
	ZeroMemory(pcData, h.cSize);
	fread(pcData, h.cSize, 1, m_pFile);

	bool ret = true;

	// start decompressing the file
	z_stream stream;
	int err;

	stream.next_in = (Bytef*)pcData;
	stream.avail_in = (uInt)h.cSize;
	stream.next_out = (Bytef*)pBuffer;
	stream.avail_out = h.ucSize;
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;

	err = inflateInit2(&stream, -MAX_WBITS);
	if (err == Z_OK)
	{
		err = inflate(&stream, Z_FINISH);
		inflateEnd(&stream);
		if (err == Z_STREAM_END)
			err = Z_OK;
		inflateEnd(&stream);
	}
	if (err != Z_OK)
		ret = false;
	
	delete[] pcData;
	return ret;
}

bool ZipFile::ReadLargeFile(int index, void *pBuffer, std::function<void(int, bool&)> progressCallback)
{
	if (pBuffer == nullptr || index < 0 || index >= m_nEntries)
		return false;

	// seek to the actual files location and read the local header
	fseek(m_pFile, m_ppDir[index]->hdrOffset, SEEK_SET);

	TZipLocalHeader h;
	ZeroMemory(&h, sizeof(h));
	fread(&h, sizeof(h), 1, m_pFile);
	if (h.sig != TZipLocalHeader::SIGNATURE)
		return false;

	// skip extra fields
	fseek(m_pFile, h.fnameLen + h.xtraLen, SEEK_CUR);

	// if the file is uncompressed, read it into the buffer and return
	if (h.compression == Z_NO_COMPRESSION)
	{
		fread(pBuffer, h.cSize, 1, m_pFile);
		return true;
	}
	else if (h.compression != Z_DEFLATED)
		return false;

	// allocate a compressed buffer, read the data, then uncompress
	char* pcData = CB_NEW char[h.cSize];
	if (!pcData)
		return false;

	ZeroMemory(pcData, h.cSize);
	fread(pcData, h.cSize, 1, m_pFile);

	bool ret = true;

	// start decompressing the file
	z_stream stream;
	int err;

	stream.next_in = (Bytef*)pcData;
	stream.avail_in = (uInt)h.cSize;
	stream.next_out = (Bytef*)pBuffer;
	stream.avail_out = (128 * 1024); // read 128k at a time
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;

	err = inflateInit2(&stream, -MAX_WBITS);
	if (err == Z_OK)
	{
		uInt count = 0;
		bool cancel = false;
		while (stream.total_in < (uInt)h.cSize && !cancel)
		{
			err = inflate(&stream, Z_SYNC_FLUSH);
			if (err == Z_STREAM_END)
			{
				err = Z_OK;
				break;
			}
			else if (err != Z_OK)
			{
				CB_ASSERT(0 && "Something bad happened.");
				break;
			}

			stream.avail_out = (128 * 1024);
			stream.next_out += stream.total_out;

			progressCallback(count * 100 / h.cSize, cancel);
		}
		inflateEnd(&stream);
	}
	if (err != Z_OK)
		ret = false;

	delete[] pcData;
	return ret;
}

int ZipFile::Find(const std::string& path) const
{
	std::string lowercase = path;
	// convert path to all lowercase
	std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), (int(*)(int)) std::tolower);
	ZipContentsMap::const_iterator it = m_ZipContentsMap.find(lowercase);
	if (it == m_ZipContentsMap.end())
		return -1;
	return it->second;
}