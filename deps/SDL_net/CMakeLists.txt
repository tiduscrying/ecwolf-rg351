if(NOT TARGET SDL2::SDL2)
	find_package(SDL2 REQUIRED)
endif()

add_library(SDL2_net STATIC
	SDL_net.h
	SDLnet.c
	SDLnetselect.c
	SDLnetTCP.c
	SDLnetUDP.c
)

target_link_libraries(SDL2_net PUBLIC SDL2::SDL2)
if(WIN32)
	target_link_libraries(SDL2_net PRIVATE iphlpapi wsock32)
endif()

target_include_directories(SDL2_net PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
