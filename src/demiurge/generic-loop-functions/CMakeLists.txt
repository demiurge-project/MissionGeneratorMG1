

set(ARENA_HEADERS
	Arenas/BaseArena.h
	Arenas/TetragonArena.h
	Arenas/TrigonArena.h
  Arenas/HexagonArena.h)

set(OBJECT_HEADERS
  Arenas/Objects/Box.h
  Arenas/Objects/Light.h
  Arenas/Objects/Patch.h
)

set(MISSION_HEADERS
  Missions/GenericLoopFunctions.h
  Missions/ForagingMission.h
  Missions/HomingMission.h
  Missions/AggregationXORMission.h
)

set(GENERATOR_HEADERS
  Generator/ConfigurationParser.h)

# Sources
set(MG_SOURCES
  ${ARENA_HEADERS}
  ${OBJECT_HEADERS}
  ${MISSION_HEADERS}
  ${GENERATOR_HEADERS}
  Generator/ConfigurationParser.cpp
  # Arenas
  Arenas/BaseArena.cpp
  Arenas/TetragonArena.cpp
  Arenas/TrigonArena.cpp
  Arenas/HexagonArena.cpp
  # Objects
  Arenas/Objects/Box.cpp
  Arenas/Objects/Light.cpp
  Arenas/Objects/Patch.cpp
  # Missions
  Missions/GenericLoopFunctions.cpp
  Missions/ForagingMission.cpp
  Missions/HomingMission.cpp
  Missions/AggregationXORMission.cpp)


install(
  FILES ${ARENA_HEADERS}
  DESTINATION include/argos3/demiurge/generic-loop-functions/Arenas/)
install(
  FILES ${OBJECT_HEADERS}
  DESTINATION include/argos3/demiurge/generic-loop-functions/Arenas/Objects)
install(
  FILES ${MISSION_HEADERS}
  DESTINATION include/argos3/demiurge/generic-loop-functions/Missions/)
install(
  FILES ${GENERATOR_HEADERS}
  DESTINATION include/argos3/demiurge/generic-loop-functions/Generator/)

add_library(argos3_demiurge_generic_loopfunc SHARED ${MG_SOURCES})
target_link_libraries(argos3_demiurge_generic_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

# Install location for the e-puck plugin
install(TARGETS argos3_demiurge_generic_loopfunc
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/argos3
  ARCHIVE DESTINATION lib/argos3)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
add_executable(mission_generator Generator/MissionGeneratorMain.cpp)
target_link_libraries(mission_generator argos3_demiurge_generic_loopfunc argos3core_${ARGOS_BUILD_FOR} argos3plugin_${ARGOS_BUILD_FOR}_epuck)
