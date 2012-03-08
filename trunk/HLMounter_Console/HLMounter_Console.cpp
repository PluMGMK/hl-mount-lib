/*
 * This file is part of HLMounter_Console.
 * Copyright (C) 2012 Michael "PluMGMK" Keyes - keyesmic@gmail.com

 * HLMounter_Console is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * HLMounter_Console is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with HLMounter_Console.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include <iostream>
#include <conio.h>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	hlChar *szGCF = new hlChar[260];
	_TCHAR *szMountPoint;

	bool bGcfAssigned;
	bool bMountPointAssigned;

	for(int i = 0; i < argc; i++)
	{
		if( !wcsicmp(argv[i], L"-gcf") )
		{
			i++;
			wcstombs(szGCF, argv[i], 260);
			bGcfAssigned = true;
		}
		if( !wcsicmp(argv[i], L"-mount") )
		{
			i++;
			szMountPoint = argv[i];
			bMountPointAssigned = true;
		}
	}

	if(bGcfAssigned && bMountPointAssigned)
	{
		CreateNormalMountThread(szGCF, szMountPoint);

		cout << "Attempting to mount ";
		cout << szGCF;
		cout << " at ";
		wprintf( szMountPoint );
		cout << "...\n";

		if( !g_bMounted )
			cout << "An error occurred while mounting.";

		cout << "Please press any key to exit";
		if( g_bMounted )
			cout << ", when you are finished";
		cout << ".\n";
		getch();

		if( g_bMounted )
			HLMountShutdown(szMountPoint);
	}
	else
		cout << "You must specify a GCF file with the switch '-gcf' and a mount point with the switch '-mount'\n";

	return 0;
}

