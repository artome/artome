cmake_minimum_required(VERSION 3.5)
set(CMAKE_VERBOSE_MAKEFILE on)
project(CampaignAnalysis VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")
if(WIN32)
   set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj")
endif()



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
add_definitions( -DLIBINTERFACE_VERSION)
execute_process (
    COMMAND bash -c "git -C  ${CMAKE_SOURCE_DIR} --no-pager log -1 --format='%ai' " OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE LIBINTERFACE_VERSION
)
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/version.h")

configure_file( ${CMAKE_SOURCE_DIR}/../cmake/Modules/version_config.h.in ${CMAKE_BINARY_DIR}/version.h )


find_package(PLplot REQUIRED)
include_directories(${PLPLOT_INCLUDE_DIR})
include_directories(../PLottingFramework ../DbAPI ../exprtk ../GslWrapper ../EditHtmlEntries ${CMAKE_BINARY_DIR}/)
find_package(GSL REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        plotcanvas.cpp
        plotcanvas.h
        plotcanvas.ui
        plotdata.cpp
        plotdata.h
        configplotwidget.cpp
        configplotwidget.h
        configplotwidget.ui
        timerunsmodel.cpp
        timerunsmodel.h
        builtcolorscheme.cpp
        builtcolorscheme.h
        builtcolorscheme.ui
        defineevents.cpp
        defineevents.h
        defineevents.ui
        timefromruns.cpp
        timefromruns.h
        timefromruns.ui
        plot2table.cpp
        plot2table.h
        plot2table.ui
        savedataplot.cpp
        savedataplot.h
        savedataplot.ui
        saveplot.cpp
        saveplot.h
        saveplot.ui
        plotscale.cpp
        plotscale.h
        plotscale.ui
        movie.cpp
        movie.h
        movie.ui
        xytablemodel.cpp
        xytablemodel.h
        help.cpp
        help.h
        help.ui
        databases.cpp
        databases.h
        databases.ui
        managingtime.cpp
        managingtime.h
        managingtime.ui
        previouslogins.cpp
        previouslogins.h
        previouslogins.ui
        tablecanvas.cpp
        tablecanvas.h
        tablecanvas.ui
        downloaditensthread.cpp
        downloaditensthread.h
        TextView.cpp
        TextView.h
        TextView.ui
        savetable.cpp
        savetable.h
        savetable.ui
        hdpifactores.cpp
        hdpifactores.h
        hdpifactores.ui
        definingscreens.cpp
        definingscreens.h
        definingscreens.ui
        searchtext.cpp
        searchtext.h
        searchtext.ui
        channelwindow.cpp
        channelwindow.h
        channelwindow.ui
        buildhistogram.cpp
        buildhistogram.h
        buildhistogram.ui
        buildsurfaceplot.cpp
        buildsurfaceplot.h
        buildsurfaceplot.ui
        editlocalplots.cpp
        editlocalplots.h
        editlocalplots.ui
        derivedplot.h
        derivedplot.cpp
        derivedplot.ui
        statisticswindow.cpp
        statisticswindow.h
        statisticswindow.ui
        loadlocalfile.h
        loadlocalfile.ui
        loadlocalfile.cpp
        inststatus.cpp
        inststatus.h
        inststatus.ui
        ../PLottingFramework/gslwrapper.cpp
        ../PLottingFramework/histodata.cpp
        ../PLottingFramework/histoproprieties.cpp
        ../PLottingFramework/plotproprieties.cpp
        ../PLottingFramework/plotting.cpp
        ../PLottingFramework/plplot_utils.cpp
        ../PLottingFramework/xydata.cpp
        ../PLottingFramework/xyproprieties.cpp
        ../PLottingFramework/xyzdata.cpp
        ../PLottingFramework/xyzproprieties.cpp
        ../PLottingFramework/gslwrapper.h
        ../PLottingFramework/histodata.h
        ../PLottingFramework/histoproprieties.h
        ../PLottingFramework/plotproprieties.h
        ../PLottingFramework/plotting.h
        ../PLottingFramework/plplot_utils.h
        ../PLottingFramework/xydata.h
        ../PLottingFramework/xyproprieties.h
        ../PLottingFramework/xyzdata.h
        ../PLottingFramework/xyzproprieties.h
        ../DbAPI/dbapi.cpp
        ../DbAPI/dbapi.h
        ../exprtk/exprtk.hpp
        ../GslWrapper/gslkindofwrapper.cpp
        ../GslWrapper/gslkindofwrapper.h
        ../EditHtmlEntries/maketext.cpp
        ../EditHtmlEntries/maketext.h
        ../EditHtmlEntries/maketext.ui
        ../EditHtmlEntries/Textedit.cpp
        ../EditHtmlEntries/Textedit.h
        ../EditHtmlEntries/addnewentry.cpp
        ../EditHtmlEntries/addnewentry.h
        ../EditHtmlEntries/addnewentry.ui
        ../Ressources.qrc
        ${version_file}

)
source_group(ui FILES
   mainwindow.ui
   plotcanvas.ui
   configplotwidget.ui
   builtcolorscheme.ui
   defineevents.ui
   timefromruns.ui
   plot2table.ui
   savedataplot.ui
   saveplot.ui
   plotscale.ui
   movie.ui
   help.ui
   databases.ui
   managingtime.ui
   previouslogins.ui
   tablecanvas.ui
   TextView.ui
   savetable.ui
   hdpifactores.ui
   definingscreens.ui
   searchtext.ui
   channelwindow.ui
   buildhistogram.ui
   buildsurfaceplot.ui
   editlocalplots.ui
   derivedplot.ui
   statisticswindow.ui
   loadlocalfile.ui
   inststatus.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CampaignAnalysis
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(CampaignAnalysis SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(CampaignAnalysis
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(CampaignAnalysis PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::Sql
   GSL::gsl GSL::gslcblas  ${PLPLOT_LIBRARIES} Qt${QT_VERSION_MAJOR}::Svg)


set_target_properties(CampaignAnalysis PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CampaignAnalysis)
endif()
