cmake_minimum_required(VERSION 3.10)
project(massdb)

#------------------------------ 编译器设置 ------------------------------

# SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
# SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

SET(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_PREFIX_PATH "C:/msys2/msys64/mingw64")

SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto -std=c++17 -DWITHOUT_MZ5 -fPIC")
SET(CMAKE_BUILD_TYPE "Debug")

SET(GUI_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/massdbwidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/massdbwidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/massdbwidget.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/icon.qrc
        gui/file_str.h util/mod_total.cpp util/mod_total.h util/deMs1.h util/PrSM.h)

#IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
#    # Windows specific code
#    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -std=c++17 -O3 -DWITHOUT_MZ5 -fPIC -fpermissive")
#ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

#------------------------------ 导入依赖文件 ------------------------------

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTOPPIC_WINDOWS -DWIN32")
    SET(JSON_Install OFF CACHE INTERNAL "")
    include_directories(
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/src
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/ext
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/crow/include

            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/leveldb/include
            ${CMAKE_CURRENT_SOURCE_DIR}/util
            ${CMAKE_CURRENT_SOURCE_DIR}/ms_interface
    )
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/leveldb)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/crow)

# pwiz
file(GLOB PWIZ_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/ext/pwiz/utility/minimxml/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/ext/pwiz/utility/misc/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/ext/pwiz/data/common/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/ext/pwiz/data/msdata/*.cpp)

# common
file(GLOB_RECURSE COMMON_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/src/common/*.cpp)

# ms
file(GLOB SPEC_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/src/ms/spec/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/src/ms/env/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/src/ms/feature/*.cpp)

# classes for topfd
file(GLOB_RECURSE TOPFD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/toppic-suite/src/topfd/*.cpp)


file(GLOB MS_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ms_interface/*.cpp)
file(GLOB UTIL_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/util/*.cpp)
file(GLOB NETWORK_SRC ${CMAKE_CURRENT_SOURCE_DIR}/network/*.cpp)

file(GLOB MASSDB_TEST ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp)
file(GLOB DB_TEST ${CMAKE_CURRENT_SOURCE_DIR}/test/db_test.cpp)
file(GLOB DB_TEST ${CMAKE_CURRENT_SOURCE_DIR}/test/json_test.cpp)

#------------------------------ 设置可执行文件 ----------------------------

add_executable(massdb_test
        ${MASSDB_TEST}
        ${UTIL_SRCS}
        ${MS_INTERFACE}
        ${TOPFD_SRCS}
        ${COMMON_SRCS}
        ${PARA_SRCS}
        ${SPEC_SRCS}
        ${PWIZ_SRCS})

#add_executable(min_test ${CMAKE_CURRENT_SOURCE_DIR}/test/min.cpp)

#add_executable(db_test ${CMAKE_CURRENT_SOURCE_DIR}/test/db_test.cpp)

#add_executable(web_test ${CMAKE_CURRENT_SOURCE_DIR}/test/web_test.cpp)

add_executable(massdb_gui
        ${GUI_SOURCES}
        ${UTIL_SRCS}
        ${MS_INTERFACE}
        ${TOPFD_SRCS}
        ${COMMON_SRCS}
        ${PARA_SRCS}
        ${SPEC_SRCS}
        ${PWIZ_SRCS})

add_executable(network_server
        ${NETWORK_SRC}
        ${UTIL_SRCS}
        ${MS_INTERFACE}
        ${TOPFD_SRCS}
        ${COMMON_SRCS}
        ${PARA_SRCS}
        ${SPEC_SRCS}
        ${PWIZ_SRCS})

#------------------------------ 配置链接库 --------------------------------

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(massdb_test
            leveldb
            xerces-c
            boost_program_options-mt
            boost_filesystem-mt
            boost_system-mt
            boost_thread-mt
            boost_iostreams-mt
            boost_chrono-mt
            pthread
            z
            bcrypt
            Ws2_32)

    #target_link_libraries(db_test leveldb)

    #target_link_libraries(min_test bcrypt)

    #target_link_libraries(web_test PRIVATE Crow::Crow Ws2_32 wsock32)

    target_link_libraries(massdb_gui PRIVATE
            Qt5::Widgets
            leveldb

            xerces-c
            boost_program_options-mt
            boost_filesystem-mt
            boost_system-mt
            boost_thread-mt
            boost_iostreams-mt
            boost_chrono-mt
            pthread
            z
            bcrypt
            Ws2_32
            )

    target_link_libraries(network_server
            leveldb
            xerces-c
            boost_program_options-mt
            boost_filesystem-mt
            boost_system-mt
            boost_thread-mt
            boost_iostreams-mt
            boost_chrono-mt
            pthread
            z
            bcrypt
            Ws2_32

            Crow::Crow
            wsock32
            )

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")