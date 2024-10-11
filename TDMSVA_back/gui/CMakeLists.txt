cmake_minimum_required(VERSION 3.10)

project(massdb_gui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")

set(PROJECT_SOURCES
        main.cpp
        massdbwidget.cpp
        massdbwidget.h
        massdbwidget.ui
        icon.qrc
        )

add_executable(massdb_gui ${PROJECT_SOURCES})

target_link_libraries(massdb_gui PRIVATE Qt5::Widgets)