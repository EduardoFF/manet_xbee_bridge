
set(Timer_HEADERS
  timer.h
)

set(Timer_IMPLEMENTATIONS
  timer.cc
)


add_library(timer ${Timer_IMPLEMENTATIONS})

target_link_libraries(timer
  xbeep.a
  xbee.a
  )

install(TARGETS timer
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install(FILES 
  timer.h DESTINATION include)

