cmake_minimum_required(VERSION 3.6)
enable_language(C CXX)

set(APP_NAME "DisplayRefreshRateController")
set(EXE_NAME "${APP_NAME}")
set(APP_VERSION "2024.1.0")

project(${APP_NAME})

set(SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/D3DManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DisplayRefreshRateController.cpp
    )

source_group("Source Files" FILES
    ${SOURCE_FILES}
)

set(SOURCE_FILES_INTEL
    ${CMAKE_CURRENT_LIST_DIR}/Intel/IntelDisplayControllerAdapter.cpp
    )

source_group("Source Files\\intel" FILES
    ${SOURCE_FILES_INTEL}
)

set(SOURCE_FILES_IMGUI
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/backends/imgui_impl_dx12.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/backends/imgui_impl_win32.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/misc/cpp/imgui_stdlib.cpp
    )

source_group("Source Files\\imgui" FILES
    ${SOURCE_FILES_IMGUI}
)

set(SOURCE_FILES_IGCL
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/drivers.gpu.control-library/Source/cApiWrapper.cpp
    )

source_group("Source Files\\igcl" FILES
    ${SOURCE_FILES_IGCL}
)

set(HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/D3DManager.h
    ${CMAKE_CURRENT_LIST_DIR}/AppContext.h
    ${CMAKE_CURRENT_LIST_DIR}/DisplayRefreshRateController.h
    ${CMAKE_CURRENT_LIST_DIR}/DisplayControllerInterface.h
    )

source_group("Header Files" FILES
    ${HEADER_FILES}
    )

set(HEADER_FILES_INTEL
    ${CMAKE_CURRENT_LIST_DIR}/Intel/IntelDisplayControllerAdapter.h
    )

source_group("Header Files\\intel" FILES
    ${HEADER_FILES_INTEL}
    )

set(HEADER_FILES_IMGUI
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imconfig.h
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imgui.h
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imgui_internal.h
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imstb_rectpack.h
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imstb_textedit.h
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/imstb_truetype.h
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/backends/imgui_impl_dx12.h
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/backends/imgui_impl_win32.h
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/misc/cpp/imgui_stdlib.h
    )

source_group("Header Files\\imgui" FILES
    ${HEADER_FILES_IMGUI}
    )

set(HEADER_FILES_IGCL
    ${CMAKE_CURRENT_LIST_DIR}/../submodules/drivers.gpu.control-library/include/igcl_api.h
    )

source_group("Header Files\\igcl" FILES
    ${HEADER_FILES_IGCL}
    )

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(
    APP_NAME="${APP_NAME}"
    APP_NAME_W=L"${APP_NAME}"
    APP_VERSION="${APP_VERSION}"
)

add_executable(${APP_NAME} WIN32 ${SOURCE_FILES} ${SOURCE_FILES_INTEL} ${SOURCE_FILES_IMGUI} ${SOURCE_FILES_IGCL} ${HEADER_FILES} ${HEADER_FILES_INTEL} ${HEADER_FILES_IMGUI} ${HEADER_FILES_IGCL})
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})

target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui)
target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/backends)
target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../submodules/imgui/misc/cpp)
target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../submodules/drivers.gpu.control-library/include)
target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../submodules/drivers.gpu.control-library/Source)

target_link_libraries(${APP_NAME} d3d12.lib)
target_link_libraries(${APP_NAME} d3dcompiler.lib)
target_link_libraries(${APP_NAME} dxgi.lib)
