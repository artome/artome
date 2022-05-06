cmake_minimum_required(VERSION 3.5)
set(CMAKE_VERBOSE_MAKEFILE on)
project(EditHtmlEntries VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()
if(USEQT5)
    find_package(QT NAMES Qt5  COMPONENTS Widgets REQUIRED)
else()
    find_package(QT NAMES Qt6 PATH /usr/lib64/cmake/Qt6/ COMPONENTS Widgets REQUIRED)
endif()
#set(QT_VERSION_MAJOR 6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Svg REQUIRED)


include_directories(../DbAPI ../CampaignSetup )

set(PROJECT_SOURCES
    main.cpp
    ../CampaignSetup/server.cpp 
    ../DbAPI/dbapi.cpp 
        Textedit.cpp
        addnewentry.cpp
        mainwindow.cpp
        maketext.cpp
        ../CampaignSetup/server.h
        ../DbAPI/dbapi.h
        Textedit.h
        addnewentry.h
        mainwindow.h
        maketext.h
        ../CampaignSetup/server.ui
        addnewentry.ui
        mainwindow.ui
        maketext.ui
   ../Ressources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(EditHtmlEntries
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(EditHtmlEntries SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(EditHtmlEntries
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(EditHtmlEntries PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Svg)

set_target_properties(EditHtmlEntries PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EditHtmlEntries)
endif()
