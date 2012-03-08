/*
 * This file is part of HLMounter_Batch.
 * Copyright (C) 2012 Michael "PluMGMK" Keyes - keyesmic@gmail.com

 * HLMounter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * HLMounter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with HLMounter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <vector>
#include <process.h>

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	vector<_TCHAR *>vGCF;
	vector<_TCHAR *>vMountPoint;

	for(int i = 0; i < argc; i++)
	{
		if( !wcsicmp(argv[i], L"-gcf") )
		{
			i++;
			vGCF.push_back(argv[i]);
		}
		if( !wcsicmp(argv[i], L"-mount") )
		{
			i++;
			vMountPoint.push_back(argv[i]);
		}
	}

	cout << "Vectors populated!\n";

	int iValidMounts = min(vGCF.size(), vMountPoint.size());

	if(iValidMounts)
	{
		cout << "You input " << iValidMounts << " valid mount requests!\n";

		vector<FILE *>vProcesses;

		for(int j = 0; j < iValidMounts; j++)
		{
			_TCHAR *command = new _TCHAR[400];
			wsprintf(command, L"HLMounter_Console.exe -gcf \"%s\" -mount \"%s\"", vGCF[j], vMountPoint[j]);
			cout << "Command formulated!\n";

			FILE *pProcess = _wpopen(command, L"w");
			vProcesses.push_back( pProcess );
			if( !pProcess )
			{
				cout << "Error spawning process for ";
				wprintf(vGCF[j]);
				cout << "\n";
			}
			else
			{
				cout << "Successfully spawned process for ";
				wprintf(vGCF[j]);
				cout << "\n";/*

				char *psBuffer = new char[128];
				while(fgets(psBuffer, 128, pProcess))
				{
					printf(psBuffer);
				}
				delete[] psBuffer;*/
			}
			delete[] command;
		}

		cout << "Please press any key to exit, when you are finished.\n";

		_TCHAR escape = getch();
		for(int k = 0; k < iValidMounts; k++)
		{
			//fputws(&escape, vProcesses[k]);
			fwprintf(vProcesses[k], &escape);
		}

		vGCF.clear();
		vMountPoint.clear();
		vProcesses.clear();
	}
	else
		cout << "You must specify at least one GCF file with the switch '-gcf' and a corresponding mount point for each with the switch '-mount'\n";

	return 0;
}

