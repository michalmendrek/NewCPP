set(MODULE_NAME "MLCommon")
cmake_minimum_required(VERSION 3.5.1)

project(${PROJECT_NAME} LANGUAGES ASM C CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-pthread -Wall -pedantic")

include("CMake/GTest.cmake")
include("CMake/lizard.cmake")
integrate_google_test()
enable_testing()

file(GLOB STM "inc/Stm/*.h")
file(GLOB TIMER "src/TimerFramework/*.cpp"
                "inc/TimerFramework/*.h")
file(GLOB NETWORK "src/Network/*.cpp"
                  "inc/Network/*.h")
file(GLOB NETWORK_IPC "src/Network/Ipc/*.cpp"
                      "inc/Network/Ipc/*.h")
if (UNIX)
    file(GLOB NETWORK_OTHER_SOURCES "src/Network/Unix/*.cpp"
                                    "inc/Network/Unix/*.h")
elseif (WIN32)
    file(GLOB NETWORK_OTHER_SOURCES "src/Network/Windows/*.cpp"
                                    "inc/Network/Windows/*.h")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

file(GLOB TOOLS "src/Tools/*.cpp"
                "inc/Tools/*.h")
file(GLOB TYPES "src/Types/*.cpp"
                "inc/Types/*.h")
file(GLOB DATFILE "src/SensorData/DatFile/*.cpp"
                  "inc/SensorData/DatFile/*.h")
file(GLOB FRAME "src/SensorData/Frame/*.cpp"
                "inc/SensorData/Frame/*.h")
file(GLOB STREAM "src/SensorData/Stream/*.cpp"
                 "inc/SensorData/Stream/*.h")
file(GLOB CONVERTER "src/Converter/*.cpp"
                    "inc/Converter/*.h")
file(GLOB CONFIGURATION "src/Configuration/*.cpp"
                        "inc/Configuration/*.h")

add_library(${MODULE_NAME} STATIC
  ${STM}
  ${TIMER}
  ${NETWORK}
  ${NETWORK_IPC}
  ${NETWORK_OTHER_SOURCES}
  ${TOOLS}
  ${TYPES}
  ${DATFILE}
  ${FRAME}
  ${STREAM}
  ${CONVERTER}
  ${CONFIGURATION}
)

link_libraries(${MODULE_NAME} pthread)

if (WIN32)
target_link_libraries(${MODULE_NAME} libshlwapi.a)
target_link_libraries(${MODULE_NAME} libws2_32.a)
endif(WIN32)

target_include_directories(${MODULE_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE
    src
    inc/TimerFramework
    inc/Network
    inc/Network/Ipc
if (UNIX)
    inc/Network/Unix
elseif (WIN32)
    inc/Network/Windows
endif()
    inc/Tools
    inc/Types
    inc/SensorData/DatFile
    inc/SensorData/Frame
    inc/Converter
    inc/Configuration
)

add_subdirectory(test)
