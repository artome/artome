cmake_minimum_required(VERSION 3.5)

project(MonitoringSlave VERSION 0.1 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")
## for debug treating this as the principal project
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../cmake/Modules/")
find_package(QT NAMES Qt6  COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS PrintSupport REQUIRED)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Svg REQUIRED)

#message("lixo")
#message(${CMAKE_MODULE_PATH})
#message("lixo")
#message ("${CMAKE_SOURCE_DIR}/../../cmake/Modules/")
find_package(PLplot REQUIRED)
include_directories(${PLPLOT_INCLUDE_DIR})
include_directories(../../PLottingFramework ../../DbAPI ../../CampaignAnalysis)
#include_directories(../../PLottingFramework ../../DbAPI ../../CampaignAnalysis  ../../exprtk)
find_package(GSL REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        ../../CampaignAnalysis/plotcanvas.cpp
        ../../CampaignAnalysis/plotcanvas.h
        ../../CampaignAnalysis/plotcanvas.ui
        ../../CampaignAnalysis/timerunsmodel.cpp
        ../../CampaignAnalysis/timerunsmodel.h
        ../../CampaignAnalysis/plotdata.h
        ../../CampaignAnalysis/plotdata.cpp
        ../../CampaignAnalysis/configplotwidget.cpp
        ../../CampaignAnalysis/configplotwidget.h
        ../../CampaignAnalysis/configplotwidget.ui
        ../../CampaignAnalysis/defineevents.cpp
        ../../CampaignAnalysis/defineevents.h
        ../../CampaignAnalysis/defineevents.ui
        ../../CampaignAnalysis/plot2table.cpp
        ../../CampaignAnalysis/plot2table.h
        ../../CampaignAnalysis/plot2table.ui
        ../../CampaignAnalysis/savedataplot.cpp
        ../../CampaignAnalysis/savedataplot.h
        ../../CampaignAnalysis/savedataplot.ui
        ../../CampaignAnalysis/saveplot.cpp
        ../../CampaignAnalysis/saveplot.h
        ../../CampaignAnalysis/saveplot.ui
        ../../CampaignAnalysis/timefromruns.cpp
        ../../CampaignAnalysis/timefromruns.h
        ../../CampaignAnalysis/timefromruns.ui
        ../../CampaignAnalysis/movie.cpp
        ../../CampaignAnalysis/movie.h
        ../../CampaignAnalysis/movie.ui
        ../../CampaignAnalysis/builtcolorscheme.cpp
        ../../CampaignAnalysis/builtcolorscheme.h
        ../../CampaignAnalysis/builtcolorscheme.ui
        ../../CampaignAnalysis/plotscale.cpp
        ../../CampaignAnalysis/plotscale.h
        ../../CampaignAnalysis/plotscale.ui
        ../../CampaignAnalysis/help.cpp
        ../../CampaignAnalysis/help.h
        ../../CampaignAnalysis/help.ui
        ../../CampaignAnalysis/derivedplot.h
        ../../CampaignAnalysis/derivedplot.cpp
        ../../CampaignAnalysis/derivedplot.ui
        ../../PLottingFramework/gslwrapper.cpp
        ../../PLottingFramework/histodata.cpp
        ../../PLottingFramework/histoproprieties.cpp
        ../../PLottingFramework/plotproprieties.cpp
        ../../PLottingFramework/plotting.cpp
        ../../PLottingFramework/plplot_utils.cpp
        ../../PLottingFramework/xydata.cpp
        ../../PLottingFramework/xyproprieties.cpp
        ../../PLottingFramework/xyzdata.cpp
        ../../PLottingFramework/xyzproprieties.cpp
        ../../PLottingFramework/gslwrapper.h
        ../../PLottingFramework/histodata.h
        ../../PLottingFramework/histoproprieties.h
        ../../PLottingFramework/plotproprieties.h
        ../../PLottingFramework/plotting.h
        ../../PLottingFramework/plplot_utils.h
        ../../PLottingFramework/xydata.h
        ../../PLottingFramework/xyproprieties.h
        ../../PLottingFramework/xyzdata.h
        ../../PLottingFramework/xyzproprieties.h


        ../../DbAPI/dbapi.cpp
        ../../DbAPI/dbapi.h
        ../../Ressources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MonitoringSlave
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MonitoringSlave APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MonitoringSlave SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MonitoringSlave
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MonitoringSlave PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql  GSL::gsl GSL::gslcblas ${PLPLOT_LIBRARIES} Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::Svg)

set_target_properties(MonitoringSlave PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MonitoringSlave)
endif()
