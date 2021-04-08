:: run this from developer console of visual studio

set VERSION=v0.0.2

set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%\..\

set DLL_32_DIR=%ROOT_DIR%\bin\Win32\Release\
set DLL_32=DotNetHooker32.dll
set DLL_64_DIR=%ROOT_DIR%\bin\x64\Release\
set DLL_64=DotNetHooker64.dll

set INSTALL_SCRIPT=install.bat
set RUN_SCRIPT=run.bat

msbuild %ROOT_DIR%\DotNetHooker.sln -property:Configuration=Release,Platform=x64 -target:rebuild
msbuild %ROOT_DIR%\DotNetHooker.sln -property:Configuration=Release,Platform=x86 -target:rebuild

tar.exe -a -c -f %ROOT_DIR%\bin\DotNetHooker_%VERSION%.zip -C %DLL_32_DIR% %DLL_32% -C %DLL_64_DIR% %DLL_64% -C %SCRIPT_DIR% %INSTALL_SCRIPT% %RUN_SCRIPT%
