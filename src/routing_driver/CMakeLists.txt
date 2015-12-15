
set(routingdriver_HEADERS
  routing_driver.h
)

set(routingdriver_IMPLEMENTATIONS
  routing_driver.cc
)


add_library(routing_driver ${routingdriver_IMPLEMENTATIONS})

target_link_libraries(gps_driver)

install(TARGETS routing_driver
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install(FILES routing_driver.h DESTINATION include)

