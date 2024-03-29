cmake_minimum_required(VERSION 3.20)

project(Reader LANGUAGES C CXX
        VERSION 0.3.0
        DESCRIPTION "a simple epub-file reader"
        HOMEPAGE_URL "https://github.com/yyhran/epubreader"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ../res/icon.o")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui Xml REQUIRED)
if(Qt5_FOUND)
    message("Qt5_FOUND")
endif()

message("------------------------------------------------------------")
message("PROJECT_NAME: " ${PROJECT_NAME})
message("PROJECT_VERSION: " ${PROJECT_VERSION})
message("DESCRIPTION: " ${PROJECT_DESCRIPTION})
message("HOMEPAGE_URL: " ${PROJECT_HOMEPAGE_URL})
message("------------------------------------------------------------")

include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32)
else()
    add_executable(${PROJECT_NAME})
endif()


# file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.cpp *.h)
file(GLOB sources CONFIGURE_DEPENDS res/res.qrc src/*.cpp src/*.h)

target_sources(${PROJECT_NAME} PUBLIC ${sources})

target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Xml z)
