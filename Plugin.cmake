# ~~~
# Summary:      Local, non-generic plugin setup
# Copyright (c) 2020-2021 Mike Rossiter
# License:      GPLv3+
# ~~~

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.


# -------- Options ----------

set(OCPN_TEST_REPO
  "nohal/opencpn-plugins"
  CACHE STRING "Default repository for untagged builds"
)
set(OCPN_BETA_REPO
  "nohal/objsearch_pi-beta"
  CACHE STRING
  "Default repository for tagged builds matching 'beta'"
)
set(OCPN_RELEASE_REPO
  "nohal/objsearch_pi-stable"
  CACHE STRING
  "Default repository for tagged builds not matching 'beta'"
)

#
#
# -------  Plugin setup --------
#
set(PKG_NAME objsearch_pi)
set(PKG_VERSION "0.22.0")
set(PKG_PRERELEASE "beta")  # Empty, or a tag like 'beta'

set(DISPLAY_NAME objsearch)    # Dialogs, installer artifacts, ...
set(PLUGIN_API_NAME objsearch_pi) # As of GetCommonName() in plugin API
set(PKG_SUMMARY "Chart object search plugin for OpenCPN")
set(PKG_DESCRIPTION [=[
Vector chart object search  plugin for OpenCPN
]=])

set(PKG_AUTHOR "Pavel Kalian")
set(PKG_IS_OPEN_SOURCE "yes")
set(PKG_HOMEPAGE https://github.com/nohal/objsearch_pi)
set(PKG_INFO_URL https://opencpn.org/OpenCPN/plugins/objsearch.html)

option(WITH_TESTS "Whether or not to build the tests" OFF)
option(SANITIZE "What sanitizers to use" "")

if(NOT "${SANITIZE}" STREQUAL "OFF" AND NOT "${SANITIZE}" STREQUAL "")
  add_compile_options(-fsanitize=${SANITIZE} -fno-omit-frame-pointer)
  add_link_options(-fsanitize=${SANITIZE} -fno-omit-frame-pointer)
endif()

add_definitions(-DOBJSEARCH_USE_SVG)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/wxsqlite3-3.5.9/include)
include_directories(${CMAKE_SOURCE_DIR}/sqlite-amalgamation-3260000)

set(HDR_OBJSEARCH
  ${CMAKE_SOURCE_DIR}/include/clcpopup.h
  ${CMAKE_SOURCE_DIR}/include/csv_parser.h
  ${CMAKE_SOURCE_DIR}/include/icons.h
  ${CMAKE_SOURCE_DIR}/include/ObjSearchDialog.h
  ${CMAKE_SOURCE_DIR}/include/objsearch_pi.h
  )
set(SRC_OBJSEARCH
  ${CMAKE_SOURCE_DIR}/src/clcpopup.cpp
  ${CMAKE_SOURCE_DIR}/src/csv_parser.cpp
  ${CMAKE_SOURCE_DIR}/src/icons.cpp
  ${CMAKE_SOURCE_DIR}/src/ObjSearchDialog.cpp
  ${CMAKE_SOURCE_DIR}/src/objsearch_pi.cpp
  )

set(SRC
  ${HDR_OBJSEARCH}
  ${SRC_OBJSEARCH}
  ${CMAKE_SOURCE_DIR}/sqlite-amalgamation-3260000/sqlite3.c
  ${CMAKE_SOURCE_DIR}/wxsqlite3-3.5.9/src/wxsqlite3.cpp
)

set(PKG_API_LIB api-16)  #  A dir in opencpn-libs/ e. g., api-17 or api-16

macro(late_init)
  # Perform initialization after the PACKAGE_NAME library, compilers
  # and ocpn::api is available.
endmacro()

macro(add_plugin_libraries)
  # Add libraries required by this plugin
  if(WIN32)
    add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/WindowsHeaders")
    target_link_libraries(${PACKAGE_NAME} windows::headers)
  endif()

endmacro()
