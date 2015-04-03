HLMountLib is a dynamic library that can mount Steam's Game Cache Files (GCFs) as folders on the filesystem, which allows their contents to be browsed and read in situ, just like normal files. It makes use of the well-known HLLib and Dokan libraries.

## Bugs ##
  * Mounted text files will not open in Notepad.
  * Attempting to browse a folder containing an EXE file can sometimes cause an access violation.
  * HLMounter\_Batch does the job it's meant to do, but its user interaction is a total disaster - when it says press a key, it means press a key once for each mount point you've specified, then another for good measure - and that's if you're lucky enough to get human-readable output from it in the first place!

## Other ##
Also included in the source is HLMounter, a very simple GUI for the library, along with HLMounter\_Console and HLMounter\_Batch, two command-line applications.