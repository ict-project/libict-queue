################################################################
cmake_minimum_required(VERSION 3.5)
################################################################
set(LIBRARY_NAME "queue")
set(LIBRARY_DESC "Library with queue service.")

project(libict-${LIBRARY_NAME})

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_LINK_LIBS "")

macro(add_ict_subdir ICT_NAME)
  set(CMAKE_SUBDIRECTORY      ${ICT_NAME})
  set(CMAKE_SUBDIRECTORY_DIR  ../${CMAKE_SUBDIRECTORY})
  set(CMAKE_SUBDIRECTORY_SRC  ${CMAKE_SUBDIRECTORY_DIR}/source)
  set(CMAKE_SUBDIRECTORY_BLD  ${CMAKE_SUBDIRECTORY_DIR}/build)
  set(CMAKE_SUBDIRECTORY_LIB  ${ICT_NAME}.so)

  add_subdirectory(${CMAKE_SUBDIRECTORY_SRC} ${CMAKE_SUBDIRECTORY_BLD} EXCLUDE_FROM_ALL)

  add_custom_target(
    ${CMAKE_SUBDIRECTORY} ALL
    COMMAND make
    WORKING_DIRECTORY ${CMAKE_SUBDIRECTORY_DIR}
  )

  link_directories(${CMAKE_SUBDIRECTORY_BLD})
  include_directories(${CMAKE_SUBDIRECTORY_DIR})

  set(CMAKE_LINK_LIBS "${CMAKE_LINK_LIBS} ${CMAKE_SUBDIRECTORY_LIB}")
  string(STRIP ${CMAKE_LINK_LIBS} CMAKE_LINK_LIBS)
endmacro()

find_package(Threads)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/info.hpp 
  PRE_BUILD COMMAND ./info.sh ${LIBRARY_NAME} info.hpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/info.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/info.cpp 
  PRE_BUILD COMMAND ./info.sh ${LIBRARY_NAME} info.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/info.hpp ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/all.hpp 
  PRE_BUILD COMMAND ./all.sh 
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/all.sh ${CMAKE_CURRENT_SOURCE_DIR}/info.hpp 
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(CMAKE_SOURCE_FILES 
  info.cpp
  file-pool.cpp
  file-interface.cpp
  basic.cpp
  single.cpp
  dir-lock.cpp
  dir-pool.cpp
  dir-singleton.cpp
  pool.cpp
  prioritized.cpp
)

add_library(ict-static-${LIBRARY_NAME} STATIC ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-static-${LIBRARY_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ict-static-${LIBRARY_NAME} ${CMAKE_LINK_LIBS})
set_target_properties(ict-static-${LIBRARY_NAME}  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})

add_library(ict-shared-${LIBRARY_NAME} SHARED ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-shared-${LIBRARY_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ict-shared-${LIBRARY_NAME} ${CMAKE_LINK_LIBS})
set_target_properties(ict-shared-${LIBRARY_NAME}  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})

add_executable(${PROJECT_NAME}-test all.hpp test.cpp ${CMAKE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME}-test ${CMAKE_LINK_LIBS})
target_compile_definitions(${PROJECT_NAME}-test PUBLIC -DENABLE_TESTING)

add_executable(${PROJECT_NAME}-single-push single-push.cpp ${CMAKE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-single-push ${CMAKE_THREAD_LIBS_INIT})

add_executable(${PROJECT_NAME}-single-pop single-pop.cpp ${CMAKE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-single-pop ${CMAKE_THREAD_LIBS_INIT})

add_executable(${PROJECT_NAME}-single-size single-size.cpp ${CMAKE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-single-size ${CMAKE_THREAD_LIBS_INIT})

add_executable(${PROJECT_NAME}-single-empty single-empty.cpp ${CMAKE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-single-empty ${CMAKE_THREAD_LIBS_INIT})

add_executable(${PROJECT_NAME}-single-clear single-clear.cpp ${CMAKE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-single-clear ${CMAKE_THREAD_LIBS_INIT})

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

file(GLOB HEADER_LIST . *.hpp)

install(TARGETS ict-static-${LIBRARY_NAME} ict-shared-${LIBRARY_NAME} DESTINATION lib COMPONENT libraries)
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
add_test(NAME ict-single-tc4 COMMAND ${PROJECT_NAME}-test ict single tc4)
add_test(NAME ict-dir_lock-tc1 COMMAND ${PROJECT_NAME}-test ict dir_lock tc1)
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

add_test(NAME single-test-bash COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/single-test.sh ./${PROJECT_NAME}-single-push ./${PROJECT_NAME}-single-pop)

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