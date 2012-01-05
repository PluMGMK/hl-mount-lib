HLMounter is a very simple GUI frontend for HLMountLib.

==Compiling==
Please change the constants GCF_TO_MOUNT and MOUNT_POINT to the path of the GCF you would like to mount for testing purposes, and the drive letter to which you would like to mount it, respectively. Please note that the former is a normal string, ie. char*, while the latter is a wide string, ie. wchar_t*.

==Running==
On the File menu, click "TEST!" to mount the GCF you specified in code to the drive letter you specified in the code. When you close the window, it will be automatically unmounted and the application will terminate.

==Bugs==
-Mounted text files will not open in Notepad. I recommend the editor Notepad++ if you feel the need to open any mounted text document.
-Attempting to browse a folder containing an EXE file seems to almost always cause an access violation.
-Access violations may also randomly occur while browsing files.
-Breakpoints may also randomly be triggered while browsing files.

==Legal Stuff==
Copyright information can be found in the accompanying source code, as well as in the application's about box. The GNU General Public License, under which HLMounter is licensed, can be found in the file COPYING. This is a regular text file. You can open it in Notepad or any other text editor, just like this file.