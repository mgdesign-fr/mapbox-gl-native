#!/usr/bin/env python

import os, subprocess

CLANG="clang"
FILES_TO_SKIP=0

scriptPath = os.path.dirname(os.path.abspath(__file__))
file_index = 0

# Generate config.hpp
# TODO destination should not be scriptPath but a deletable build directory
#
#a = subprocess.call(["git", "describe", "--tags", "--always", "--abbrev=0"])
#b = subprocess.call(["git", "rev-parse", "HEAD"])
subprocess.call(["python", "scripts/build-version.py", scriptPath, "v0.5.2", "0fcfafe7b11df0555cf9ac728e7488b1f4de5428"])

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
# TODO destination should not be scriptPath but a deletable build directory
#
build_shaders_cmd = ["python", "scripts/build-shaders.py", scriptPath] + shaderList
subprocess.call(build_shaders_cmd)

# Compile source files
#
OBJs = []

for root, dirs, files in os.walk("src"):
  
  for fname in files:
    name, ext = os.path.splitext(fname)

    if not ext.lower() in [".c", ".cpp"]:
      continue

    in_path = os.path.join(root, fname)
    out_path = os.path.join(scriptPath, "build", "%s.o" % name)
    OBJs.append(out_path)
   
    if file_index < FILES_TO_SKIP:
      file_index += 1
      continue

    if os.path.exists(out_path):
      continue

    is_cpp = ext.lower() in [".cpp"]

    clang_cmd = []
    if is_cpp:
      clang_cmd += [ "clang", "-std=c++1y" ]
    else:
      clang_cmd += [ "clang" ]

    #clang_cmd += ["-S"]
    clang_cmd += ["-DNDEBUG", "-O3"]
    clang_cmd += ["-frtti", "-fexceptions", "-fPIC", "-MMD", "-pthread"]
    clang_cmd += ["-D_USE_MATH_DEFINES"]                                                                  # NOTE(nico) - to define M_PI
    clang_cmd += ["-I" + os.path.join(scriptPath, "src"), "-I" + os.path.join(scriptPath, "include")]
    clang_cmd += ["-I" + os.path.join(scriptPath, "..", "deps", "libuv-1.0.2", "include")]
    clang_cmd += ["-I" + r"M:\boost_1_57\include\boost-1_57"]                                             # TODO(nico) put in 'deps' ?
    clang_cmd += ["-c", "-o", out_path]
    clang_cmd += [in_path]
    #clang_cmd += ["--verbose"]

    print file_index, "**", clang_cmd
    subprocess.call(clang_cmd)

    file_index += 1

assert len(OBJs) == len(set(OBJs))

# Link DLL
#
clang_cmd = [ "clang++", "-std=c++1y", "-shared", "-g", "-pthread" ]
clang_cmd += [ "-o", "toto.dll" ]
clang_cmd += [ "--verbose" ]
#clang_cmd += [ "-L" ]
clang_cmd += OBJs
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