#!/usr/bin/env python

import os, subprocess

CLANG="clang"
FILES_TO_SKIP=0

scriptPath = os.path.dirname(os.path.abspath(__file__))
buildPath = os.path.join(scriptPath, "build", "mingw")
buildGenPath = os.path.join(buildPath, "gen")
file_index = 0

# Generate config.hpp
#
#a = subprocess.call(["git", "describe", "--tags", "--always", "--abbrev=0"])
#b = subprocess.call(["git", "rev-parse", "HEAD"])
subprocess.call(["python", "scripts/build-version.py", buildGenPath, "v0.5.2", "0fcfafe7b11df0555cf9ac728e7488b1f4de5428"])

# Find shaders
#
shaderList = []
for root, dirs, files in os.walk(r"src"):
  for fname in files:
    name, ext = os.path.splitext(fname)
  
    if not ext.lower() in [".glsl"]:
      continue
    else:
      fpath = os.path.join(scriptPath, root, fname)
      shaderList.append(fpath)

# Generate shaders
#
build_shaders_cmd = ["python", "scripts/build-shaders.py", buildGenPath] + shaderList
subprocess.call(build_shaders_cmd)

# Compile source files
#
OBJs = []
src_folders = ["src", os.path.join("platform", "default"), buildGenPath]

EXCLUDED_FILES = [ "asset_request_zip.cpp" ]

for src_folder in src_folders:

  for root, dirs, files in os.walk(src_folder):
    
    for fname in files:
      name, ext = os.path.splitext(fname)

      if fname in EXCLUDED_FILES:
        print "skipping '%s'" % os.path.join(root, fname)
        continue

      if not ext.lower() in [".c", ".cpp"]:
        continue

      in_path = os.path.join(root, fname)
      out_path = os.path.join(buildPath, "%s.o" % name)
     
      if file_index < FILES_TO_SKIP:
        file_index += 1
        continue

      if os.path.exists(out_path):
        OBJs.append(out_path)
        continue

      is_cpp = ext.lower() in [".cpp"]

      clang_cmd = []
      if is_cpp:
        clang_cmd += [ "gcc", "-std=c++14" ]
        clang_cmd += ["-frtti"]                                                                             # NOTE(nico) - from osx build log
      else:
        clang_cmd += [ "gcc" ]

      #clang_cmd += ["-S"]
      #clang_cmd += ["-E"]
      clang_cmd += ["-DNDEBUG", "-O3"]
      clang_cmd += ["-fexceptions"]                                                                         # NOTE(nico) - from osx build log
      #clang_cmd += ["-fPIC"]                                                                               # NOTE(nico) - from osx build log - useless for win32
      #clang_cmd += ["-femulated-tls"]                                                                      # NOTE(nico) - ?test? for 'undefined reference to `std::__once_call'
      clang_cmd += ["-pthread"]                                                                             # NOTE(nico) - otherwise linking fails
      clang_cmd += ["-D_USE_MATH_DEFINES"]                                                                  # NOTE(nico) - to define M_PI
      clang_cmd += ["-DCURL_STATICLIB", "-DZIP_EXTERN="]                                                    # NOTE(nico) - for static linking
      clang_cmd += ["-I" + os.path.join(scriptPath, "src"), "-I" + os.path.join(scriptPath, "include")]
      #clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "libuv-1.0.2", "include")]
      clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "libuv-0.10.36", "include")]
      clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1")]
      clang_cmd += ["-I" + r"C:\mingw\lib\libzip\include"]                                                  # NOTE(nico) - ? for <zipconf.h>
      clang_cmd += ["-I" + r"C:\mingw\include"]                                                             # NOTE(nico) - ? for gcc only, which is suppose to look here
      clang_cmd += ["-I" + r"M:\boost_1_57\include\boost-1_57"]                                             # TODO(nico) put in 'deps' ?
      clang_cmd += ["-I" + os.path.join(buildGenPath, "include")]                                           # NOTE(jeff) - for mbgl generated files
      clang_cmd += ["-c", "-o", out_path]
      clang_cmd += [in_path]
      #clang_cmd += ["--verbose"]

      print file_index, "**", clang_cmd
      code = subprocess.call(clang_cmd)
      if code == 0:
        OBJs.append(out_path)

      file_index += 1

assert len(OBJs) == len(set(OBJs))

# Link DLL
#
clang_cmd = [ "g++", "-std=c++1y", "-shared", "-g", "-pthread" ]
clang_cmd += [ "-static" ]
clang_cmd += [ "-Wl,--export-all-symbols", "-Wl,--out-implib=mapbox-gl.dll.a" ]                                  # NOTE(nico) - should be 'hidden' but...
#clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "libuv-1.0.2", ".libs") ]
clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "libuv-0.10.36") ]
clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1", "_build", "libnu") ]
clang_cmd += [ "-o", "mapbox-gl.dll" ]
#clang_cmd += [ "--verbose" ]
#clang_cmd += [ "-Wl,--verbose" ]
clang_cmd += OBJs
# IMPORTANT(nico) - must come *after* the input files
# IMPORTANT(nico) - order is important for the linker
clang_cmd += [ "-luv" ]
clang_cmd += [ "-lnu" ]
clang_cmd += [ "-lglew32" ]
clang_cmd += [ "-lz", "-lzip", "-lcurl", "-ljpeg", "-lpng", "-lsqlite3", "-lglfw3" ]
clang_cmd += [ "-lwldap32"]                                                                                # NOTE(nico) - for static lcurl
clang_cmd += [ "-lwsock32", "-lws2_32", "-liphlpapi", "-lpsapi" ]                                          # NOTE(nico) - libuv-0.10.36
clang_cmd += [ "-lopengl32", "-lgdi32" ]
print clang_cmd
subprocess.call(clang_cmd)

# Link `render.exe`
#
clang_cmd = [ "g++", "-std=c++1y", "-g", "-pthread" ]
clang_cmd += [ "bin\\render.cpp" ]
#clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "libuv-1.0.2", ".libs") ]
clang_cmd += ["-I" + os.path.join(scriptPath, "src"), "-I" + os.path.join(scriptPath, "include")]
clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "libuv-0.10.36", "include")]
clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1")]
clang_cmd += ["-I" + r"C:\mingw\include"]                                                             # NOTE(nico) - ? for gcc only, which is suppose to look here
clang_cmd += ["-I" + r"M:\boost_1_57\include\boost-1_57"]                                             # TODO(nico) put in 'deps' ?
clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "libuv-0.10.36") ]
clang_cmd += [ "-L"+os.path.join(scriptPath, "..", "deps", "nunicode-1.5.1", "_build", "libnu") ]
clang_cmd += [ "-L." ]
clang_cmd += [ "-o", "render.exe" ]
#clang_cmd += [ "--verbose" ]
#clang_cmd += [ "-Wl,--verbose" ]
'''
clang_cmd += OBJs
'''
# IMPORTANT(nico) - must come *after* the input files
# IMPORTANT(nico) - order is important for the linker
clang_cmd += [ "-luv" ]
clang_cmd += [ "-lwsock32", "-lws2_32", "-liphlpapi", "-lpsapi" ]                                          # NOTE(nico) - libuv-0.10.36
clang_cmd += [ "-lmapbox-gl" ]
'''
clang_cmd += [ "-lnu" ]
clang_cmd += [ "-lglew32" ]
clang_cmd += [ "-lz", "-lzip", "-lcurl", "-ljpeg", "-lpng", "-lsqlite3", "-lglfw3" ]
clang_cmd += [ "-lwldap32"]                                                                                # NOTE(nico) - for static lcurl
clang_cmd += [ "-lopengl32", "-lgdi32" ]
'''
print clang_cmd
subprocess.call(clang_cmd)

"""
import os, subprocess, sys, errno

if os.name == 'nt':
    import msvcrt
else:
    import fcntl

# from http://stackoverflow.com/a/600612
def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

def flock(lockfile, cmd_list, verbose = False):
    mkdir_p(os.path.dirname(lockfile))

    if os.name == 'nt':
        print "*"*80
        print lockfile, cmd_list

        fd = open(lockfile, 'w+')
        msvcrt.locking(fd.fileno(), msvcrt.LK_LOCK, 2147483647L)
    else:
        fd = os.open(lockfile, os.O_RDONLY | os.O_NOCTTY | os.O_CREAT, 0o666)
        fcntl.flock(fd, fcntl.LOCK_EX)

    if os.name == 'nt':
        orig_cmd_list = '-c "' + ' '.join(cmd_list) + '"'
        cmd_list = []
        cmd_list.append(r'"C:\Program Files (x86)\Git\bin\sh.exe"')
        cmd_list.append(orig_cmd_list)

    if verbose:
        print(' '.join(cmd_list))
    return subprocess.call(cmd_list)

if '__main__' == __name__:
    try:
        if sys.argv[1] == '-v':
            sys.exit(flock(sys.argv[2], sys.argv[3:], True))
        else:
            sys.exit(flock(sys.argv[1], sys.argv[2:]))
    except KeyboardInterrupt:
        sys.exit(1)
"""