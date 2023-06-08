cmake_minimum_required(VERSION 3.12)

project(shipovnik LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(GOST_OPTIMIZATION CACHE STRING "Set GOST optimization level")
set_property(CACHE GOST_OPTIMIZATION PROPERTY STRINGS "0" "1" "2" "3")
if(NOT GOST_OPTIMIZATION)
  set(GOST_OPTIMIZATION "0")
endif()

set(ENTROPY_SOURCE CACHE STRING "Set entropy source")
if(NOT ENTROPY_SOURCE)
  set(ENTROPY_SOURCE "/dev/urandom")
endif()

add_subdirectory(streebog)

file(GLOB SOURCES "src/*.c")
file(GLOB PUBLIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/shipovnik/*.h")

add_library(shipovnik ${SOURCES})
set_target_properties(shipovnik PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
target_compile_definitions(shipovnik PRIVATE ENTROPY_SOURCE="${ENTROPY_SOURCE}")
target_include_directories(shipovnik PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/shipovnik>  
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/shipovnik>
)
target_link_libraries(shipovnik PRIVATE streebog)

add_executable(shipovnik_example shipovnik_example.c)
target_link_libraries(shipovnik_example PRIVATE shipovnik)

configure_package_config_file(
  shipovnikConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/shipovnikConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/shipovnik/cmake"
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/shipovnikConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/shipovnik/cmake"
)

install(
  TARGETS shipovnik streebog
  EXPORT shipovnikTargets 
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/shipovnik"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/shipovnik"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/shipovnik"
)

install(
  EXPORT shipovnikTargets 
  FILE shipovnikTargets.cmake
  DESTINATION "${CMAKE_INSTALL_DATADIR}/shipovnik/cmake"
)
