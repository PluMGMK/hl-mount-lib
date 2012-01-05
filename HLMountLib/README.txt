HLMountLib is a dynamic library that can mount Steam's Game Cache Files (GCFs) as drives, which allows their contents to be browsed and read in situ, just like normal files. It makes use of the well-known HLLib and Dokan libraries.

==Bugs==
-Mounted text files will not open in Notepad. I recommend the editor Notepad++ if you feel the need to open any mounted text document.
-Attempting to browse a folder containing an EXE file seems to almost always cause an access violation.
-Access violations may also randomly occur while browsing files.
-Breakpoints may also randomly be triggered while browsing files.

==Legal Stuff==
Copyright information can be found in the accompanying source code, as well as in the application's about box. The GNU Lesser General Public License, under which HLMountLib is licensed, can be found in the file COPYING.LESSER. The GNU General Public License can be found in the file COPYING. These are regular text files. You can open them in Notepad or any other text editor, just like this file.