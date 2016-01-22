
include_directories(${PROJECT_SOURCE_DIR}/gps_driver)
include_directories(${PROJECT_SOURCE_DIR}/xbee_interface)
include_directories(${PROJECT_SOURCE_DIR}/xbee_app_data)
include_directories(${PROJECT_SOURCE_DIR}/routing_driver)
include_directories(${PROJECT_SOURCE_DIR}/timer)
set(EndNode_HEADERS
  end_node.h
)

set(EndNode_IMPLEMENTATIONS
  end_node.cc
)


add_executable(end_node ${EndNode_IMPLEMENTATIONS})

target_link_libraries(end_node
  gps_driver
  routing_driver
  xbee_interface
  xbee_app_data
  timer
  pthread
  lcm
  )




install(TARGETS end_node
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install(FILES 
  end_node.h DESTINATION include)

