cmake_minimum_required(VERSION 3.29)
project(LayerMgr)

set(SRC_PATH ${PROJECT_SOURCE_DIR}/src)
set(UI_PATH ${SRC_PATH}/ui)
set(JSON_PATH ${SRC_PATH}/json)
set(MODEL_PATH ${SRC_PATH}/model)
set(OPER_PATH ${MODEL_PATH}/operations)
set(BIN_PATH ${PROJECT_SOURCE_DIR}/build/bin)

message("OCSV dir ${OCSV_DIR}")

set(
    MODEL_FILES
    ${MODEL_PATH}/stage.cpp
    ${MODEL_PATH}/layers.cpp
    ${MODEL_PATH}/layer.cpp
    ${MODEL_PATH}/tools.cpp
    ${MODEL_PATH}/stages_stack.cpp
)

set(
    OPER_FILES
    ${OPER_PATH}/operation.cpp
    ${OPER_PATH}/params.cpp
    ${OPER_PATH}/from_file.cpp
    ${OPER_PATH}/rectangle.cpp
)

set (
    UI_FILES
    ${UI_PATH}/img_wnd.cpp
    ${UI_PATH}/generic_wnd.cpp
    ${UI_PATH}/layer_props_wnd.cpp
    ${UI_PATH}/status_bar.cpp
    ${UI_PATH}/win_tools.cpp
    ${UI_PATH}/layer_base_props_editor.cpp
)

add_executable(
    layerMgr
    ${MODEL_FILES}
    ${OPER_FILES}
    ${UI_FILES}
    ${JSON_PATH}/json_lite.cpp
    ${SRC_PATH}/main.cpp
    ${SRC_PATH}/main.rc
)

message(${OCSV_DIR})
target_include_directories(
    layerMgr
    PRIVATE
    ${SRC_PATH}
    ${OPER_PATH}
    ${MODEL_PATH}
    ${UI_PATH}
    ${JSON_PATH}
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
    comctl32
)

