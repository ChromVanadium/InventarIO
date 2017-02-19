@echo off 
set /p var= <build.txt 
set /a var= %var%+1 
echo %var% >build.txt
echo #define BUILD %var% >build.h
echo %var%

@SET BD=%DATE:~0,2%.%DATE:~3,2%.%DATE:~6,4%
@SET BT=%TIME:~0,2%:%TIME:~3,2%:%TIME:~6,2%

echo #define BUILDDATE "%BD%" >>build.h
echo #define BUILDTIME "%BT%" >>build.h