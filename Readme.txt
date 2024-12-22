Before you begin, ensure you have the following installed on your system:

- [Ubuntu 20.04 or later](https://ubuntu.com/download/desktop)
- [Make](https://www.gnu.org/software/make/)
- [GCC (GNU Compiler Collection)](https://gcc.gnu.org/)

To install `Make` and `GCC`, run the following command:

sudo apt update
sudo apt install build-essential


Please remember to follow these steps:



be sure to be superuser

	sudo su
    
compile the module with make

check the major number that has been assigned with 

	cat /proc/device 
	
then as as everything goes well 
by ./threads in terminal,it will be executed.


to remove all compiled files and binaries:
   make clean 
   
   
to view kernel log 
   dmesg | tail -n 200
   
 Folder satatus:

    specific.c
    This program uses POSIX threads to create various tasks including periodic and aperiodic tasks. It simulates real-time execution and scheduling policies by opening a device file to log messages for each task.

    threads.cpp
    This file implements a simple character device as a Linux kernel module, which logs any data written to it into the kernel log.

    Makefile
    The Makefile streamlines the tasks of compiling, installing, and cleaning up the kernel module (specific_device) and the user-space threads . Additionally, it manages the creation of the device file for the kernel module.