################################################################
cmake_minimum_required(VERSION 3.25)
################################################################
set(LIBRARY_NAME "queue")
set(LIBRARY_DESC "Library with queue service.")

project(libict-${LIBRARY_NAME})

set(CMAKE_CXX_STANDARD 17)
find_package(Threads)

include(../libict-dev-tools/libs-include.cmake)
include(../libict-dev-tools/info-include.cmake)
include(../libict-dev-tools/test-include.cmake)
include(../libict-dev-tools/git-include.cmake)

file(GLOB CMAKE_HEADER_LIST . *.hpp)

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
set_target_properties(ict-static-${LIBRARY_NAME}  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})

add_library(ict-shared-${LIBRARY_NAME} SHARED ${CMAKE_SOURCE_FILES})
target_link_libraries(ict-shared-${LIBRARY_NAME} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(ict-shared-${LIBRARY_NAME}  PROPERTIES OUTPUT_NAME ict-${LIBRARY_NAME})

add_executable(${PROJECT_NAME}-test ${CMAKE_HEADER_LIST} test.cpp)
target_link_libraries(${PROJECT_NAME}-test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME}-test ict-static-${LIBRARY_NAME})
target_link_libraries(${PROJECT_NAME}-test ${CMAKE_LINK_LIBS})
target_compile_definitions(${PROJECT_NAME}-test PUBLIC -DENABLE_TESTING)

add_executable(${PROJECT_NAME}-single-push single-push.cpp)
target_link_libraries(${PROJECT_NAME}-single-push ict-static-${LIBRARY_NAME})
target_link_libraries(${PROJECT_NAME}-single-push ${CMAKE_THREAD_LIBS_INIT})

add_executable(${PROJECT_NAME}-single-pop single-pop.cpp)
target_link_libraries(${PROJECT_NAME}-single-pop ict-static-${LIBRARY_NAME})
target_link_libraries(${PROJECT_NAME}-single-pop ${CMAKE_THREAD_LIBS_INIT})

add_executable(${PROJECT_NAME}-single-size single-size.cpp)
target_link_libraries(${PROJECT_NAME}-single-size ict-static-${LIBRARY_NAME})
target_link_libraries(${PROJECT_NAME}-single-size ${CMAKE_THREAD_LIBS_INIT})

add_executable(${PROJECT_NAME}-single-empty single-empty.cpp)
target_link_libraries(${PROJECT_NAME}-single-empty ict-static-${LIBRARY_NAME})
target_link_libraries(${PROJECT_NAME}-single-empty ${CMAKE_THREAD_LIBS_INIT})

add_executable(${PROJECT_NAME}-single-clear single-clear.cpp)
target_link_libraries(${PROJECT_NAME}-single-clear ict-static-${LIBRARY_NAME})
target_link_libraries(${PROJECT_NAME}-single-clear ${CMAKE_THREAD_LIBS_INIT})

################################################################

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
include(../libict-dev-tools/cpack-include.cmake)
################################################################