@echo off
echo Setting up environment...
echo.
echo.
set PATH=C:\Qt\6.8.0\mingw_64\bin;C:/Qt/Tools/mingw1310_64/bin;%PATH%

cd C:\Qt\6.8.0\mingw_64\bin
lrelease-pro.exe D:\OMSI-Tools\OMSI-Tools\OMSI-Tools.pro

echo ------------------------------------------------------------------------------------------
echo Finished.
timeout 4