cmake_minimum_required(VERSION 3.29)
project(LayerMgr)

set(SRC_PATH ${PROJECT_SOURCE_DIR}/src)
set(BIN_PATH ${PROJECT_SOURCE_DIR}/build/bin)

message("OCSV dir ${OCSV_DIR}")

add_executable(
    layerMgr
    ${SRC_PATH}/main.cpp
    ${SRC_PATH}/layers.cpp
    ${SRC_PATH}/layer.cpp
    ${SRC_PATH}/img_wnd.cpp
    ${SRC_PATH}/generic_wnd.cpp
    ${SRC_PATH}/layer_props_wnd.cpp
    ${SRC_PATH}/main.rc
)

message(${OCSV_DIR})
target_include_directories(
    layerMgr
    PRIVATE
    ${OCSV_DIR}/build/include
    ${OCSV_DIR}/build/include/opencv2
)

target_link_libraries(
    layerMgr
    PRIVATE
    opencv_world500d
)

target_link_directories(
    layerMgr
    PRIVATE
    ${OCSV_DIR}/build/x64/vc16/lib 
)

target_compile_options(
    layerMgr
    PRIVATE
    /GS
    /W3
    /MDd
    /Od
    /EHc
    /std:c++17
    /Zi
    /ZI
)

target_link_options(
    layerMgr
    PRIVATE
    /machine:x64
    /subsystem:windows
    /debug
)

target_compile_definitions(
    layerMgr
    PRIVATE
    MCBS
    _MCBS
    _CRT_SECURE_NO_WARNINGS
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
    CMAKE_INTDIR="Debug"
    _DEBUG
    DEBUG
)

target_link_libraries(
    layerMgr
    PRIVATE
    kernel32
    user32
    gdi32
    winspool
    shell32
    ole32
    oleaut32
    uuid
    comdlg32
    advapi32
)

