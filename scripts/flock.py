#!/usr/bin/env python

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

def flock(lockfile, cmd_list, verbose = True):
    mkdir_p(os.path.dirname(lockfile))
    
    if os.name == 'nt':
        fd = open(lockfile, 'w+')
        msvcrt.locking(fd.fileno(), msvcrt.LK_LOCK, 2147483647L)
        cmd_list = ["sh", "-c", " ".join(cmd_list)]
    else:
        fd = os.open(lockfile, os.O_RDONLY | os.O_NOCTTY | os.O_CREAT, 0o666)
        fcntl.flock(fd, fcntl.LOCK_EX)
    
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
