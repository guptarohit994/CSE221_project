# CSE221_project

## Some important things that might be needed

**Increase the priority of a process**

Use 'nice' shell command or system call in C. Refer for system call in C - http://u.cs.biu.ac.il/~linraz/os/OS5.pdf
<br>
For usage of shell command - https://www.techrepublic.com/article/how-to-adjust-cpu-priority-using-apples-terminal/
<br>
Remember to run as superuser if doing in a C program!

**Use single core and disable hyperthreading**

For macOS (Need Xcode and its Developer Tools installed), 
1. Open Xcode
2. From top left, Xcode -> Open Developer Tool -> Instruments
3. From top left again, Instruments -> Preferences -> CPUs

**Intel® 64 and IA-32 Architectures Software Developer’s Manual**

Useful for finding precisely about instructions and asm code. Refer - https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf
