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

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(./calender_dialog)
add_subdirectory(calender_dialog)
aux_source_directory(./calender_dialog CALENDER_SRCS)

include_directories(./reminder_dialog)
add_subdirectory(reminder_dialog)
aux_source_directory(./reminder_dialog REMINDER_SRCS)

set(PROJECT_SOURCES
        main.cpp
        ${CALENDER_SRCS}
        ${REMINDER_SRCS}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MyCalender
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MyCalender APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MyCalender SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MyCalender
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MyCalender PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

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

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MyCalender)
endif()
