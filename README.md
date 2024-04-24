# Port of HAProxy to WASIX

To build the code, you need a working WASIX sysroot, which can be built from [the wasix-libc repo](https://github.com/wasix-org/wasix-libc).

Once you have the sysroot, you can use the `wasix-build.sh` script to build the code. Provide the path to the sysroot using the `WASI_SYSROOT` env var, like so:

```sh
WASI_SYSROOT=/path/to/wasix-libc/sysroot ./wasix-build.sh
```

There is an exmaple for running HAProxy in front of php-fpm in `examples/php-fpm-in-wasix.sh` which shows the correct way to run HAProxy in Wasmer.

# Original README

The HAProxy documentation has been split into a number of different files for
ease of use.

Please refer to the following files depending on what you're looking for :

- INSTALL for instructions on how to build and install HAProxy
- BRANCHES to understand the project's life cycle and what version to use
- LICENSE for the project's license
- CONTRIBUTING for the process to follow to submit contributions

The more detailed documentation is located into the doc/ directory :

- doc/intro.txt for a quick introduction on HAProxy
- doc/configuration.txt for the configuration's reference manual
- doc/lua.txt for the Lua's reference manual
- doc/SPOE.txt for how to use the SPOE engine
- doc/network-namespaces.txt for how to use network namespaces under Linux
- doc/management.txt for the management guide
- doc/regression-testing.txt for how to use the regression testing suite
- doc/peers.txt for the peers protocol reference
- doc/coding-style.txt for how to adopt HAProxy's coding style
- doc/internals for developer-specific documentation (not all up to date)
