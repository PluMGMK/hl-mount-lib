/*
 * This file is part of HLMountLib.
 * Copyright (C) 2012 Michael "PluMGMK" Keyes - keyesmic@gmail.com

 * HLMountLib makes use of HLLib, Copyright (C) 2006-2010 Ryan Gregg
 * HLMountLib makes use of Dokan, Copyright (C) 2008 Hiroki Asakawa info@dokan-dev.net 

 * HLMountLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * HLMountLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with HLMountLib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "HLMountLib.h"
#include "DokanFunctions.h"
#include "Wrapper.h"
#include "Error.h"

//Function prototypes
BOOL				MountGCF(const hlChar *szFileName, LPCWSTR szMountPoint, PBOOL pbMounted); 
DWORD WINAPI		MountThread(PVOID pParam);

//Needed to pass data to the thread
typedef struct _MOUNTDATA
{
	hlChar *m_szFileName;
	LPCWSTR m_szMountPoint;
	PBOOL m_pbMounted;
	_MOUNTDATA(hlChar *szFileName, LPCWSTR szMountPoint, PBOOL pbMounted) : m_szFileName(szFileName), m_szMountPoint(szMountPoint), m_pbMounted(pbMounted){}
} MOUNTDATA, *LPMOUNTDATA;

// exported variables
HLMOUNTLIB_API BOOL g_bMounted=FALSE;
HLMOUNTLIB_API DWORD g_dPackageSize=0;

// exported function
HLMOUNTLIB_API HANDLE CreateNormalMountThread(hlChar *szFileName, LPCWSTR szMountPoint, PBOOL pbMounted)
{
	if( *pbMounted || g_bMounted )
		return NULL;

	LPMOUNTDATA pMountData = new MOUNTDATA(szFileName, szMountPoint, pbMounted);
	HANDLE hReturnValue = CreateThread(NULL, 0, &MountThread, (LPVOID)pMountData, 0, NULL);
	return hReturnValue;
}

HLMOUNTLIB_API VOID HLMountShutdown(LPCWSTR szMountPoint)
{
	DokanRemoveMountPoint(szMountPoint);
	hlShutdown();
}

DWORD WINAPI MountThread(PVOID pParam)
{
	LPMOUNTDATA pMountData = (LPMOUNTDATA)pParam;
	MountGCF(pMountData->m_szFileName, pMountData->m_szMountPoint, pMountData->m_pbMounted);
	delete pMountData;

	return 0;
}

BOOL MountGCF(const hlChar *szFileName, LPCWSTR szMountPoint, PBOOL pbMounted)
{
	hlInitialize();

	MessageBoxD(NULL, L"Library initialised!", L"Notice", MB_OK);

	FILE *pPackageFile = fopen(szFileName, "r");
	HLPackageType ePackageType;
	if(pPackageFile != 0)
	{
		g_dPackageSize = GetFileSize(pPackageFile, NULL);

		hlByte buffer[HL_DEFAULT_PACKAGE_TEST_BUFFER_SIZE];
		hlUInt uiBufferSize = (hlUInt)fread(buffer, 1, HL_DEFAULT_PACKAGE_TEST_BUFFER_SIZE, pPackageFile);
		ePackageType = hlGetPackageTypeFromMemory(buffer, uiBufferSize);

		fclose(pPackageFile);
		pPackageFile = 0;
	}

	if( ePackageType != HL_PACKAGE_GCF )
		return FALSE;

	MessageBoxD(NULL, L"Package is GCF!", L"Notice", MB_OK);

	hlUInt uiPackage;
	if( !hlCreatePackage(ePackageType, &uiPackage) )
		return FALSE;

	MessageBoxD(NULL, L"Package created!", L"Notice", MB_OK);

	if( !hlBindPackage( uiPackage ) )
		return FALSE;

	MessageBoxD(NULL, L"Package bound!", L"Notice", MB_OK);

	if( !hlPackageOpenFile(szFileName, HL_MODE_READ | HL_MODE_VOLATILE ) )
		return FALSE; //TODO: Check if Steam is running and, if not, make things writeable

	MessageBoxD(NULL, L"Package opened!", L"Notice", MB_OK);

	PDOKAN_OPERATIONS dokanOperations = (PDOKAN_OPERATIONS)malloc(sizeof(DOKAN_OPERATIONS));
	PDOKAN_OPTIONS dokanOptions = (PDOKAN_OPTIONS)malloc(sizeof(DOKAN_OPTIONS));

	dokanOptions->Version = DOKAN_VERSION;
	dokanOptions->ThreadCount = 0; // use default
	dokanOptions->MountPoint = szMountPoint;
	dokanOptions->Options |= DOKAN_OPTION_KEEP_ALIVE;

	ZeroMemory (dokanOperations, sizeof(DOKAN_OPERATIONS));
	dokanOperations->CreateFile = &GCFCreateFile;
	dokanOperations->OpenDirectory = &GCFOpenDirectory;
	dokanOperations->CreateDirectory = NULL;
	dokanOperations->Cleanup = &GCFCleanup;
	dokanOperations->CloseFile = &GCFCloseFile;
	dokanOperations->ReadFile = &GCFReadFile;
	dokanOperations->WriteFile = &GCFWriteFile;
	dokanOperations->FlushFileBuffers = NULL;
	dokanOperations->GetFileInformation = &GCFGetFileInformation;
	dokanOperations->FindFiles = &GCFFindFiles;
	dokanOperations->FindFilesWithPattern = NULL;
	dokanOperations->SetFileAttributes = NULL;
	dokanOperations->SetFileTime = NULL;
	dokanOperations->DeleteFile = NULL;
	dokanOperations->DeleteDirectory = NULL;
	dokanOperations->MoveFile = NULL;
	dokanOperations->SetEndOfFile = NULL;
	dokanOperations->SetAllocationSize = NULL;	
	dokanOperations->LockFile = NULL;
	dokanOperations->UnlockFile = NULL;
	dokanOperations->GetFileSecurity = NULL;
	dokanOperations->SetFileSecurity = NULL;
	dokanOperations->GetDiskFreeSpace = &GCFGetDiskFreeSpace;
	dokanOperations->GetVolumeInformation = &GCFGetVolumeInformation;
	dokanOperations->Unmount = &GCFUnmount;

	*pbMounted = TRUE;

	int iStatus = DokanMain(dokanOptions, dokanOperations);
	free(dokanOperations);
	free(dokanOptions);
	return !iStatus;
}

typedef struct ReadPtrs{
	LPDWORD pdBytesRead;
	HLStream *pStream;
	LPVOID pBuffer;
	DWORD dBufferSize;

	ReadPtrs(LPDWORD i, HLStream *ii, LPVOID iii, DWORD iv) : pdBytesRead(i), pStream(ii), pBuffer(iii), dBufferSize(iv) {}
} READPTRS, *PREADPTRS;

DWORD WINAPI ReadThread(PVOID pParam)
{
	PREADPTRS pPtrs = (PREADPTRS)pParam;
	try
	{
		DWORD dBytesRead = hlStreamRead(pPtrs->pStream, pPtrs->pBuffer, pPtrs->dBufferSize);
		MessageBoxD(NULL, (LPCWSTR)(pPtrs->pBuffer), L"Read:", MB_OK);
		*pPtrs->pdBytesRead = dBytesRead;
		return !dBytesRead;
	}
	catch(char *e)
	{
		MessageBoxD(NULL, e, L"Error:", MB_ICONSTOP);
		*pPtrs->pdBytesRead = 0;
		return 1;
	}
}

#pragma region DokanFunctions
using namespace HLLib;

const hlChar *WtoHLChar(const LPCWSTR szIn)
{
	size_t sBufferSize = wcslen(szIn) + 1;
	hlChar *buffer = new hlChar[sBufferSize];
	wcstombs( buffer, szIn, sBufferSize );
	return buffer;
}

const LPCWSTR HLtoWChar(const hlChar *szIn)
{
	size_t sBufferSize = strlen(szIn) + 1;
	WCHAR *buffer = new WCHAR[sBufferSize];
	mbstowcs( buffer, szIn, sBufferSize );
	return buffer;
}

HLStream *GetStreamFromLong(ULONG64 lIn)
{
#ifdef _WIN64
	return (HLStream *)lIn;
#else
	UINT32 i = lIn;
	return (HLStream *)i;
#endif
}

ULONG64 SetLongToStream(HLStream *pIn)
{
#ifdef _WIN64
	return (ULONG64)lIn;
#else
	return (UINT32)pIn;
#endif
}

BOOL IsDesktopINI(LPCWSTR szFileName)
{
	size_t sLength = wcslen(szFileName);
	if( sLength >= 12 ) return !wcsicmp(&(szFileName[sLength-12]), L"\\desktop.ini");
	return FALSE;
}

int DOKAN_CALLBACK GCFReadFile(
	LPCWSTR				FileName,
	LPVOID				Buffer,
	DWORD				BufferLength,
	LPDWORD				ReadLength,
	LONGLONG			Offset,
	PDOKAN_FILE_INFO	DokanFileInfo)
{
	if( IsDesktopINI(FileName) ){*ReadLength=BufferLength; return 0;}

	HLStream *pStream = PfromC;
	if( !pStream ) return -ERROR_INVALID_ADDRESS;
	hlStreamSeek(pStream, Offset, HLSeekMode::HL_SEEK_BEGINNING);
#if 1
	WaitForSingleObject( CreateThread(NULL, 0, ReadThread, (LPVOID)new READPTRS(ReadLength, pStream, Buffer, BufferLength), 0, NULL), INFINITE );
#else
	try
	{
		DWORD dBytesRead = hlStreamRead(pStream, Buffer, BufferLength);
		MessageBoxD(NULL, (LPCWSTR)(Buffer), L"Read:", MB_OK);
		*ReadLength = dBytesRead;
		return !dBytesRead;
	}
	catch(char *e)
	{
		MessageBoxD(NULL, e, L"Error:", MB_ICONSTOP);
		*ReadLength = 0;
		return 1;
	}
#endif
	return 0;
}

int DOKAN_CALLBACK GCFWriteFile(
	LPCWSTR		FileName,
	LPCVOID		Buffer,
	DWORD		NumberOfBytesToWrite,
	LPDWORD		NumberOfBytesWritten,
	LONGLONG			Offset,
	PDOKAN_FILE_INFO	DokanFileInfo)
{
	if( IsDesktopINI(FileName) ){*NumberOfBytesWritten=NumberOfBytesToWrite; return 0;}

	if( !DokanFileInfo ) return -ERROR_INVALID_ADDRESS;
	HLStream *pStream = PfromC;
	if( !pStream ) return -ERROR_INVALID_ADDRESS;
	hlStreamSeek(pStream, Offset, HLSeekMode::HL_SEEK_BEGINNING);
	*NumberOfBytesWritten = hlStreamWrite(pStream, Buffer, NumberOfBytesToWrite);

	return 0;
}

int DOKAN_CALLBACK GCFCleanup(
	LPCWSTR					FileName,
	PDOKAN_FILE_INFO		DokanFileInfo)
{
	if( IsDesktopINI(FileName) ) return 0;

	MessageBoxD(NULL, FileName, L"Cleaning up", MB_OK);

	if( !DokanFileInfo ) return -ERROR_INVALID_ADDRESS;
	HLStream *pStream = PfromC;
	if( pStream )
	{
		hlStreamClose( pStream );
		hlPackageReleaseStream( pStream );
		DokanFileInfo->Context = 0;
	}
	else return -ERROR_INVALID_ADDRESS;

	return 0;
}

int DOKAN_CALLBACK GCFCloseFile(
	LPCWSTR					FileName,
	PDOKAN_FILE_INFO		DokanFileInfo)
{
	if( IsDesktopINI(FileName) ) return 0;

	MessageBoxD(NULL, FileName, L"Closing", MB_OK);

	if( !DokanFileInfo ) return -ERROR_INVALID_ADDRESS;
	HLStream *pStream = PfromC;
	if( pStream )
	{
		hlStreamClose( pStream );
		hlPackageReleaseStream( pStream );
		DokanFileInfo->Context = 0;
	}

	return 0;
}

int DOKAN_CALLBACK GCFCreateFile(
	LPCWSTR					FileName,
	DWORD					AccessMode,
	DWORD					ShareMode,
	DWORD					CreationDisposition,
	DWORD					FlagsAndAttributes,
	PDOKAN_FILE_INFO		DokanFileInfo)
{
	if( IsDesktopINI(FileName) ) return 0;
	MessageBoxD(NULL, FileName, L"Filename is:", MB_ICONASTERISK);

	if( !DokanFileInfo ) return -ERROR_INVALID_ADDRESS;
	
	HLDirectoryItem *pItem = hlFolderGetItemByPath(hlPackageGetRoot(), WtoHLChar(FileName), HLFindType::HL_FIND_ALL);
	if ( !pItem ){return -ERROR_FILE_NOT_FOUND;}

	if( DokanFileInfo->IsDirectory = ( hlItemGetType( pItem ) == HLDirectoryItemType::HL_ITEM_FOLDER ) )return 0;
	if( hlItemGetType( pItem ) == HLDirectoryItemType::HL_ITEM_NONE ){MessageBoxD(NULL, FileName, L"File is invalid:", MB_ICONERROR);}

	BOOL bRead = ( AccessMode & (GENERIC_ALL|GENERIC_READ|FILE_READ_DATA|FILE_READ_EA|READ_CONTROL|STANDARD_RIGHTS_READ) ) ? TRUE : FALSE;
	BOOL bWrite = ( AccessMode & (GENERIC_ALL|GENERIC_WRITE|FILE_WRITE_DATA|FILE_WRITE_ATTRIBUTES|FILE_WRITE_EA|FILE_APPEND_DATA|WRITE_DAC|WRITE_OWNER|STANDARD_RIGHTS_WRITE) ) ? TRUE : FALSE;
	BOOL bCreate = ( (CreationDisposition & OPEN_ALWAYS) || (CreationDisposition & CREATE_ALWAYS) ) ? TRUE : FALSE;

	bWrite &= ( ((CDirectoryItem *)pItem)->GetPackage()->GetMapping()->GetMode() & HL_MODE_WRITE ) ? TRUE : FALSE;
	bCreate = FALSE; //Disabled for now.

	hlUInt uiMode = HL_MODE_NO_FILEMAPPING;
	if( bRead )uiMode |= HL_MODE_READ;
	if( bWrite )uiMode |= HL_MODE_WRITE;
	else uiMode |= HL_MODE_VOLATILE|HL_MODE_READ; //Just in case it's not already open for reading.
	if( bCreate )uiMode |= HL_MODE_CREATE;

	try
	{
		HLStream *pStream;
		if( !hlPackageCreateStream(pItem, &pStream) ){MessageBoxD(NULL, FileName, L"pStream NULL for file:", MB_ICONERROR); return -ERROR_UNABLE_TO_LOAD_MEDIUM;}
		if( !hlStreamOpen(pStream, uiMode) ){MessageBoxD(NULL, FileName, HLtoWChar(hlGetString(HL_ERROR)), MB_ICONERROR); hlPackageReleaseStream(pStream); return -ERROR_FILE_INVALID;}
		DokanFileInfo->Context = StoL(pStream);
	}
	catch(char *e)
	{
		MessageBoxD(NULL, e, L"Error:", MB_ICONSTOP);;
		return -ERROR_UNABLE_TO_LOAD_MEDIUM;
	}

	return 0;
}

int DOKAN_CALLBACK GCFUnmount(
	PDOKAN_FILE_INFO	DokanFileInfo)
{
	if( DokanFileInfo && DokanFileInfo->Context )
	{
		HLStream *pStream = PfromC;
		hlStreamClose( pStream );
		hlPackageReleaseStream( pStream );
		DokanFileInfo->Context = 0;
	}

	hlPackageClose();
	return 0;
}

static int DOKAN_CALLBACK GCFOpenDirectory(
	LPCWSTR					FileName,
	PDOKAN_FILE_INFO		DokanFileInfo)
{
	MessageBoxD(NULL, FileName, L"Opening Directory:", MB_ICONASTERISK);

	HLDirectoryItem *pItem = hlFolderGetItemByPath(hlPackageGetRoot(), WtoHLChar(FileName), HLFindType::HL_FIND_ALL);
	if ( !pItem ){MessageBoxD(NULL, FileName, L"Folder not found!", MB_ICONERROR); return -ERROR_FILE_NOT_FOUND;}

	return 0;
}

static int DOKAN_CALLBACK GCFFindFiles(
	LPCWSTR				FileName,
	PFillFindData		FillFindData, // function pointer
	PDOKAN_FILE_INFO	DokanFileInfo)
{
	WIN32_FIND_DATAW findData;
	hlUInt nFileSizeLow;
	
	try
	{
		HLDirectoryItem *pFolder = hlFolderGetItemByPath(hlPackageGetRoot(), WtoHLChar(FileName), HLFindType::HL_FIND_ALL);
		if ( !pFolder ){MessageBoxD(NULL, FileName, L"Folder not found!", MB_ICONERROR); return -ERROR_FILE_NOT_FOUND;}

		HLDirectoryItem *pItem = hlFolderFindFirst(pFolder, "*", static_cast<HLFindType>(HL_FIND_ALL|HL_FIND_NO_RECURSE));
		while( pItem )
		{
			wcscpy(findData.cFileName, HLtoWChar(hlItemGetName(pItem)));
			findData.dwFileAttributes = FILE_ATTRIBUTE_VIRTUAL;
			if( !( ((CDirectoryItem *)pItem)->GetPackage()->GetMapping()->GetMode() & HL_MODE_WRITE ) ) findData.dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
			if(hlItemGetType(pItem)==HL_ITEM_FOLDER) findData.dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
		
			hlPackageGetFileSize(pItem, &nFileSizeLow);
			findData.nFileSizeLow = nFileSizeLow;
			findData.nFileSizeHigh = 0;

#ifdef HLMOUNTLIB_DEBUG_MESSAGES
			LPWSTR buffer = new WCHAR[20];
			_ultow(findData.nFileSizeLow, buffer, 10);
			MessageBoxD(NULL, buffer, findData.cFileName, MB_OK);
			delete[] buffer;
#endif

			FillFindData( &findData, DokanFileInfo );
			pItem = hlFolderFindNext(pFolder, pItem, "*", static_cast<HLFindType>(HL_FIND_ALL|HL_FIND_NO_RECURSE));
		}
	}
	catch(char *e)
	{
		MessageBoxD(NULL, e, L"Error:", MB_ICONSTOP);;
		return -ERROR_FILE_NOT_FOUND;
	}

	return 0;
}

static int DOKAN_CALLBACK GCFGetVolumeInformation(
	LPWSTR		VolumeNameBuffer,
	DWORD		VolumeNameSize,
	LPDWORD		VolumeSerialNumber,
	LPDWORD		MaximumComponentLength,
	LPDWORD		FileSystemFlags,
	LPWSTR		FileSystemNameBuffer,
	DWORD		FileSystemNameSize,
	PDOKAN_FILE_INFO	DokanFileInfo)
{
	//wcscpy_s( VolumeNameBuffer, VolumeNameSize / sizeof(WCHAR), L"source sounds.gcf" );
	*FileSystemFlags |= FILE_READ_ONLY|FILE_READ_ONLY_VOLUME|FILE_UNICODE_ON_DISK;
	wcscpy_s(FileSystemNameBuffer, FileSystemNameSize / sizeof(WCHAR), HLtoWChar(hlPackageGetDescription()));

	return 0;
}

static int DOKAN_CALLBACK GCFGetDiskFreeSpace(
	PULONGLONG			FreeBytesAvailable,
	PULONGLONG			TotalNumberOfBytes,
	PULONGLONG			TotalNumberOfFreeBytes,
	PDOKAN_FILE_INFO	DokanFileInfo)
{
	*FreeBytesAvailable = 0;
	*TotalNumberOfFreeBytes = 0;
	*TotalNumberOfBytes = g_dPackageSize;

	return 0;
}

static int DOKAN_CALLBACK GCFGetFileInformation(
	LPCWSTR							FileName,
	LPBY_HANDLE_FILE_INFORMATION	HandleFileInformation,
	PDOKAN_FILE_INFO				DokanFileInfo)
{
	if( IsDesktopINI(FileName) )return 0;

	HLDirectoryItem *pItem = hlFolderGetItemByPath(hlPackageGetRoot(), WtoHLChar(FileName), HL_FIND_ALL);
	HandleFileInformation->dwFileAttributes = FILE_ATTRIBUTE_VIRTUAL;
	if( !( ((CDirectoryItem *)pItem)->GetPackage()->GetMapping()->GetMode() & HL_MODE_WRITE ) ) HandleFileInformation->dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
	if(hlItemGetType(pItem)==HL_ITEM_FOLDER) HandleFileInformation->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;

	hlUInt nFileSize;
	hlPackageGetFileSize(pItem, &nFileSize);
	HandleFileInformation->nFileSizeHigh = 0;
	HandleFileInformation->nFileSizeLow = nFileSize;

	return 0;
}

#pragma endregion