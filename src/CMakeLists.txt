#


if(EXISTS "/opt/vc/src/hello_pi/hello_fft/gpu_fft.h")
  add_definitions(-DUSE_GPU_FFT)
endif()

#
# gpu_fft
#

if(EXISTS "/opt/vc/src/hello_pi/hello_fft/gpu_fft.h")
set(gpu_fft_DIR /opt/vc/src/hello_pi/hello_fft/)
set(gpu_fft_SOURCES
	${gpu_fft_DIR}/gpu_fft.c
	${gpu_fft_DIR}/gpu_fft_base.c
	${gpu_fft_DIR}/gpu_fft_shaders.c
	${gpu_fft_DIR}/gpu_fft_twiddles.c
	${gpu_fft_DIR}/mailbox.c
	)
add_library(gpu_fft SHARED ${gpu_fft_SOURCES})
target_include_directories(gpu_fft PRIVATE ${gpu_fft_DIR})
target_link_libraries(gpu_fft rt m dl)
add_library(gpu_fft-static STATIC ${gpu_fft_SOURCES})
target_include_directories(gpu_fft-static PRIVATE ${gpu_fft_DIR})
target_link_libraries(gpu_fft-static rt m dl)

install(TARGETS gpu_fft gpu_fft-static
        DESTINATION lib)

endif()

#
# zerotune
#


set(zerotune_SOURCES
	wsnac.c
	fft.c
	kiss_fft.c
	capture.c
	)

add_library(zerotune SHARED ${zerotune_SOURCES})
add_library(zerotune-static STATIC ${zerotune_SOURCES})

target_include_directories(zerotune PRIVATE ${gpu_fft_DIR})
target_include_directories(zerotune-static PRIVATE ${gpu_fft_DIR})


if(EXISTS "/opt/vc/src/hello_pi/hello_fft/gpu_fft.h")
 target_link_libraries(zerotune ${zerotune_DEP_LIBRARIES} gpu_fft-static)
 target_link_libraries(zerotune-static ${zerotune_DEP_LIBRARIES} gpu_fft-static)
else()
 target_link_libraries(zerotune ${zerotune_DEP_LIBRARIES} )
 target_link_libraries(zerotune-static ${zerotune_DEP_LIBRARIES} )
endif()

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
