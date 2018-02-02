##---------------------------------------------------------------------------
## Author:      Pavel Kalian
##***************************************************************************
## *   Copyright (C) 2013 by Pavel Kalian                                    *
## *   This program is free software; you can redistribute it and/or modify  *
## *   it under the terms of the GNU General Public License as published by  *
## *   the Free Software Foundation; either version 2 of the License, or     *
## *   (at your option) any later version.                                   *
## *                                                                         *
## *   This program is distributed in the hope that it will be useful,       *
## *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
## *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
## *   GNU General Public License for more details.                          *
## *                                                                         *
## *   You should have received a copy of the GNU General Public License     *
## *   along with this program; if not, write to the                         *
## *   Free Software Foundation, Inc.,                                       *
## *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
## ***************************************************************************

# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(objsearch_pi)

SET(PACKAGE_NAME objsearch_pi)
SET(VERBOSE_NAME ObjectSearch)
SET(TITLE_NAME ObjectSearch)
SET(CPACK_PACKAGE_CONTACT "Pavel Kalian")

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "9")

#SET(CMAKE_BUILD_TYPE Debug)

OPTION(OBJSEARCH_USE_SVG "Use SVG graphics" ON)

INCLUDE("cmake/PluginConfigure.cmake")

IF(OBJSEARCH_USE_SVG)
  ADD_DEFINITIONS(-DOBJSEARCH_USE_SVG)
ENDIF(OBJSEARCH_USE_SVG)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

SET(SRC_OBJSEARCH
            src/objsearch_pi.h
            src/objsearch_pi.cpp
            src/icons.h
            src/icons.cpp
            src/ObjSearchDialog.h
            src/ObjSearchDialog.cpp
            src/clcpopup.h
            src/clcpopup.cpp
            src/csv_parser.h
            src/csv_parser.cpp
            wxsqlite3-3.5.9/src/wxsqlite3.cpp
            sqlite-amalgamation-3220000/sqlite3.c
)
INCLUDE_DIRECTORIES(wxsqlite3-3.5.9/include sqlite-amalgamation-3220000)
ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_OBJSEARCH})

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
