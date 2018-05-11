#

set(zerotune_SOURCES
	wsnac.c
	fft.c
	kiss_fft.c
	capture.c
	)

add_library(zerotune SHARED ${zerotune_SOURCES})
target_link_libraries(zerotune ${zerotune_DEP_LIBRARIES})

add_library(zerotune-static STATIC ${zerotune_SOURCES})
target_link_libraries(zerotune-static ${zerotune_DEP_LIBRARIES})

install(TARGETS zerotune zerotune-static
        DESTINATION lib)

add_executable(zerotune-test zerotune-test.c)
target_link_libraries(zerotune-test zerotune-static asound m)
install(TARGETS zerotune-test DESTINATION bin)

# Install header
install(FILES zerotune.h DESTINATION include)



# Generate pkg-config file & install it
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkg-config.pc.cmakein
#        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
#        DESTINATION lib/pkgconfig)
