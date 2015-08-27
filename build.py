#!/usr/bin/env python

import os, subprocess

CLANG="clang"

for root, dirs, files in os.walk("src"):
  for fname in files:
    name, ext = os.path.splitext(fname)
    if not ext.lower() in [".c", ".cpp"]:
      continue

    is_cpp = ext.lower() in [".cpp"]

    fpath = os.path.join(root, fname)
    clang_cmd = [CLANG, "-S" ]
    if is_cpp:
      clang_cmd += [ "-std=c++14" ]
    clang_cmd += [r"-ID:\Projets\LNPN\mapbox-gl-native\src\src", r"-ID:\Projets\LNPN\mapbox-gl-native\src\include"]
    clang_cmd += [fpath]

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