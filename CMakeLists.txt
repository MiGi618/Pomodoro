cmake_minimum_required(VERSION 3.5)

project(QtPromoto VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg Multimedia Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg Core Multimedia Gui)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        circularprogress.cpp
        circularprogress.h
        app.qrc
        settingsdialog.cpp
        settingsdialog.h
        taskprogressbar.cpp
        taskprogressbar.h
        task.h
        taskeditdialog.h
        taskeditdialog.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QtPromoto
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(QtPromoto
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(QtPromoto PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)

set_target_properties(QtPromoto PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QtPromoto)
endif()
