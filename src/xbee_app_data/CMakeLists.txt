
set(XBeeAppData_HEADERS
  xbee_app_data.h
)

set(XBeeAppData_IMPLEMENTATIONS
  xbee_app_data.cc
)


add_library(xbee_app_data ${XBeeAppData_IMPLEMENTATIONS})

target_link_libraries(xbee_app_data
  xbeep.a
  xbee.a
  )

install(TARGETS xbee_app_data
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install(FILES 
  xbee_app_data.h DESTINATION include)

