# ABII - Application Binary Interface Interceptor

ABII is a powerful linux tool designed to intercept and manipulate application binary interfaces (ABIs) at runtime. It
allows users to monitor, modify, and analyze the behavior of applications by intercepting C function calls to shared
libraries.

This tool does nothing by itself; it requires a plugin for the library you want to intercept. \
It does come with an example plugin for testing and as a base for making your own.

## Usage

### `abii [--searchpath <searchpath>] <plugin> <syms> <program> [<args>...]`

`<plugin>` is the name of the plugin to load. This is usually the name of the library you want to intercept without
the "lib" prefix and ".so" suffix, followed by a "-" and the plugin type (eg. ~~lib~~ c ~~.so~~ -logger -> c-logger for
logging libc.so calls). 

`<syms>` is a comma-separated list of function names to intercept (eg. dlopen,dlsym,dlclose).

`<program>` is the path to the executable you want to run with ABII.

`[<args>...]` are any additional arguments to pass to the program being run.

Calls are logged to the user's home directory in a log folder named `abii_log`. Inside is separate logs for the injected
process and all child process.

#### Options:

-h, --help Show this screen.

--version Show the version number.

--searchpath <searchpath>     Additional colon-separated plugin search path. The default paths are
`/usr/share/abii/plugins/32:/usr/share/abii/plugins/64`, but more can be added for finding plugins installed in other
locations.

### `abii --list-syms <plugin>`

Lists all available symbols for the specified plugin.

## Current Plugins

- <a href=https://github.com/fredjt/ABII-example-logging-plugin>example-logger</a>: An example plugin to showcase
  logging function calls.

## In-Development

- <a href=https://github.com/fredjt/ABII-c-logging-plugin>c-logger</a>: A plugin to log calls to glibc (libc.so.6)
  functions.
- steam_api-logger: A plugin to log calls to Steam API (libsteam_api.so) functions.

## Future Plans

- c-replay: A plugin to capture calls and recreate a source file with the captured calls to be replayed.
- c-python: A plugin to enable replacing or hooking library calls with python scripts.
