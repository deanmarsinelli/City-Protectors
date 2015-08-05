/*
	ZipFile.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
	Original Author: Javier Arevalo

	Zip files are stored as follows:
	TZipDirHeader: File directory at the end of the file
	TZipDirFileHeader: Array of structures, one for each file which stores
	name of file, type of compression, and size before/after compression
	Files: Each file has a local header followed by compressed file data.

	========================
	|      File 0          |
	|-- TZipLocalHeader    |
	|-- File Data          |
	|======================|
	|      File 1          |
	|--- TZipLocalHeader   |
	|--- File Data         |
	|         ...          |
	|======================|
	| TZipDirFileHeader[0] |
	| TZipDirFileHeader[1] |
	| TZipDirFileHeader[2] |
	|         ...          |
	| TZipDirFileHeader[n] |
	|======================|
	|    TZipDirHeader     |
	========================
*/

#pragma once

#include <map>
#include <string>

// Maps a path to a zip content id
typedef std::map<std::string, int> ZipContentsMap;

/**
	Represents a zip file existing in memory.

	This class is used to extract the data of a zip file on disk and 
	load individual files into the resource cache.
*/
class ZipFile
{
public:
	/// Default Constructor
	ZipFile();

	/// Virtual Destructor
	virtual ~ZipFile();

	/// Initialize a zip object from a zip file on disk
	bool Init(const std::wstring& resourceFileName);

	/// Clear the object and erase any memory
	void End();

	/// Return the number of files in the zip object
	inline int GetNumFiles() const;

	/// Get the name of a file given the index
	std::string GetFileName(int index) const;

	/// Get the uncompressed size of a file given an index
	int GetFileLength(int index) const;

	/// Uncompress the contents of a file into a buffer. This method will block while the file is loaded.
	bool ReadFile(int index, void* pBuffer);

	/// Read a large file into a buffer asynchronously
	bool ReadLargeFile(int index, void* pBuffer, void(*progressCallback)(int, bool&));

	/// Find the index of a particular file
	int Find(const std::string& path) const;

	/// Map of names to indices in the object
	ZipContentsMap m_ZipContentsMap;
private:
	/// Struct representing the Dir Header at the end of a zip file
	struct TZipDirHeader;

	/// Struct representing a Dir File Header, one for each file
	struct TZipDirFileHeader;

	// Struct representing a Local Header before a file
	struct TZipLocalHeader;

	/// Pointer to the zip file on disk
	FILE* m_pFile;

	/// Raw dir data
	char* m_pDirData;

	/// Number of entries in the zip object
	int m_nEntries;

	/// Array of pointers to dir entries in pDirData
	const TZipDirFileHeader** m_ppDir;
};