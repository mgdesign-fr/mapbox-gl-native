# -*- coding: windows-1252 -*-

import ctypes
import os
import sys

def _cdll( path ):
  
  handle = None
  if os.name in ( "nt", "ce" ):
    kernel32 = ctypes.windll.kernel32
    LOAD_WITH_ALTERED_SEARCH_PATH = 0x0008
    flags = LOAD_WITH_ALTERED_SEARCH_PATH
    handle = kernel32.LoadLibraryExA( path, None, flags )
  
  return ctypes.CDLL( path, handle=handle )


def _loadDLL():
  
  import itertools
  import platform
  
  # Find dll in sys.path, prefer debug dll if one exists.
  #
  if platform.system() == "Windows":
    MODULE_NAMES = [ "mbgl.dll" ]
  
  # Add current path to sys.path (for release)
  #
  sys.path.append(os.path.abspath(os.path.dirname(__file__)))
  
  # Search dll in sys.path
  #
  for modulePath, moduleName in itertools.product( sys.path, MODULE_NAMES ):
    modulePath = os.path.join( modulePath, moduleName )
    if os.path.exists( modulePath ):
      return _cdll( modulePath )
  
  # Not found in sys.path, let ctypes try to handle the module name
  #
  return ctypes.CDLL( MODULE_NAMES[0] )


_dll = _loadDLL()


# MBGL CApiView callbacks
# 
class mbgl_CApiView_Callbacks_t(ctypes.Structure):
  _fields_=[("getPixelRatio", ctypes.c_void_p),
            ("getSize", ctypes.c_void_p),
            ("getFramebufferSize", ctypes.c_void_p),
            ("activate", ctypes.c_void_p),
            ("deactivate", ctypes.c_void_p),
            ("notify", ctypes.c_void_p),
            ("invalidate", ctypes.c_void_p),
            ("swap", ctypes.c_void_p)]

# MBGL CApiView
# 
mbgl_CApiView_t = ctypes.c_void_p

# mbgl_CApiView_init
#
# SEE : int mbgl_CApiView_init(mbgl_CApiView_Callbacks_t* callbacks, void* userdata, mbgl_CApiView_t** out);
# 
mbgl_CApiView_init = _dll.mbgl_CApiView_init
mbgl_CApiView_init.argtypes = [ ctypes.POINTER(mbgl_CApiView_Callbacks_t), ctypes.c_void_p, ctypes.POINTER(ctypes.POINTER(mbgl_CApiView_t)) ]
mbgl_CApiView_init.restype = ctypes.c_int

# mbgl_CApiView_close
#
# SEE : int mbgl_CApiView_close(mbgl_CApiView_t* view);
# 
mbgl_CApiView_close = _dll.mbgl_CApiView_close
mbgl_CApiView_close.argtypes = [ ctypes.POINTER(mbgl_CApiView_t) ]
mbgl_CApiView_close.restype = ctypes.c_int
