cmake_minimum_required(VERSION 3.16.0)
set(LIB_NAME CuteEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Multimedia REQUIRED)

aux_source_directory(./src SRCS)
file(GLOB INCLUDE_FILES "./include/*.h")

## Header files should also be added to the target's sources, or the MOC won't be able to find it.
add_library(${LIB_NAME} SHARED ${SRCS} ${INCLUDE_FILES} ./res.qrc)

target_link_libraries(${LIB_NAME} PUBLIC Qt5::Multimedia Qt5::Widgets Qt5::Core)
target_include_directories(${LIB_NAME} PUBLIC ./include)
