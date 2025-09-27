@echo off
REM Get current git branch name
for /f "delims=" %%b in ('git branch --show-current') do set "branch=%%b"

set "filename=%branch%.exe"
echo Compiling %filename%

REM Uncomment this block if you want to compile files individually like in your bash script
REM echo Starting %filename% compilation...
REM for %%c in (*.c) do (
REM     echo     Compiling %%c...
REM     gcc -c "%%c" -o ..\build\obj\%%~nc.o
REM )

clang-cl %* *.c -o ..\build\%filename% -lm
