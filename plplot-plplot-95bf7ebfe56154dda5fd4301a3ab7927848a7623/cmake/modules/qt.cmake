# cmake/modules/qt.cmake
#
# Copyright (C) 2009-2020 Alan W. Irwin
#
# This file is part of PLplot.
#
# PLplot is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# PLplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the file PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

# Module for determining all configuration variables related to the qt
# device driver, the plplotqt library, and either the plplot_pyqt4 or
# plplot_pyqt5 Python extension modules.
# The following variables are set/modified:
# PLD_bmpqt		  - ON means the bmpqt device is enabled.
# PLD_jpgqt		  - ON means the jpgqt device is enabled.
# PLD_pngqt		  - ON means the pngqt device is enabled.
# PLD_ppmqt		  - ON means the ppmqt device is enabled.
# PLD_tiffqt		  - ON means the tiffqt device is enabled.
# PLD_epsqt		  - ON means the epsqt device is enabled.
# PLD_pdfqt		  - ON means the pdfqt device is enabled.
# PLD_qtwidget		  - ON means the qtwidget device is enabled.
# PLD_svgqt		  - ON means the svgqt device is enabled.
# PLD_extqt		  - ON means the extqt device is enabled.
# PLD_memqt               - ON means the memqt device is enabled.
# PLPLOT_USE_Qt5          - ON means the Qt5 (rather than Qt4) libraries are used.
# pc_qt_COMPILE_FLAGS	  - Space-separated Qt-related COMPILE_FLAGS used strictly
#                           just for the pkg-config configuration case.
# pc_qt_LIBRARIES_LIST    - List of Qt-related libraries used for just
#                           the pkg-config configuration and ocaml static build cases.
#                           Note it is the same as
#                           QT_LIBRARIES for the Qt4 case so for src/CMakeLists.txt
#                           logic is required to avoid this redundancy.
# qt_COMPILE_FLAGS	  - empty.  Not needed. COMPILE_FLAGS are taken care of explicitly in the
#                           Qt5 case by a special target_link_libraries call and taken care of
#                           explicitly in the Qt4 case by the function
#                           set_qt4_target_properties(<target>) defined by "include(ndp_UseQt4)".
# qt_LINK_FLAGS	  	  - empty.  Not needed. Linking is taken care of explicitly in the
#                           Qt5 case by a special target_link_libraries call and taken care of
#                           explicitly in the Qt4 case by the QT_LIBRARIES variable that is defined
#                           by "include(ndp_UseQt4)".
# qt_TARGETS		  - list of targets which the qt dynamic device
#			    depends on.
# qt_RPATH	       	  - RPATH directory list for qt device driver.
# DRIVERS_LINK_FLAGS  	  - list of device LINK_FLAGS for case when
#			    ENABLE_DYNDRIVERS OFF.  (qt_TARGETS not included
#			    since source code used instead of this target
#			    for ENABLE_DYNDRIVERS OFF.)
# qt_SOURCE		  - list of source files to be appended to qt.cpp.
# ENABLE_qt		  - ON means the plplot_qt library is enabled.
# ENABLE_pyqt4		  - ON means the plplot_pyqt4 Python extension module
# 			    is enabled.
# ENABLE_pyqt5		  - ON means the plplot_pyqt5 Python extension module
# 			    is enabled.
# ENABLE_smoke            - ON means the smoke plplotqt library is enabled.
# SIP_EXECUTABLE	  - full path for sip
# PYQT_SIP_DIR		  - sip system directory containing QtCore/QtCoremod.sip (and other required sip files).
# PYQT_SIP_FLAGS	  - sip command flags

if(DEFAULT_NO_BINDINGS)
  option(ENABLE_qt "Enable Qt binding" OFF)
  option(ENABLE_pyqt4 "Enable pyqt4 Python extension module" OFF)
  option(ENABLE_pyqt5 "Enable pyqt5 Python extension module" OFF)
  option(ENABLE_smoke "Enable smoke Qt binding" OFF)
else(DEFAULT_NO_BINDINGS)
  option(ENABLE_qt "Enable Qt binding" ON)
  option(ENABLE_pyqt4 "Enable pyqt4 Python extension module" ON)
  option(ENABLE_pyqt5 "Enable pyqt5 Python extension module" ON)
  # Still experimental so this should default to OFF, but the user
  # has the option to turn it ON.
  option(ENABLE_smoke "Enable smoke Qt binding" OFF)
endif(DEFAULT_NO_BINDINGS)

if(ENABLE_qt)
  # Our experience now (since the Qt5 font configuration fix) is Qt5 is just as good as Qt4.
  # So use Qt5 by default.
  set(Qt_library_name_list
        Core
        Gui
        PrintSupport
        Widgets
        Svg
        )
  option(PLPLOT_USE_QT6 "Use Qt6" ON)

  if(NOT PLPLOT_USE_QT6)

    find_package(Qt5 5.7.1 COMPONENTS Svg Gui PrintSupport Widgets)

  get_target_property(qt_core_interface_compile_options Qt5::Core INTERFACE_COMPILE_OPTIONS)

  endif(NOT PLPLOT_USE_QT6)
   if(PLPLOT_USE_QT6)

    find_package(Qt6 COMPONENTS Svg Gui PrintSupport Widgets)
    get_target_property(qt_core_interface_compile_options Qt6::Core INTERFACE_COMPILE_OPTIONS)


  endif(PLPLOT_USE_QT6)

    if(qt_core_interface_compile_options)
	set(pc_qt_COMPILE_FLAGS ${qt_core_interface_compile_options})
	string(REGEX REPLACE ";" " " pc_qt_COMPILE_FLAGS ${pc_qt_COMPILE_FLAGS})
      else(qt_core_interface_compile_options)
	set(pc_qt_COMPILE_FLAGS)
      endif(qt_core_interface_compile_options)

      foreach(Qt_library_name ${Qt_library_name_list})
   if(NOT PLPLOT_USE_QT6)
	if(Qt5${Qt_library_name}_EXECUTABLE_COMPILE_FLAGS)
	  set(pc_qt_COMPILE_FLAGS "${pc_qt_COMPILE_FLAGS} ${Qt5${Qt5_library_name}_EXECUTABLE_COMPILE_FLAGS}")
	endif(Qt5${Qt_library_name}_EXECUTABLE_COMPILE_FLAGS)
	endif(NOT PLPLOT_USE_QT6)
	if(PLPLOT_USE_QT6)
	if(Qt6${Qt_library_name}_EXECUTABLE_COMPILE_FLAGS)
	  set(pc_qt_COMPILE_FLAGS "${pc_qt_COMPILE_FLAGS} ${Qt5${Qt5_library_name}_EXECUTABLE_COMPILE_FLAGS}")
	endif(Qt6${Qt_library_name}_EXECUTABLE_COMPILE_FLAGS)
	endif(PLPLOT_USE_QT6)

        string(TOUPPER ${Qt_library_name} macro_core_name)
        # Set required macros so headers will be found.
        set(pc_qt_COMPILE_FLAGS "${pc_qt_COMPILE_FLAGS} -DQT_${macro_core_name}_LIB")
        find_file(${Qt_library_name}_header_directory Qt${Qt_library_name} HINTS ${Qt${Qt_library_name}_INCLUDE_DIRS})
            get_filename_component(parent_directory ${${Qt_library_name}_header_directory} DIRECTORY)
          set(pc_qt_COMPILE_FLAGS "${pc_qt_COMPILE_FLAGS} -I/usr/include/qt6/${${Qt_library_name}}")
     endforeach(Qt_library_name ${Qt5_library_name_list})
      message(STATUS "Qt pc_qt_COMPILE_FLAGS = ${pc_qt_COMPILE_FLAGS}")

      set(pc_qt_LIBRARIES_LIST)
      foreach(Qt_library_name ${Qt_library_name_list})

    if(NOT PLPLOT_USE_QT6)
        set(Qt_library_fullpath ${Qt5${Qt_library_name}_LIBRARIES})
  endif(NOT PLPLOT_USE_QT6)

    if( PLPLOT_USE_QT6)
    message(STATUS "WARNING: merda")
        set(Qt_library_fullpath ${Qt6${Qt_library_name}_LIBRARIES})
  endif(PLPLOT_USE_QT6)
message(STATUS "WARNING: ${Qt_library_fullpath}")
          get_target_property(Qt_library_fullpath ${Qt_library_fullpath} LOCATION)
            list(APPEND pc_qt_LIBRARIES_LIST ${Qt_library_fullpath})

  endforeach(Qt_library_name ${Qt_library_name_list})

endif(ENABLE_qt)

# MAINTENANCE: mention every qt device here.
if(PLD_bmpqt OR PLD_jpgqt OR PLD_pngqt OR PLD_ppmqt OR PLD_tiffqt OR PLD_epsqt OR PLD_pdfqt OR PLD_qtwidget OR PLD_svgqt OR PLD_extqt OR PLD_memqt)
  set(ANY_QT_DEVICE ON)
endif(PLD_bmpqt OR PLD_jpgqt OR PLD_pngqt OR PLD_ppmqt OR PLD_tiffqt OR PLD_epsqt OR PLD_pdfqt OR PLD_qtwidget OR PLD_svgqt OR PLD_extqt OR PLD_memqt)

if(ANY_QT_DEVICE)
  if(NOT CMAKE_CXX_COMPILER_WORKS)
    message(STATUS "WARNING: no working C++ compiler so disabling all qt devices")
    set(ANY_QT_DEVICE OFF)
  endif(NOT CMAKE_CXX_COMPILER_WORKS)
endif(ANY_QT_DEVICE)

# All qt devices depend on ENABLE_qt
if(NOT ENABLE_qt AND ANY_QT_DEVICE)
  message(STATUS "WARNING: ENABLE_qt is OFF so setting all qt devices to OFF")
  set(ANY_QT_DEVICE OFF)
endif(NOT ENABLE_qt AND ANY_QT_DEVICE)

# MAINTENANCE: must mention all qt devices here.
if(NOT ANY_QT_DEVICE)
  set(PLD_bmpqt OFF CACHE BOOL "Enable Qt Windows bmp device" FORCE)
  set(PLD_jpgqt OFF CACHE BOOL "Enable Qt jpg device" FORCE)
  set(PLD_pngqt OFF CACHE BOOL "Enable Qt png device" FORCE)
  set(PLD_ppmqt OFF CACHE BOOL "Enable Qt ppm device" FORCE)
  set(PLD_tiffqt OFF CACHE BOOL "Enable Qt tiff device" FORCE)
  set(PLD_epsqt OFF CACHE BOOL "Enable Qt EPS device" FORCE)
  set(PLD_pdfqt OFF CACHE BOOL "Enable Qt PDF device" FORCE)
  set(PLD_qtwidget OFF CACHE BOOL "Enable Qt interactive device" FORCE)
  set(PLD_svgqt OFF CACHE BOOL "Enable Qt SVG device" FORCE)
  set(PLD_extqt OFF CACHE BOOL "Enable Qt ext device" FORCE)
  set(PLD_memqt OFF CACHE BOOL "Enable Qt mem device" FORCE)
endif(NOT ANY_QT_DEVICE)

# The libplplotqt build depends on MOC file generation which
# depends on at least some qt devices being
# enabled.  Therefore, disable the library build if there
# are no qt devices.  (In any case, I think that library build
# would not be useful for much without devices.)

if(NOT ANY_QT_DEVICE AND ENABLE_qt)
  message(STATUS "WARNING: ANY_QT_DEVICE is OFF so setting ENABLE_qt to OFF")
  set(ENABLE_qt OFF CACHE BOOL "Enable Qt binding" FORCE)
endif(NOT ANY_QT_DEVICE AND ENABLE_qt)

if(ENABLE_qt)
  # These two must be explicitly empty since other alternative means
  # are used.
  set(qt_COMPILE_FLAGS)
  set(qt_LINK_FLAGS)
endif(ENABLE_qt)

if(PLPLOT_USE_QT5)
  # Calculate what will be the AUTOMOC_MOC_OPTIONS property
  # for targets where the AUTOMOC property needs to be set.
  set(PLPLOT_AUTOMOC_MOC_OPTIONS)
  if(PLD_bmpqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_bmpqt)
  endif(PLD_bmpqt)
  if(PLD_jpgqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_jpgqt)
  endif(PLD_jpgqt)
  if(PLD_pngqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_pngqt)
  endif(PLD_pngqt)
  if(PLD_ppmqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_ppmqt)
  endif(PLD_ppmqt)
  if(PLD_tiffqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_tiffqt)
  endif(PLD_tiffqt)
  if(PLD_svgqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_svgqt)
  endif(PLD_svgqt)
  if(PLD_epsqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_epsqt)
  endif(PLD_epsqt)
  if(PLD_pdfqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_pdfqt)
  endif(PLD_pdfqt)
  if(PLD_qtwidget)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_qtwidget)
  endif(PLD_qtwidget)
  if(PLD_extqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_extqt)
  endif(PLD_extqt)
  if(PLD_memqt)
    list(APPEND PLPLOT_AUTOMOC_MOC_OPTIONS -DPLD_memqt)
  endif(PLD_memqt)
endif(PLPLOT_USE_QT5)


if(ENABLE_pyqt4 AND PLPLOT_USE_QT5)
  message(STATUS "WARNING: PLPLOT_USE_QT5 is ON so setting ENABLE_pyqt4 to OFF")
  set(ENABLE_pyqt4 OFF CACHE BOOL "Enable pyqt4 Python extension module " FORCE)
endif(ENABLE_pyqt4 AND PLPLOT_USE_QT5)

if(ENABLE_pyqt5 AND NOT PLPLOT_USE_QT5)
  message(STATUS "WARNING: PLPLOT_USE_QT5 is OFF so setting ENABLE_pyqt5 to OFF")
  set(ENABLE_pyqt5 OFF CACHE BOOL "Enable pyqt5 Python extension module " FORCE)
endif(ENABLE_pyqt5 AND NOT PLPLOT_USE_QT5)

if(ENABLE_pyqt4 AND NOT ENABLE_python)
  message(STATUS "WARNING: ENABLE_python is OFF so setting ENABLE_pyqt4 to OFF")
  set(ENABLE_pyqt4 OFF CACHE BOOL "Enable pyqt4 Python extension module " FORCE)
endif(ENABLE_pyqt4 AND NOT ENABLE_python)

if(ENABLE_pyqt5 AND NOT ENABLE_python)
  message(STATUS "WARNING: ENABLE_python is OFF so setting ENABLE_pyqt5 to OFF")
  set(ENABLE_pyqt5 OFF CACHE BOOL "Enable pyqt5 Python extension module " FORCE)
endif(ENABLE_pyqt5 AND NOT ENABLE_python)


if(ENABLE_pyqt4 AND NOT PLD_extqt)
  message(STATUS "WARNING: PLD_extqt is OFF so setting ENABLE_pyqt4 to OFF")
  set(ENABLE_pyqt4 OFF CACHE BOOL "Enable pyqt4 Python extension module " FORCE)
endif(ENABLE_pyqt4 AND NOT PLD_extqt)

if(ENABLE_pyqt5 AND NOT PLD_extqt)
  message(STATUS
    "WARNING: PLD_extqt is OFF so setting ENABLE_pyqt5 to OFF"
    )
  set(ENABLE_pyqt5 OFF CACHE BOOL "Enable pyqt5 Python extension module " FORCE)
endif(ENABLE_pyqt5 AND NOT PLD_extqt)


if(PLD_extqt)
  set(qt_gui_true "")
else(PLD_extqt)
  set(qt_gui_true "#")
endif(PLD_extqt)

if(ANY_QT_DEVICE)
  if(ENABLE_DYNDRIVERS)
    if(ENABLE_qt)
      set(qt_SOURCE)
      set(qt_TARGETS PLPLOT::plplotqt)
    else(ENABLE_qt)
      # if qt disabled, then must include full source and forget
      # qt_TARGETS for this dynamic device.
      set(qt_SOURCE ${CMAKE_SOURCE_DIR}/bindings/qt_gui/plqt.cpp)
    endif(ENABLE_qt)
  else(ENABLE_DYNDRIVERS)
    # N.B. no qt_TARGETS here since use appropriate source code (see below)
    # instead to break circular linking.
    list(APPEND DRIVERS_LINK_FLAGS ${qt_LINK_FLAGS})
    set(qt_SOURCE ${CMAKE_SOURCE_DIR}/bindings/qt_gui/plqt.cpp)
  endif(ENABLE_DYNDRIVERS)
endif(ANY_QT_DEVICE)

if(ENABLE_pyqt4 OR ENABLE_pyqt5)
  find_program(SIP_EXECUTABLE sip)
  message(STATUS "pyqt: SIP_EXECUTABLE = ${SIP_EXECUTABLE}")
  if(SIP_EXECUTABLE)
    execute_process(
      COMMAND ${SIP_EXECUTABLE} -V
      OUTPUT_VARIABLE SIP_VERSION
      RESULT_VARIABLE SIP_VERSION_ERR
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    if(SIP_VERSION_ERR)
      message(AUTHOR_WARNING "sip -V command could not determine sip version")
    else(SIP_VERSION_ERR)
      message(STATUS "SIP_VERSION = ${SIP_VERSION}")
    endif(SIP_VERSION_ERR)

    # Look in standard places for sip files.
    execute_process(
      COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print(sys.prefix)"
      OUTPUT_VARIABLE SYS_PREFIX
      RESULT_VARIABLE SYS_PREFIX_ERR
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    if(NOT SYS_PREFIX_ERR)
      # Find directory which contains QtCore/QtCoremod.sip.  This file
      # is imported by both bindings/qt_gui/pyqt4/plplot_pyqt4.sip and
      # bindings/qt_gui/pyqt5/plplot_pyqt5.sip.  If that directory
      # location is found then we also check below that the other sip
      # files imported by those files are also resident in this same
      # directory.  Use all combinations of sip_dir_PATHS and
      # sip_dir_PATH_SUFFIXES to find this location.

      set(sip_dir_PATHS)
      if(WIN32_AND_NOT_CYGWIN)
	# Here is the MinGW-w64/MSYS2 data concerning this location.
	# from the following results from Arjen's run of "pkgfile -v QtCoremod.sip".
	# mingw32/mingw-w64-i686-python2-pyqt4 4.11.4-2 /mingw32/share/sip/Py2-Qt4/QtCore/QtCoremod.sip
	# mingw32/mingw-w64-i686-python2-pyqt5 5.8-1 /mingw32/share/sip/Py2-Qt5/QtCore/QtCoremod.sip
	# mingw32/mingw-w64-i686-python3-pyqt4 4.11.4-2 /mingw32/share/sip/PyQt4/QtCore/QtCoremod.sip
	# mingw32/mingw-w64-i686-python3-pyqt5 5.8-1 /mingw32/share/sip/PyQt5/QtCore/QtCoremod.sip
	# mingw64/mingw-w64-x86_64-python2-pyqt4 4.11.4-2 /mingw64/share/sip/Py2-Qt4/QtCore/QtCoremod.sip
	# mingw64/mingw-w64-x86_64-python2-pyqt5 5.8-1 /mingw64/share/sip/Py2-Qt5/QtCore/QtCoremod.sip
	# mingw64/mingw-w64-x86_64-python3-pyqt4 4.11.4-2 /mingw64/share/sip/PyQt4/QtCore/QtCoremod.sip
	# mingw64/mingw-w64-x86_64-python3-pyqt5 5.8-1 /mingw64/share/sip/PyQt5/QtCore/QtCoremod.sip
        list(APPEND sip_dir_PATHS "${SYS_PREFIX}/share/sip")
	# MSVC, MinGW-w64, legacy MinGW, legacy MinGW/MSYS) which do not
	# distribute python/sip/pyqt themselves so users typically
	# download python/sip/pyqt as a generic binary
	# download suitable for "Windows" platforms.  (For one such download, the
	# sip directory location was c:\python25\sip so we assume
	# that generic form (other than the prefix) here.)
        list(APPEND sip_dir_PATHS "${SYS_PREFIX}/sip")
      else(WIN32_AND_NOT_CYGWIN)
        if(NOT ${PYTHON_VERSION} VERSION_LESS "3.0.0")
	  # Here is the Fedora 32 (and likely previous Fedora) python3/qt5 locations according to
	  # <http://rpm.pbone.net/index.php3/stat/6/idpl/70683647/dir/fedora_32/com/python3-qt5-devel-5.14.2-3.fc32.x86_64.rpm>
	  # I assume these two different locations are for backwards compatibility or compatibility
	  # with other Linux distros (see below) so I will search
	  # for both.
	  # Fedora 32: /usr/share/python3-sip/PyQt5/QtCore/QtCoremod.sip
	  # Fedora 32: /usr/share/sip/PyQt5/QtCore/QtCoremod.sip

	  # Here is the OpenSUSE Tumbleweed distro location from
	  # <http://rpm.pbone.net/index.php3/stat/6/idpl/54882581/dir/opensuse_tumbleweed/com/python-qt5-utils-5.12-2.3.x86_64.rpm>
	  # I note this location is the same as one of the Fedora alternatives so it is likely
	  # a good generic location for most Linux distributions other than Debian Buster and Sid.
	  # OpenSUSE Tumbleweed: /usr/share/sip/PyQt5/QtCore/QtCoremod.sip
          list(APPEND sip_dir_PATHS "${SYS_PREFIX}/share/python3-sip")
          list(APPEND sip_dir_PATHS "${SYS_PREFIX}/share/sip")
	  # Here is Debian Sid data (taken from <https://packages.debian.org/sid/all/pyqt5-dev/filelist>)
	  # concerning this python3 location.
	  # pyqt5-dev: /usr/lib/python3/dist-packages/PyQt5/bindings/QtCore/QtCoremod.sip
	  list(APPEND sip_dir_PATHS "${SYS_PREFIX}/lib/python3/dist-packages")
        endif(NOT ${PYTHON_VERSION} VERSION_LESS "3.0.0")

	# Here is Debian Buster data (from "apt-file search") concerning this location.
	# pyqt5-dev: /usr/share/sip/PyQt5/QtCore/QtCoremod.sip
	# python-qt4-dev: /usr/share/sip/PyQt4/QtCore/QtCoremod.sip
        # Also from looking at rpm.pbone results for non-Debian based distros, and
        # <https://cygwin.com/cgi-bin2/package-grep.cgi>, all Linux
        # distros other than Fedora/python3 Cygwin and Debian Sid install their
        # sip files in this location.
        list(APPEND sip_dir_PATHS "${SYS_PREFIX}/share/sip")

	# I have no QtCore/QtCoremod.sip location information for official Mac
	# OSX software; the Mac free software distros Homebrew,
	# MacPorts, or Fink; the *BSD free software distros, or
	# proprietary Unices.  So for those platforms we will have to
	# rely on feedback from our users to figure out the location
	# of QtCore/QtCoremod.sip if none of the variants we try
	# below work.

      endif(WIN32_AND_NOT_CYGWIN)
      # We determine PATHS corresponding to the data above, and then use
      # PATH_SUFFIXES to look for all possible subdirectories of those PATHS.
      # QtCore/QtCoremod.sip within several possible subdirectories

      # I have assumed below that the pyqt_module_name for PyQt4 or
      # PyQt5 always has the respective name of "PyQt4" or "PyQt5".
      if(ENABLE_pyqt4)
        set(pyqt_module_name PyQt4)
        set(sip_dir_PATH_SUFFIXES Py2-Qt4 PyQt4)
      elseif(ENABLE_pyqt5)
        set(pyqt_module_name PyQt5)
	# The PyQt5/bindings variant required by Debian Sid, see above
        set(sip_dir_PATH_SUFFIXES Py2-Qt5 PyQt5 PyQt5/bindings)
      else(ENABLE_pyqt4)
        # N.B. at this stage either ENABLE_pyqt4 or ENABLE_pyqt5 is
        # true but not both.
        message(AUTHOR_ERROR "Both ENABLE_pyqt4 and ENABLE_pyqt5 are true which should be impossible at this stage in the logic")
      endif(ENABLE_pyqt4)

      # Use sip_dir_PATHS and sip_dir_PATH_SUFFIXES to find the PATH of QtCore/QtCoremod.sip

      find_path(PYQT_SIP_DIR NAMES QtCore/QtCoremod.sip PATHS ${sip_dir_PATHS} PATH_SUFFIXES ${sip_dir_PATH_SUFFIXES})

      message(STATUS "PYQT_SIP_DIR = ${PYQT_SIP_DIR}")
      if(PYQT_SIP_DIR)
        if(EXISTS ${PYQT_SIP_DIR}/QtCore/QtCoremod.sip AND EXISTS ${PYQT_SIP_DIR}/QtGui/QtGuimod.sip AND (ENABLE_pyqt4 OR EXISTS ${PYQT_SIP_DIR}/QtWidgets/QtWidgetsmod.sip))
          execute_process(
            COMMAND ${PYTHON_EXECUTABLE} -c "from ${pyqt_module_name}.QtCore import PYQT_CONFIGURATION; print(PYQT_CONFIGURATION['sip_flags'])"
            OUTPUT_VARIABLE PYQT_SIP_FLAGS
            RESULT_VARIABLE PYQT_SIP_FLAGS_ERR
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
          if(NOT PYQT_SIP_FLAGS_ERR)
            # Must change from blank-delimited string to CMake list so that sip
            # COMMAND will work properly with these flags later on.
            string(REGEX REPLACE " " ";" PYQT_SIP_FLAGS "${PYQT_SIP_FLAGS}")
            message(STATUS "${pyqt_module_name}: PYQT_SIP_FLAGS = ${PYQT_SIP_FLAGS}")
          else(NOT PYQT_SIP_FLAGS_ERR)
            message(STATUS
              "WARNING: could not find sip flags so setting ENABLE_pyqt4 / ENABLE_pyqt5 to OFF"
              )
            set(ENABLE_pyqt4 OFF CACHE BOOL "Enable pyqt4 Python extension module " FORCE)
            set(ENABLE_pyqt5 OFF CACHE BOOL "Enable pyqt5 Python extension module " FORCE)
          endif(NOT PYQT_SIP_FLAGS_ERR)
        else(EXISTS ${PYQT_SIP_DIR}/QtCore/QtCoremod.sip AND EXISTS ${PYQT_SIP_DIR}/QtGui/QtGuimod.sip AND (ENABLE_pyqt4 OR EXISTS ${PYQT_SIP_DIR}/QtWidgets/QtWidgetsmod.sip))
          if(ENABLE_pyqt4)
            message(STATUS "WARNING: One of QtCore/QtCoremod.sip or QtGui/QtGuimod.sip does not exist in the PYQT_SIP_DIR = ${PYQT_SIP_DIR} directory location. Therefore setting ENABLE_pyqt4 to OFF.  The issue might be you need to install a system package containing these files.  But if they are installed on your platform, then work around the PLplot build system not being able to find these files by specifying the correct PYQT_SIP_DIR directory corresponding to their location.  And also report this issue to the plplot-general mailing list to allow us to fix our build system")
            set(ENABLE_pyqt4 OFF CACHE BOOL "Enable pyqt4 Python extension module " FORCE)
          elseif(ENABLE_pyqt5)
            message(STATUS "WARNING: One of QtCore/QtCoremod.sip, QtGui/QtGuimod.sip, or QtWidgets/QtWidgetsmod.sip does not exist in the PYQT_SIP_DIR = ${PYQT_SIP_DIR} directory location.  Therefore setting ENABLE_pyqt5 to OFF.  The issue might be you need to install a system package containing these files.  But if they are installed on your platform, then work around the PLplot build system not being able to find these files by specifying the correct PYQT_SIP_DIR directory corresponding to their location.  And also report this issue to the plplot-general mailing list to allow us to fix our build system")
            set(ENABLE_pyqt5 OFF CACHE BOOL "Enable pyqt5 Python extension module " FORCE)
          endif(ENABLE_pyqt4)
        endif(EXISTS ${PYQT_SIP_DIR}/QtCore/QtCoremod.sip AND EXISTS ${PYQT_SIP_DIR}/QtGui/QtGuimod.sip AND (ENABLE_pyqt4 OR EXISTS ${PYQT_SIP_DIR}/QtWidgets/QtWidgetsmod.sip))
      else(PYQT_SIP_DIR)
        if(ENABLE_pyqt4)
          message(STATUS "WARNING: QtCore/QtCoremod.sip not found in any combination of the directories in sip_dir_PATHS = ${sip_dir_PATHS} and the subdirectories in sip_dir_PATH_SUFFIXES = ${sip_dir_PATH_SUFFIXES}.  Therefore setting  ENABLE_pyqt4 to OFF.  The issue might be you need to install the system package containing QtCore/QtCoremod.sip.  But if that file is already installed on your platform, then work around the PLplot build system not being able to find the path location of that file by specifying the correct PYQT_SIP_DIR directory corresponding to their location.  And also report this issue to the plplot-general mailing list to allow us to fix our build system")
          set(ENABLE_pyqt4 OFF CACHE BOOL "Enable pyqt4 Python extension module " FORCE)
        elseif(ENABLE_pyqt5)
          message(STATUS "WARNING: QtCore/QtCoremod.sip not found in any combination of the directories in sip_dir_PATHS = ${sip_dir_PATHS} and the subdirectories in sip_dir_PATH_SUFFIXES = ${sip_dir_PATH_SUFFIXES}.  Therefore setting  ENABLE_pyqt5 to OFF.  The issue might be you need to install the system package containing QtCore/QtCoremod.sip.  But if that file is already installed on your platform, then work around the PLplot build system not being able to find the path location of that file by specifying the correct PYQT_SIP_DIR directory corresponding to their location.  And also report this issue to the plplot-general mailing list to allow us to fix our build system")
          set(ENABLE_pyqt5 OFF CACHE BOOL "Enable pyqt5 Python extension module " FORCE)
        endif(ENABLE_pyqt4)
      endif(PYQT_SIP_DIR)
    else(NOT SYS_PREFIX_ERR)
      message(STATUS "WARNING: sys.prefix not found so setting ENABLE_pyqt4 / ENABLE_pyqt5 to OFF")
      set(ENABLE_pyqt4 OFF CACHE BOOL "Enable pyqt4 Python extension module " FORCE)
      set(ENABLE_pyqt5 OFF CACHE BOOL "Enable pyqt5 Python extension module " FORCE)
    endif(NOT SYS_PREFIX_ERR)
  else(SIP_EXECUTABLE)
    message(STATUS "WARNING: sip not found so setting ENABLE_pyqt4 / ENABLE_pyqt5 to OFF")
    set(ENABLE_pyqt4 OFF CACHE BOOL "Enable pyqt4 Python extension module " FORCE)
    set(ENABLE_pyqt5 OFF CACHE BOOL "Enable pyqt5 Python extension module " FORCE)
  endif(SIP_EXECUTABLE)
endif(ENABLE_pyqt4 OR ENABLE_pyqt5)

if(ENABLE_smoke)
  if(PLPLOT_USE_QT5)
    # The smoke-qt5 possibility is discussed extensively at
    # <https://github.com/ryanmelt/qtbindings/issues/131> and as of
    # 2015-11 there didn't seem to be much chance of a Qt5
    # version of smoke being developed any time soon (if ever).
    message(STATUS "WARNING: smoke only works with Qt4 so setting ENABLE_smoke to OFF")
    set(ENABLE_smoke OFF CACHE BOOL "Enable smoke Qt4 extension module " FORCE)
  else(PLPLOT_USE_QT5)
    find_package(Smoke QUIET COMPONENTS QtCore QtGui)
    message(STATUS "SMOKE_QTCORE_FOUND = ${SMOKE_QTCORE_FOUND}")
    message(STATUS "SMOKE_QTGUI_FOUND = ${SMOKE_QTGUI_FOUND}")
    message(STATUS "SMOKE_GEN_BIN = ${SMOKE_GEN_BIN}")
    message(STATUS "SMOKE_GEN_SHARED = ${SMOKE_GEN_SHARED}")
    message(STATUS "SMOKE_QTCORE_LIBRARY = ${SMOKE_QTCORE_LIBRARY}")
    message(STATUS "SMOKE_QTGUI_LIBRARY = ${SMOKE_QTGUI_LIBRARY}")
    if(SMOKE_QTCORE_FOUND AND SMOKE_QTGUI_FOUND)
      message(STATUS "smoke installation found")
    else(SMOKE_QTCORE_FOUND AND SMOKE_QTGUI_FOUND)
      message(STATUS "WARNING: smoke installation not found so setting ENABLE_smoke to OFF")
      set(ENABLE_smoke OFF CACHE BOOL "Enable smoke Qt4 extension module " FORCE)
    endif(SMOKE_QTCORE_FOUND AND SMOKE_QTGUI_FOUND)
  endif(PLPLOT_USE_QT5)
endif(ENABLE_smoke)
