#
# Find the ARGoS package
#
if(ARGOS_BUILD_FOR_SIMULATOR)
  find_package(PkgConfig)
  pkg_check_modules(ARGOS REQUIRED argos3_simulator)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)
  include_directories(${ARGOS_INCLUDE_DIRS})
  link_directories(${ARGOS_LIBRARY_DIRS})
elseif(ARGOS_BUILD_FOR_LOCALEPUCK)
  find_package(PkgConfig)
  pkg_check_modules(ARGOS REQUIRED argos3_localepuck)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)
  include_directories(${ARGOS_INCLUDE_DIRS})
  link_directories(${ARGOS_LIBRARY_DIRS})
endif()

IF(NOT ARGOS_BUILD_FOR_EPUCK)
  find_package(Lua53)
  if(LUA53_FOUND)
    include_directories(${LUA_INCLUDE_DIR})
  endif(LUA53_FOUND)
endif (NOT ARGOS_BUILD_FOR_EPUCK)

#
# Check for Qt and OpenGL when compiling for the simulator
#
if(ARGOS_BUILD_FOR_SIMULATOR)
  include(ARGoSCheckQTOpenGL)
endif(ARGOS_BUILD_FOR_SIMULATOR)
