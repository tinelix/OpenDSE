@echo off

if "%OPENDSE_CC_TARGET%"=="" IF "%1"=="" (
	echo.

	echo You run a script designed to build the OpenDSE on Windows.
	echo.
	echo Before starting the build, you need to configure the build.
	echo Currently, two target versions of Microsoft Visual C++ are supported:
	echo.
	echo [ vc2] Microsoft Visual C++  2.0 - 2005
	echo [ vc8] Microsoft Visual C++ 2005 - 2015
	echo [vc14] Microsoft Visual C++ 2015 - 2025
	echo.
	set /P OPENDSE_CC_TARGET=Choose your MSVC target version [vc2, vc8, vc14]: 
) else (
	echo.
)

if "%OPENDSE_CC_TARGET%"=="" (set OPENDSE_CC_TARGET=vc2)


if "%OPENDSE_DEBUG%"=="" IF "%2"=="" (
	set /P OPENDSE_DEBUG=Include debug symbols before building? [y, n]: 
)

if NOT "%OPENDSE_DEBUG%"=="" (
	set DEBUG=1
) else (
	set DEBUG=
)

if NOT "%2"=="" (
	set DEBUG=1
) else (
	set DEBUG=
)

echo.

if "%OPENDSE_DEBUG%"=="" IF "%2"=="" (
	echo [1/2] Building OpenDSE from source...
) else (
	echo [1/2] Building OpenDSE with debug symbols from source...
)

cd ..\proj\msvc
nmake -nologo -f %OPENDSE_CC_TARGET%.mak DEBUG=%DEBUG%

echo.
echo Done!

cd ..\..\build

set DEBUG=
set OPENDSE_DEBUG=

echo.

pause