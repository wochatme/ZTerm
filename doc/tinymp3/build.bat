echo off
echo ======================Usage======================
echo Step 1: Please install Visual Studio 2022 at first
echo Step 2: Open "x64 Native Tools Command Prompt for VS 2022"

del *.obj
del *.exe

REM compile the tinymp3 software 
cl tinymp3.cpp

del *.obj
echo =================================================
echo Try to type: tinymp3.exe mp3_file_name
echo =================================================
echo Enjoy it!
