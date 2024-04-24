#! /usr/bin/env sh

set -eou

if [ -z "${WASI_SYSROOT}" ]; then
    echo "WASI_SYSROOT must be set in order to build the code"
    exit 1
fi

make TARGET=generic ARCH=wasi USE_OBSOLETE_LINKER=yes -j14

wasm-opt -O3 --asyncify -g --fpcast-emu haproxy -o haproxy.wasm -pa max-func-params@32

rm haproxy