# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "APilot_autogen"
  "CMakeFiles/APilot_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/APilot_autogen.dir/ParseCache.txt"
  )
endif()
