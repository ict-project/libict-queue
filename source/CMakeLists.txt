################################################################
cmake_minimum_required(VERSION 3.5)
################################################################
set(LIBRARY_NAME "queue")
set(LIBRARY_DESC "Library with queue service.")

project(libict-${LIBRARY_NAME})

set(CMAKE_CXX_STANDARD 17)

find_package(Threads)

add_custom_command(OUTPUT all.hpp PRE_BUILD COMMAND ./all.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_SOURCE_FILES 
  file-pool.cpp
  file-interface.cpp
  basic.cpp
  single.cpp
  dir-pool.cpp
  pool.cpp
  prioritized.cpp
)

add_library(ict-static STATIC ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-static ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(ict-static  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})

add_library(ict-shared SHARED ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-shared ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(ict-shared  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})

add_executable(${PROJECT_NAME}-test all.hpp test.cpp ${CMAKE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-test ${CMAKE_THREAD_LIBS_INIT})
target_compile_definitions(${PROJECT_NAME}-test PUBLIC -DENABLE_TESTING)

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
  DESTINATION include/libict/${LIBRARY_NAME} COMPONENT headers
)
################################################################
enable_testing()

add_test(NAME ict-filepool-tc1 COMMAND ${PROJECT_NAME}-test ict filepool tc1)
add_test(NAME ict-filepool-tc2 COMMAND ${PROJECT_NAME}-test ict filepool tc2)
add_test(NAME ict-filepool-tc3 COMMAND ${PROJECT_NAME}-test ict filepool tc3)
add_test(NAME ict-filepool-tc4 COMMAND ${PROJECT_NAME}-test ict filepool tc4)
add_test(NAME ict-filepool-tc5 COMMAND ${PROJECT_NAME}-test ict filepool tc5)
add_test(NAME ict-filepool-tc6 COMMAND ${PROJECT_NAME}-test ict filepool tc6)
add_test(NAME ict-fileinterface-tc1 COMMAND ${PROJECT_NAME}-test ict fileinterface tc1)
add_test(NAME ict-basic-tc1 COMMAND ${PROJECT_NAME}-test ict basic tc1)
add_test(NAME ict-basic-tc2 COMMAND ${PROJECT_NAME}-test ict basic tc2)
add_test(NAME ict-basic-tc3 COMMAND ${PROJECT_NAME}-test ict basic tc3)
add_test(NAME ict-basic-tc4 COMMAND ${PROJECT_NAME}-test ict basic tc4)
add_test(NAME ict-basic-tc5 COMMAND ${PROJECT_NAME}-test ict basic tc5)
add_test(NAME ict-single-tc1 COMMAND ${PROJECT_NAME}-test ict single tc1)
add_test(NAME ict-single-tc2 COMMAND ${PROJECT_NAME}-test ict single tc2)
add_test(NAME ict-single-tc3 COMMAND ${PROJECT_NAME}-test ict single tc3)
add_test(NAME ict-dirpool-tc1 COMMAND ${PROJECT_NAME}-test ict dirpool tc1)
add_test(NAME ict-dirpool-tc2 COMMAND ${PROJECT_NAME}-test ict dirpool tc2)
add_test(NAME ict-dirpool-tc3 COMMAND ${PROJECT_NAME}-test ict dirpool tc3)
add_test(NAME ict-dirpool-tc4 COMMAND ${PROJECT_NAME}-test ict dirpool tc4)
add_test(NAME ict-pool-tc1 COMMAND ${PROJECT_NAME}-test ict pool tc1)
add_test(NAME ict-prioritized-tc1 COMMAND ${PROJECT_NAME}-test ict prioritized tc1)
add_test(NAME ict-prioritized-tc2 COMMAND ${PROJECT_NAME}-test ict prioritized tc2)
add_test(NAME ict-prioritized-tc3 COMMAND ${PROJECT_NAME}-test ict prioritized tc3)
add_test(NAME ict-prioritized-tc4 COMMAND ${PROJECT_NAME}-test ict prioritized tc4)
add_test(NAME ict-prioritized-tc5 COMMAND ${PROJECT_NAME}-test ict prioritized tc5)

################################################################
#set(CPACK_GENERATOR "TZ;TGZ;STGZ;DEB;RPM") # Linux
#set(CPACK_GENERATOR "TZ;TGZ;STGZ")         # MacOS

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "ict-project.pl")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mariusz Ornowski <mariusz.ornowski@ict-project.pl>")
set(CPACK_RPM_PACKAGE_LICENSE "BSD 3-clause")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - ${LIBRARY_DESC}")
set(CPACK_PACKAGE_VERSION ${GIT_VERSION})

set(CPACK_COMPONENTS_ALL libraries headers)

set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Lib files for: ${LIBRARY_DESC}")

set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "Headers")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION  "Header files for: ${LIBRARY_DESC}")
set(CPACK_COMPONENT_HEADERS_DEPENDS libraries)

include(CPack)
################################################################