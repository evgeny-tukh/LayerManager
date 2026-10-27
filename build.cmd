@echo off

set cmake="C:\Program Files\CMake\bin\cmake.exe"

if not exist build (
    md build
)

cd build

%cmake% -DCMAKE_BUILD_TYPE=Debug ..

if errorlevel 1 exit

%cmake% --build .

