# This fork

This fork is about build/port mapbox-gl to win32, see [`BUILD-win32.md`](./BUILD-win32.md) for more.

We also work on creating a C API (we use with python bindings) :
* The C API is defined in `include/mbgl/mbgl_c.h`
* and implemented in `src/mbgl/c_api/`

# Mapbox GL native

An OpenGL ES-based renderer for [Mapbox Vector Tiles](https://www.mapbox.com/blog/vector-tiles), consisting of a C++ library for OS X and Linux and SDK bindings for iOS and Android.

**This repository is for development on Mapbox GL itself.**

If you want to use products _based on_ Mapbox GL, check out: 

- [Mapbox Mobile](http://mapbox.com/mobile) — iOS and Android native development libraries for vector maps in apps
- [`node-mapbox-gl-native`](https://github.com/mapbox/node-mapbox-gl-native) — Node.js library for vector map rendering
- [`react-native-mapbox-gl`](https://github.com/mapbox/react-native-mapbox-gl`) — React Native library for vector maps in apps
- [`qmapboxgl`](https://github.com/tmpsantos/qmapboxgl) — Qt library for vector map rendering

--

See [`INSTALL.md`](./INSTALL.md) for development setup and install instructions, including dependencies. 
