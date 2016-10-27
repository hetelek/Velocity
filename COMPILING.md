The Qt Creator IDE is used to build/debug Velocity. Velocity currently depends on Qt4 and Botan 1.10.

==========

- WINDOWS

	While debugging, Velocity is compiled with 'Qt 4.8.1 for Desktop - MingGW (Qt SDK) Debug'.
	While compiling for release mode, Velocity is compiled with 'Qt 4.8.1 for Desktop - MingGW (Qt SDK) Release'.
	
	Tool Chain: 'Mingw as a GCC for Windows targets'
	
	WARNING: Botan must be found in the working directory of Velocity.exe for Velocity to successfully run. To obtain the Botan libraries, you have 2 options:
	
	1. Download the Botan sources from http://botan.randombit.net/download.html.
	2. Download the Botan binary (.a) from http://www.mediafire.com/?o5zhbb4leufk8d7, if available.
	
- MAC
	n/a

- LINUX

	The installation process differs depending on distribution. For example:
	
	Ubuntu: `apt-get install libbotan-1.10-0`  
	Arch Linux: `pacman -S botan`
	
==========

If you do not want to use Qt Creator IDE, you can use Makefile in the root directory of the project.
The Makefile builds Velocity with debug configuration by default, but one can explicitly set desired configuration as the parameter like this:  
`make debug` or `make release`