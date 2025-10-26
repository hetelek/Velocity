# MSVC toolchain file for Ninja generator
# Forces use of MSVC compiler and linker to avoid conflicts with MinGW

set(CMAKE_C_COMPILER cl)
set(CMAKE_CXX_COMPILER cl)
set(CMAKE_LINKER link)

# Set Qt path for MSVC
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.3/msvc2022_64" CACHE PATH "Qt installation path")
