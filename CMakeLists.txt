#
# s3fs-fuse-awscred-lib ( s3fs-fuse credential I/F library for AWS )
#
#     Copyright 2022 Takeshi Nakatani <ggtakec@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.13.4)

#
# specify the C++ standard
#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#
# Create version information
#
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/make_version_files.sh commit_hash)
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/make_version_files.sh product_version)
file(READ commit_hash COMMIT_HASH)
file(READ product_version PRODUCT_VERSION)

#
# Set project information
#
project(s3fsawscred LANGUAGES CXX)
project(s3fsawscred VERSION ${PRODUCT_VERSION})

#
# Create config.h
#
configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

#
# Set Library information
#
set(LIB_NAME   "s3fsawscred")
set(LIB_SRC    "awscred.cpp" "awscred_func.cpp")
set(LIB_HEADER "awscred.h" "awscred_func.h" "config.h")
set(LIB_SAMPLE "awscred_test.cpp")
set(LIB_TYPE   "SHARED")

#
# AWS libraries
#
find_package(AWSSDK REQUIRED COMPONENTS core identity-management)

#
# For building Library
#
add_library(${LIB_NAME} ${LIB_TYPE} ${LIB_SRC})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${LIB_NAME} ${AWSSDK_LINK_LIBRARIES})

#
# For building test file.
#
link_directories(${INSTALL_DIR}/lib)
add_executable("${LIB_NAME}_test" ${LIB_SAMPLE})
target_include_directories("${LIB_NAME}_test" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${INSTALL_DIR}/include)
target_link_libraries("${LIB_NAME}_test" ${LIB_NAME} ${AWSSDK_LINK_LIBRARIES})

#
# Specify Install Folder
#
install(TARGETS "${LIB_NAME}" LIBRARY DESTINATION "${INSTALL_DIR}/lib")

#
# Flags
#
set_compiler_flags(${PROJECT_NAME})
set_compiler_warnings(${PROJECT_NAME})
set_compiler_flags(${LIB_NAME})
set_compiler_warnings(${LIB_NAME})

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim600: noexpandtab sw=4 ts=4 fdm=marker
# vim<600: noexpandtab sw=4 ts=4
#
