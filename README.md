# FALL 2019 CSE221 Project : [System Measurement Project](http://cseweb.ucsd.edu/classes/fa19/cse221-a/project.html)

------------------------------<br>
Rohit Gupta                   <br>
Kien Nguyen                   <br>
Ben Zhou                      <br>
------------------------------<br>

## Table of Contents
  * [Directory Structure](#directory-structure)
  * [Build](#build)
  * [Executing Experiments](#executing-experiments)
  * [Important Know-how for Mac](#important-know-how-for-mac)

## Directory Structure

## Build
We used `Makefile` to specify experiment build dependencies and their rules for this project. So, use the following command from the top directory to generate all executables in created `build` folder.
```
make
```

## Executing Experiments

1. CPU, Scheduling, and OS Services
    * **Measurement overhead**
        ```
        sudo ./utils/repeat_cmd.sh ./build/loop_overhead 100
        ```
        ```
        sudo ./utils/repeat_cmd.sh ./build/measurement_overhead 100
        ```
    * **Procedure call overhead**
    * **System call overhead**
        ```
        sudo ./utils/repeat_cmd.sh ./build/system_call 100
        ```
    * **Task creation time**
        ```
        sudo ./utils/repeat_cmd.sh build/creation_processes 20
        ```
        ```
        sudo ./utils/repeat_cmd.sh build/creation_kthreads 20
        ```
    * **Context switch time**
        ```
        sudo ./utils/repeat_cmd.sh ./build/context_switch_processes 100
        ```
        ```
        sudo ./utils/repeat_cmd.sh ./build/context_switch_kthreads 100
        ```

2. Memory
    * **RAM access time**
        ```
        sudo ./build/ram_access_time
        ```
    * **RAM bandwidth**
        ```
        ```
    * **Page fault service time**
        ```
        sudo ./utils/repeat_cmd.sh ./build/page_fault_time 100
        ```

3. Network
    * **Round trip time**
        <br>
        Run following command to create a dummy echo server either locally or remotely on port 2000.
        ```
        ncat -l 2000 --keep-open --exec "/bin/cat"
        ```
        **Local**
        ```
        ./utils/repeat_cmd.sh ./build/round_trip_time 100
        ```
        **Remote**
        <br>
        Manually compile the executable for this case, specifying the remote server IP Address.
        ```
        gcc -Werror -O0 -D SERVERADDR=\"x.x.x.x\" -D SERVERPORT=2000 -D DATABYTES=56 -o build/round_trip_time_remote operations/3_network/round_trip_time/round_trip_time.c
        ./utils/repeat_cmd.sh ./build/round_trip_time_remote 100
        ```
    * **Peak bandwidth**
        ```
        ./utils/repeat_cmd.sh build/peak_bandwidth 100
        ```
    * **Connection overhead**
        <br>
        Run following command to create a dummy server either locally or remotely on port 2000.
        ```
        ncat -l 2000 --keep-open
        ```
        **Setup**
        ```
        sudo ./utils/repeat_cmd.sh ./build/connection_overhead_setup 100
        ```
        Manually compile the executable for this case, specifying the remote server IP Address.
        ```
        gcc -Werror -O0 -D SERVERADDR=\"x.x.x.x\" -D SERVERPORT=2000 -o build/connection_overhead_setup_remote operations/3_network/connection_overhead/connection_overhead_setup.c
        sudo ./utils/repeat_cmd.sh ./build/connection_overhead_setup_remote 100
        ```
        **Teardown**
        ```
        ./utils/repeat_cmd.sh ./build/connection_overhead_teardown 100
        ```
        Manually compile the executable for this case, specifying the remote server IP Address.
        ```
        gcc -Werror -O0 -D SERVERADDR=\"x.x.x.x\" -D SERVERPORT=2000 -o build/connection_overhead_teardown_remote operations/3_network/connection_overhead/connection_overhead_teardown.c
        ./utils/repeat_cmd.sh ./build/connection_overhead_teardown_remote 100
        ```

4. File System
    * **Size of file cache**
        ```
        ```
    * **File read time**
        <br>
        *Random Access*
        ```
        sudo ./build/file_read_time_sizes.sh ./build/file_read_time
        ```
        *Sequential Access*
        ```
        sudo ./build/file_read_time_sizes.sh ./build/file_read_time_seq
        ```
    * **Remote file read time**
        <br>
        *Random Access*
        ```
        sudo ./build/file_read_time_sizes.sh ./build/file_read_time /locally/mounted/path/containing/temp/files
        ```
        *Sequential Access*
        ```
        sudo ./build/file_read_time_sizes.sh ./build/file_read_time_seq /locally/mounted/path/containing/temp/files
        ```
    * **Contention**
        ```
        sudo ./build/file_contention.sh ./build/contention
        ```
        
## Important Know-how for Mac

**Increase the priority of a process**

Use 'nice' shell command or system call in C. Refer for system call in C - http://u.cs.biu.ac.il/~linraz/os/OS5.pdf
<br>
For usage of shell command - https://www.techrepublic.com/article/how-to-adjust-cpu-priority-using-apples-terminal/
<br>
Remember to run as superuser if doing in a C program!

**Use single core and disable hyperthreading**

For macOS (Need Xcode and its Developer Tools installed), 
1. Open `Instruments` app (Could search in spotlight)
3. From top left, Instruments -> Preferences -> CPUs

**Disable TurboBoost**

For macOS, I used this app - https://www.rugarciap.com/turbo-boost-switcher-for-os-x/
<br>
Just note that you should move the app to `Applications` folder and run it from there.

**Intel® 64 and IA-32 Architectures Software Developer’s Manual**

Useful for finding precisely about instructions and asm code. Refer - https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf

**create a dummy echo server using nmap's ncat tool**

`ncat -l 2000 --keep-open --exec "/bin/cat"`

**create a dummy server using nmap's ncat tool**

`ncat -l 2000 --keep-open`

**create a fake large file in MacOs**

`mkfile -n 1g temp_1GB_file`
