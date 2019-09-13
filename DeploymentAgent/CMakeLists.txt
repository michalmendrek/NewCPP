cmake_minimum_required(VERSION 3.1)

project(DeploymentAgent LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "-Wall -pedantic")
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/")

find_package(UDev REQUIRED)
if(NOT UDEV_FOUND)
    message(FATAL_ERROR "udev library not found")
endif()
include_directories(${UDEV_INCLUDE_DIR})


set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(THIRD_PARTY ${REPO_ROOT}/ThirdParty)
set(COMMON ${REPO_ROOT}/MLPlatform/MLCommon/inc)

include_directories(${COMMON})
include_directories(${THIRD_PARTY})
include_directories(src)

add_executable(${PROJECT_NAME}
    src/configuration.h
    src/deploy_pipeline.h
    src/mount_point_resolver.h
    src/usb_processing_pipeline.h
    src/archive_detector.h
    src/temporary_folder.h
    src/install_package_detector.h
    src/file_mover.h
    src/file_remover.h
    src/usb_mass_storage_monitor.h
    src/folder_monitor.h
    src/system_observable.h
    src/system_cmd_executor.h
    src/system_observer.h
    src/unpacker.h
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    udev
)
