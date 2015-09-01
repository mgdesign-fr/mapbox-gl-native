# Context

For now, we're working to port commit `263f9fa ` (which is roughly `v0.5.2`) to Windows using MinGW.

The aims are:
* to get a self contained DLL with every dependency statically-linked
* provide a C API to overcome C++ ABI incompatibilities

Why this commit ? Just because it matches the commit we tested with our data on our one and only Mac box.

Our work is in the `mgdesign-mingw` branch.

# Status

Right now, the DLL :
* builds, using `GLEW` as then OpenGL wrapper library, with the exception of `headless_display.cpp`
* links (without `headless_display.cpp`)
* do not expose any symbol (except unwanted symbols from `nunicode` lib)
* is completely untested

# Environment

We're using :
* MinGW-w64 (`4.9.3-i686-posix-dwarf-0` from http://sourceforge.net/projects/mingw-w64/files/latest/download), which includes the 'correct' gcc
* gcc 4.9.3 (as we had problems with `clang`, see below)

As we don't know any `gyp` or `make` magic, we do not use them at the moment. We created a small and dumb `build.py` script to do the job.

TODO Folder structure (`deps`)

## Building/installing dependencies

TODO list used versions

1. `libpng`
  ```
  ./configure --prefix=/mingw
  make -j & make install
  ```

1. `libjpeg`

  The usual configure/make.

1. `libzip`
  ```
  mingw-get.exe install mingw32-libz
  ```
  Then, the usual configure/make.

1. `curl`
  ```
  ./configure --with-winssl --prefix=/mingw
  make -j & make install
  # there is probably a way to build both shared & static at once.
  ./configure --enable-static --disable-shared --with-winssl --prefix=/mingw
  make -j & make install
  ```

1. `nunicode`

from https://bitbucket.org/alekseyt/nunicode

TODO

> dans le dossier "deps", créer un répertoire "nunicode-1.5.1"
> Dézipper "nunicode-1.5.1.zip" dans le répertoire "nunicode-1.5.1" (dégager le dossier 'alekseyt-nunicode-01c8e4ebc740')
> Puis, dans une console msys, créer un dossier build dans le répertoire "nunicode-1.5.1"
cd deps/nunicode
mkdir _build
cd _build
[/c/Program\ Files\ \(x86\)/CMake/bin/]cmake .. -G "MSYS Makefiles"
make
> La lib générée se trouve dans deps\nunicode\_build\libnu

1. `autotools`

TODO Use mingw-get instead of building those tools (mingw-get versions not tested)?

Installation de msys.
> Dézipper dans c:\mingw
> Copier / coller etc/fstab-sample dans etc/fstab

Installation des autotools
> Dézipper "autoconf.tar.gz", "automake.tar.gz" et "libtool.tar.gz", puis dans une console msys:
> Autoconf:
cd autoconf-2.65
./configure --host=${MINGHOST} --prefix=/mingw
make && make install
autoconf --version
> Automake:
cd automake-1.11.1
./configure --host=${MINGHOST} --prefix=/mingw
make && make install
automake --version
> Libtool
cd libtool-2.4.10
./configure --host=${MINGHOST} --disable-shared --prefix=/mingw
make clean && make && make install
libtool --version

1. `libuv`
  ```
  autogen.sh
  ./configure --prefix=/mingw
  make -j && make install
  ```

1. `GLFW`
  ```
  mkdir _build
  cd _build     # IMPORTANT or cmake does not build with mingw gcc for some reason
  cmake .. -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=/mingw/
  make -j && make install
  ```

1. `GLEW`
  ```
  make -j
  GLEW_DEST=/mingw make install
  ```

1. `boost` 1.57.0
  ```
  cmd /k bootstrap.bat mingw
  b2 install --with-program_options --prefix=/mingw toolset=gcc --layout=system \
    variant=release -j 7 link=shared,static
  ```

# Notes about *our* MinGW installation

1. First run `mingw-w64-install.exe`
  * for clang, we had to install in `c:\mingw` (including manually moving the contents of `c:\mingw\mingw32` to `c:\mingw`)

1. install `MSYS`

  TODO

1. install `mingw-get`

  TODO

# Clang issue

We had problems with clang (both `LLVM-3.6.1-win32` and `LLVM-3.8.0-r245845-win32`) so we switched to gcc 4.9.3.

The clang problems we met were related to the c++ standard library in gcc :
* clang 3.6.1 does only support gcc 4.8.x, which does not provide `std::make_unique` (among other things)
* clang nightly (3.8.0-r245845 here) supports gcc 4.9.3 but linking problems occur with `std::call_once` (`undefined reference to 'std::__once_call'` which seems to be related to [this bug](http://lists.cs.uiuc.edu/pipermail/llvmbugs/2014-August/035744.html)).

So we went to building with gcc 4.9.3.
