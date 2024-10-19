The Qt Creator IDE is used to build/debug Velocity. Velocity currently depends on Qt6 and Botan 3.4

==========

- WINDOWS

	Velocity/XboxInternals can be compiled with 'Qt 6.7 for Desktop - MingGW (11.2.0)' or higher (tested with QT 6.8.0 MingGW 13.1.0).

	Tool Chain: 'Mingw as a GCC for Windows targets'

	WARNING: Botan must be found in C:/botan/[lib/include] directory for XboxInternals to successfully compile. To obtain the Botan libraries, you have 2 options:

	1. Download the Botan sources from http://botan.randombit.net/download.html.
			$ python.exe .\configure.py --cc=gcc --os=mingw --build-tool=ninja  --without-documentation  --without-stack-protector
			$ ninja
			$ ninja check
			$ ninja install
	2. Download the Botan binary (.a) and headers from https://www.mediafire.com/file/i77h4kgrsj6vo9l/botan-3.5.0_win64.7z, if available.

	* For WINDOWS platform, make sure you have X:\Qt\Tools\Ninja and X:\Qt\Tools\mingw1310_64\bin in your PATH
	
- MAC

	Botan can be installed using Homebrew by typing `brew install botan` into your terminal.

- LINUX

	The installation process differs depending on distribution. For example:

	Ubuntu: `apt-get install libbotan-3-0`
	Arch Linux: `pacman -S botan`

==========

If you do not want to use Qt Creator IDE, you can use Makefile in the root directory of the project.
The Makefile builds Velocity with debug configuration by default, but one can explicitly set desired configuration as the parameter like this:
`make debug` or `make release`

* For WINDOWS platform, make sure you have X:\Qt\6.8.0\mingw_64\bin and X:\Qt\Tools\mingw1310_64\bin in your PATH