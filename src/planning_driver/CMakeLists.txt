
set(planningdriver_HEADERS
  planning_driver.h
)

set(planningdriver_IMPLEMENTATIONS
  planning_driver.cc
)


add_library(planning_driver ${planningdriver_IMPLEMENTATIONS})

target_link_libraries(planning_driver)

install(TARGETS planning_driver
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install(FILES planning_driver.h DESTINATION include)

