cmake_minimum_required(VERSION 3.5)

project(MyCalender VERSION 0.1 LANGUAGES CXX)

#用QtCreator时这个变量通常可以省略,如果需要指定就换成你的对应Qt目录
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt/")
#
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Network Svg Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network Svg Sql)

SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin/")

include_directories(main_window)
add_subdirectory(main_window)
aux_source_directory(main_window MAIN_WINDOW_SRCS)

include_directories(./reminder_dialog)
add_subdirectory(reminder_dialog)
aux_source_directory(./reminder_dialog REMINDER_SRCS)

include_directories(./weather)
add_subdirectory(weather)
aux_source_directory(./weather WEATHER_SRCS)

include_directories(./login_dialog)
add_subdirectory(login_dialog)
aux_source_directory(./login_dialog LOGIN_SRCS)

#file(COPY ./resource DESTINATION ${EXECUTABLE_OUTPUT_PATH})

qt5_add_resources(QRC_WRAPPERS ./resource/resource.qrc)

set(PROJECT_SOURCES
        main.cpp
        ${MAIN_WINDOW_SRCS}
        ${REMINDER_SRCS}
        ${WEATHER_SRCS}
        ${LOGIN_SRCS}
        )

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MyCalender
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET MyCalender APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else ()
    if (ANDROID)
        add_library(MyCalender SHARED
                ${PROJECT_SOURCES}
                ${QRC_WRAPPERS}
                )
        # Define properties for Android with Qt 5 after find_package() calls as:
        #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else ()
        add_executable(MyCalender
                ${PROJECT_SOURCES}
                ${QRC_WRAPPERS}
                )
    endif ()
endif ()

target_link_libraries(MyCalender
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Svg
        Qt${QT_VERSION_MAJOR}::Sql
        )

set_target_properties(MyCalender PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
        )

install(TARGETS MyCalender
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MyCalender)
endif ()
