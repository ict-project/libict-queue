################################################################
cmake_minimum_required(VERSION 3.5)
################################################################
project(libict-queue)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_custom_command(OUTPUT all.hpp PRE_BUILD COMMAND ./all.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_SOURCE_FILES 
  file-pool.cpp
  file-interface.cpp
  basic.cpp
  single.cpp
  dir-pool.cpp
  pool.cpp
)

add_library(ict-static STATIC ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-static ${CMAKE_THREAD_LIBS_INIT} "stdc++fs")
set_target_properties(ict-static  PROPERTIES OUTPUT_NAME ict-queue)

add_library(ict-shared SHARED ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-shared ${CMAKE_THREAD_LIBS_INIT} "stdc++fs")
set_target_properties(ict-shared  PROPERTIES OUTPUT_NAME ict-queue)

add_executable(libict-queue-test all.hpp test.cpp ${CMAKE_SOURCE_FILES})
target_link_libraries(libict-queue-test ${CMAKE_THREAD_LIBS_INIT} "stdc++fs")
target_compile_definitions(libict-queue-test PUBLIC -DENABLE_TESTING)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)
  find_package(Git)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} describe
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "GIT_VERSION"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    message( STATUS "Git version: ${GIT_VERSION}" )
  else(GIT_FOUND)
    set(GIT_VERSION unknown)
  endif(GIT_FOUND)
else(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)
 set(GIT_VERSION unknown)
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/git_version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/git_version.h @ONLY)

file(GLOB HEADER_LIST . *.hpp)

install(TARGETS ict-static ict-shared DESTINATION lib COMPONENT libraries)
install(
  FILES ${HEADER_LIST}
  DESTINATION include/libict COMPONENT headers
)
################################################################
enable_testing()

add_test(NAME ict-all COMMAND libict-queue-test)

################################################################
#set(CPACK_GENERATOR "TZ;TGZ;STGZ;DEB;RPM") # Linux
#set(CPACK_GENERATOR "TZ;TGZ;STGZ")         # MacOS

set(CPACK_PACKAGE_NAME "libict-queue")
set(CPACK_PACKAGE_VENDOR "ict-project.pl")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mariusz Ornowski <mariusz.ornowski@ict-project.pl>")
set(CPACK_RPM_PACKAGE_LICENSE "BSD 3-clause")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "libict-queue - C++14 Library with queue service.")
set(CPACK_PACKAGE_VERSION ${GIT_VERSION})

set(CPACK_COMPONENTS_ALL libraries headers)

set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Libraries for queue service.")

set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C++ Headers")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION  "C++ Headers for queue service.")
set(CPACK_COMPONENT_HEADERS_DEPENDS libraries)

include(CPack)
################################################################