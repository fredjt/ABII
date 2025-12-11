# ABII - Application Binary Interface Interceptor

ABII is a powerful tool designed to intercept and manipulate application binary interfaces (ABIs) at runtime. It allows
users to monitor, modify, and analyze the behavior of applications by intercepting C function calls to shared libraries.

This tool does nothing by itself; it requires a plugin for the library you want to intercept.

## Usage

`abii <plugin> [--searchpath <searchpath>] <program> [<args>...]`

`<plugin>` is the name of the plugin to load. This is usually the name of the library you want to intercept without
the "lib" prefix and ".so" suffix, followed by a "-" and the plugin type (eg. ~~lib~~ c ~~.so~~ -logger -> c-logger for
logging libc.so calls).

#### Options:

-h, --help Show this screen.

--version Show the version number.

--searchpath <searchpath>     Additional colon-separated plugin search path. The default paths are
`/usr/share/abii/plugins/32:/usr/share/abii/plugins/64`, but more can be added for finding plugins installed in other
locations.

## Current Plugins

- Coming soon!

## In-Development

- glibc-logger: A plugin to log calls to glibc (libc.so.6) functions.
- steam_api-logger: A plugin to log calls to Steam API (libsteam_api.so) functions.

## Future Plans

- glibc-replay: A plugin to capture calls and recreate a source file with the captured calls to be replayed.
- glibc-python: A plugin to enable replacing or hooking library calls with python scripts.
