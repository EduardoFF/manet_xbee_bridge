
set(gpsclient_HEADERS
  gps_client.h
)

set(gpsclient_IMPLEMENTATIONS
  gps_client.cc
)


add_library(gps_client ${gpsclient_IMPLEMENTATIONS})

target_link_libraries(gps_client)

install(TARGETS gps_client
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install(FILES gps_client.h DESTINATION include)

