# This fork

This fork is about build/port mapbox-gl to win32, see [`BUILD-win32.md`](./BUILD-win32.md) for more.

We also work on creating a C API (we use with python bindings) :
* The C API is defined in `include/mbgl/mbgl_c.h`
* and implemented in `src/mbgl/c_api/`

# Mapbox GL Native

An OpenGL-based renderer for [Mapbox Vector Tiles](https://www.mapbox.com/blog/vector-tiles), consisting of a C++ library for OS X and Linux and SDK bindings for iOS, Android and Node.js.
