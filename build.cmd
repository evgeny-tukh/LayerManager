@echo off

if [%1]==[--open-csv-dir] (
    set open_csv=-DOCSV_DIR=%2
)

set cmake="C:\Program Files\CMake\bin\cmake.exe"

if not exist build (
    md build
)

cd build

%cmake% -DCMAKE_BUILD_TYPE=Debug %open_csv% ..

if errorlevel 1 exit

%cmake% --build .

