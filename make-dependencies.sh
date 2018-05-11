#!/bin/sh
set -e


# Build path
if [ -z "$BUILD_PATH" ]; then
    export BUILD_PATH=`readlink -f ${PWD}/build`
fi

# Dependencies
RAWRTC_VERSION="0.2.2"
RAWRTC_URL="https://github.com/rawrtc/rawrtc/archive/v${RAWRTC_VERSION}.tar.gz"
RAWRTC_PATH="rawrtc"

# Prefix
export PREFIX=${BUILD_PATH}/prefix
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig:${PKG_CONFIG_PATH}
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${BUILD_PATH}/dependencies/rawrtc/build/prefix/lib/pkgconfig
export CFLAGS="${CFLAGS} -I${PREFIX}/include"
export CPPFLAGS="${CFLAGS}"
export LDFLAGS="${LDFLAGS} -L${PREFIX}/lib"
echo "Environment vars:"
echo "PREFIX: ${PREFIX}"
echo "PKG_CONFIG_PATH: ${PKG_CONFIG_PATH}"
echo "CFLAGS: ${CFLAGS}"
echo "CPPFLAGS: ${CPPFLAGS}"
echo "LDFLAGS: ${LDFLAGS}"
echo ""

DEPS_DIR=${BUILD_PATH}/dependencies
mkdir -p ${BUILD_PATH}/dependencies
cd ${DEPS_DIR}

if [ ! -d "${RAWRTC_PATH}" ]; then
    echo "Fetching RawRTC"
    curl -L -C - -o rawrtc-v${RAWRTC_VERSION}.tar.gz ${RAWRTC_URL}
    tar -xzf rawrtc-v${RAWRTC_VERSION}.tar.gz
    mv rawrtc-${RAWRTC_VERSION} ${RAWRTC_PATH}
fi

have_rawrtc=true
pkg-config --atleast-version=${RAWRTC_VERSION} rawrtc || have_rawrtc=false
if [ "$have_rawrtc" = false ]; then
    echo "\n\n============================"
    echo "Building RawRTC Dependencies"
    pushd ${DEPS_DIR}/rawrtc
    ${DEPS_DIR}/rawrtc/make-dependencies.sh
    echo "\n\n==============="
    echo "Building RawRTC"
    mkdir -p ${DEPS_DIR}/rawrtc
    pushd ${DEPS_DIR}/rawrtc
    cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} ${DEPS_DIR}/rawrtc
    make install
fi
