cmake_minimum_required(VERSION 3.2)

project(zerotune
	VERSION 0.1.0)

set(PROJCT_DESCRIPTION
	"Playground Tuner with a WebRTC DataChannel for the Pi Zero")

set(PROJECT_URL
	"https://github.com/kpe/zerotune")

set(ENV{PKG_CONFIG_PATH} "{CMAKE_INSTALL_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

set(LIB_RAWRTC_VERSION "librawrtc >= 0.2.2")

set(DEPENDENCY_VERSIONS "${RAWRTC_VERSION}")


# Debug build type as default
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, using DEBUG")
    set(CMAKE_BUILD_TYPE "DEBUG")
endif ()

# Use pkg-config
find_package(PkgConfig REQUIRED)

# Dependency list
set(zerotune_DEP_LIBRARIES)

# Dependency: librew
pkg_check_modules(LIB_RAWRTC REQUIRED ${LIB_RAWRTC_VERISON})
include_directories(${LIB_RAWRTC_INCLUDE_DIRS})
link_directories(${LIB_RAWRTC_LIBRARY_DIRS})
list(APPEND zerotune_DEP_LIBRARIES ${LIB_RAWRTC_LIBRARIES})

# Linker flags (includes libs that have no pkg-config files)
set(PKG_CONFIG_LIB_DIRS "-L\${libdir}")
set(PKG_CONFIG_LIBRARIES "-lrawrtc")
set(PKG_CONFIG_LIBS "${PKG_CONFIG_LIB_DIRS} ${PKG_CONFIG_LIBRARIES}")
set(PKG_CONFIG_LIB_DIRS_PRIVATE "${PKG_CONFIG_LIB_DIRS} -L${LIB_USRSCTP_LIBRARY_DIRS}")
set(PKG_CONFIG_LIBRARIES_PRIVATE "${PKG_CONFIG_LIBRARIES} ${LIB_USRSCTP_LIBRARIES}")
set(PKG_CONFIG_LIBS_PRIVATE "${PKG_CONFIG_LIB_DIRS_PRIVATE} ${PKG_CONFIG_LIBRARIES_PRIVATE}")

# Cflags (includes includes that have no pkg-config files)
set(PKG_CONFIG_CFLAGS "-I\${includedir} -I${LIB_USRSCTP_INCLUDE_DIRS}")

# Add custom target to install the library
add_custom_target(install-${PROJECT_NAME}
        $(MAKE) install
        COMMENT "Installing ${PROJECT_NAME}")


# Walk through subdirectories
add_subdirectory(src)






