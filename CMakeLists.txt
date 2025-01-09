#============================================================================
#
# Copyright (C) 2025 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
#
# This file is part of the Path-to-Regex which can be found at
# https://github.com/IvanPinezhaninov/path_to_regex/.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
# THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#============================================================================

cmake_minimum_required(VERSION 3.16)

project(path_to_regex LANGUAGES CXX VERSION 1.0.0)

option(PATH_TO_REGEX_BUILD_EXAMPLE "Build example" OFF)
option(PATH_TO_REGEX_BUILD_TESTS "Build tests" OFF)
option(PATH_TO_REGEX_CODECOV "Add test coverage" OFF)

if(PATH_TO_REGEX_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(PATH_TO_REGEX_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()

set(HEADERS
  include/path_to_regex.hpp
)

add_library(${PROJECT_NAME} INTERFACE
  ${HEADERS}
)

add_library("path_to_regex::path_to_regex" ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE
  include
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

if(PATH_TO_REGEX_CODECOV AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${PROJECT_NAME} INTERFACE -O0 -g --coverage)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(${PROJECT_NAME} INTERFACE --coverage)
  else()
    target_link_libraries(${PROJECT_NAME} INTERFACE --coverage)
  endif()
endif()
