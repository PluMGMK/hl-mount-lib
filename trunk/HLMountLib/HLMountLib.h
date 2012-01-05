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

#ifndef HLMOUNTLIB_H
#define HLMOUNTLIB_H

#include "HLLib.h"

#ifdef HLMOUNTLIB_EXPORTS
#define HLMOUNTLIB_API __declspec(dllexport)
#else
#define HLMOUNTLIB_API __declspec(dllimport)
#endif

extern HLMOUNTLIB_API BOOL g_bMounted;
extern HLMOUNTLIB_API DWORD g_dPackageSize;

HLMOUNTLIB_API HANDLE	CreateNormalMountThread(hlChar *szFileName, LPCWSTR szMountPoint, PBOOL pbMounted = &g_bMounted);
HLMOUNTLIB_API VOID		HLMountShutdown(LPCWSTR szMountPoint);

#endif //HLMOUNTLIB_H