#! /usr/bin/env sh

# Note, the path to the scripts must be the same for both HAProxy and php-fpm,
# otherwise php-fpm doesn't find the scripts; hence the weird mount location.
wasmer run --net \
  --mapdir /conf:. \
  --mapdir /path/to/php/app:/path/to/php/app \
  ../haproxy.wasm \
  -- \
  -f /conf/php-fpm-wasix.cfg \
  $@