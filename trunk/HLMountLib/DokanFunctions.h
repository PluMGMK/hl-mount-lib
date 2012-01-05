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

#ifndef DOKAN_FUNCTIONS_H
#define DOKAN_FUNCTIONS_H

#include "stdafx.h"

#ifdef HLMOUNTLIB_DEBUG_MESSAGES
#define MessageBoxD MessageBox
#else
#define MessageBoxD(a,b,c,d)
#endif

static const hlChar *WtoHLChar(const LPCWSTR szIn);
static const LPCWSTR HLtoWChar(const hlChar *szIn);

HLStream *GetStreamFromLong(ULONG64 lIn);
ULONG64 SetLongToStream(HLStream *pIn);

#define LtoS GetStreamFromLong
#define StoL SetLongToStream
#define PfromC LtoS(DokanFileInfo->Context)

static int DOKAN_CALLBACK GCFReadFile(
	LPCWSTR				FileName,
	LPVOID				Buffer,
	DWORD				BufferLength,
	LPDWORD				ReadLength,
	LONGLONG			Offset,
	PDOKAN_FILE_INFO	DokanFileInfo);

static int DOKAN_CALLBACK GCFCloseFile(
	LPCWSTR					FileName,
	PDOKAN_FILE_INFO		DokanFileInfo);

static int DOKAN_CALLBACK GCFCreateFile(
	LPCWSTR					FileName,
	DWORD					AccessMode,
	DWORD					ShareMode,
	DWORD					CreationDisposition,
	DWORD					FlagsAndAttributes,
	PDOKAN_FILE_INFO		DokanFileInfo);

static int DOKAN_CALLBACK GCFWriteFile(
	LPCWSTR		FileName,
	LPCVOID		Buffer,
	DWORD		NumberOfBytesToWrite,
	LPDWORD		NumberOfBytesWritten,
	LONGLONG			Offset,
	PDOKAN_FILE_INFO	DokanFileInfo);

static int DOKAN_CALLBACK GCFCleanup(
	LPCWSTR					FileName,
	PDOKAN_FILE_INFO		DokanFileInfo);

static int DOKAN_CALLBACK GCFUnmount(
	PDOKAN_FILE_INFO	DokanFileInfo);

static int DOKAN_CALLBACK GCFOpenDirectory(
	LPCWSTR					FileName,
	PDOKAN_FILE_INFO		DokanFileInfo);

static int DOKAN_CALLBACK GCFFindFiles(
	LPCWSTR				FileName,
	PFillFindData		FillFindData, // function pointer
	PDOKAN_FILE_INFO	DokanFileInfo);

static int DOKAN_CALLBACK GCFGetVolumeInformation(
	LPWSTR		VolumeNameBuffer,
	DWORD		VolumeNameSize,
	LPDWORD		VolumeSerialNumber,
	LPDWORD		MaximumComponentLength,
	LPDWORD		FileSystemFlags,
	LPWSTR		FileSystemNameBuffer,
	DWORD		FileSystemNameSize,
	PDOKAN_FILE_INFO	DokanFileInfo);

static int DOKAN_CALLBACK GCFGetDiskFreeSpace(
	PULONGLONG			FreeBytesAvailable,
	PULONGLONG			TotalNumberOfBytes,
	PULONGLONG			TotalNumberOfFreeBytes,
	PDOKAN_FILE_INFO	DokanFileInfo);

static int DOKAN_CALLBACK GCFGetFileInformation(
	LPCWSTR							FileName,
	LPBY_HANDLE_FILE_INFORMATION	HandleFileInformation,
	PDOKAN_FILE_INFO				DokanFileInfo);

#endif // DOKAN_FUNCTIONS_H