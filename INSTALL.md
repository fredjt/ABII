# Building and Installing ABII

To build ABII, obtain the <a href=https://github.com/fredjt/ABII/releases/latest>latest sources</a>, unpack them, and
enter the source directory. Then,

```
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -GNinja
ninja
sudo ninja install
```

To install in a local directory instead of system-wide in /usr, change CMAKE_INSTALL_PREFIX to the directory of
choice. \
To build the test suite, ensure Boost::Test is installed on the system and add -DBUILD_TESTS=ON to the cmake command
line. \
A 32-bit version can be created with the cmake option -DBIT32=ON. This is required for building plugins that will be
injected into applications like steam. 
