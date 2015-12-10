
set(XBeeInterface_HEADERS
  xbee_interface.h
)

set(XBeeInterface_IMPLEMENTATIONS
  xbee_interface.cc
)


add_library(xbee_interface ${XBeeInterface_IMPLEMENTATIONS})

target_link_libraries(xbee_interface
  xbeep.a
  xbee.a
  )

install(TARGETS xbee_interface
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install(FILES 
  xbee_interface.h DESTINATION include)

