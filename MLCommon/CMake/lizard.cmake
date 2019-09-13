#This file add optional target to execute lizard scaner on project
add_custom_target("run_lizard"
  COMMAND lizard -l cpp -C 7 -a 5 -w -t 3 -m -s cyclomatic_complexity -x \"*/build/*\" -x \"./CMake/*\" -x \"*/ThirdParty/*\" .
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
