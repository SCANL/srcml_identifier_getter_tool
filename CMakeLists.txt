##
#  CMakeLists.txt
#
#  Copyright (C) 2014 SDML (www.sdml.info)
#
#  This file is part of the srcml_identifier_getter project.
#
#  The srcml_identifier_getter is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  The srcml_identifier_getter is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the srcml_identifier_getter; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
project(srcml_identifier_getter)

# find needed libraries
find_package(LibXml2 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wno-reorder -Wunused-variable -Wunused-parameter")

# set output directory for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# include needed includes
include_directories(/usr/local/include
                    ${LIBXML2_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/srcSAXEventDispatch/srcSAX/src/srcsax
                    ${CMAKE_CURRENT_SOURCE_DIR}/srcSAXEventDispatch/srcSAX/src/cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/srcSAXEventDispatch/srcSAX/src/windows
                    ${CMAKE_CURRENT_SOURCE_DIR}/srcSAXEventDispatch/src/dispatcher
                    ${CMAKE_CURRENT_SOURCE_DIR}/srcSAXEventDispatch/src/policy_classes
                    popl/include
                    src)

add_subdirectory(srcSAXEventDispatch)
add_subdirectory(src)
