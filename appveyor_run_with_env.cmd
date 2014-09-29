:: We need to configure environment
:: variables to use the MSVC 2010 C++ compilers from GRMSDKX_EN_DVD.iso of:
:: MS Windows SDK for Windows 7 and .NET Framework 4 (SDK v7.1)
::
:: Note: this script needs to be run with the /E:ON and /V:ON flags for the
:: cmd interpreter, at least for (SDK v7.0)
::
:: Author: Pavel Kalian
:: Based on https://github.com/ogrisel/python-appveyor-demo/blob/master/appveyor/run_with_env.cmd
:: Author: Olivier Grisel
:: License: CC0 1.0 Universal: http://creativecommons.org/publicdomain/zero/1.0/
@ECHO OFF

SET COMMAND_TO_RUN=%*
SET WIN_SDK_ROOT=C:\Program Files\Microsoft SDKs\Windows
SET PATH=%PATH%;C:\wxWidgets-2.8.12

SET WINDOWS_SDK_VERSION="v7.1"

ECHO Using default MSVC build environment for 32 bit architecture
ECHO Executing: %COMMAND_TO_RUN%
call %COMMAND_TO_RUN% || EXIT 1
