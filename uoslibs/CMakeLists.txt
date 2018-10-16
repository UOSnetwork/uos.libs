file(GLOB HEADERS "include/uoslibs/*.hpp")

add_library( uoslibs
             uoslibs.cpp
             ${HEADERS}
           )
target_link_libraries( uoslibs appbase fc eosio_chain chain_plugin chain_api_plugin chainbase)

target_include_directories( uoslibs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
