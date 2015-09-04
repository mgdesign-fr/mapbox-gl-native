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


# mbgl_CApiView_t pointer type
# 
mbgl_CApiView = ctypes.c_void_p

# mbgl_CApiView_Callbacks_t and callbacks functions signatures
#
mbgl_CApiView_Callbacks_getPixelRatio      = ctypes.CFUNCTYPE(ctypes.c_float, mbgl_CApiView, ctypes.c_void_p)
mbgl_CApiView_Callbacks_getSize            = ctypes.CFUNCTYPE(None, mbgl_CApiView, ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint16), ctypes.POINTER(ctypes.c_uint16))
mbgl_CApiView_Callbacks_getFramebufferSize = ctypes.CFUNCTYPE(None, mbgl_CApiView, ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint16), ctypes.POINTER(ctypes.c_uint16))
mbgl_CApiView_Callbacks_activate           = ctypes.CFUNCTYPE(None, mbgl_CApiView, ctypes.c_void_p)
mbgl_CApiView_Callbacks_deactivate         = ctypes.CFUNCTYPE(None, mbgl_CApiView, ctypes.c_void_p)
mbgl_CApiView_Callbacks_notify             = ctypes.CFUNCTYPE(None, mbgl_CApiView, ctypes.c_void_p)
mbgl_CApiView_Callbacks_invalidate         = ctypes.CFUNCTYPE(None, mbgl_CApiView, ctypes.c_void_p)
mbgl_CApiView_Callbacks_swap               = ctypes.CFUNCTYPE(None, mbgl_CApiView, ctypes.c_void_p)
class mbgl_CApiView_Callbacks(ctypes.Structure):
  _fields_=[("getPixelRatio"     , mbgl_CApiView_Callbacks_getPixelRatio),
            ("getSize"           , mbgl_CApiView_Callbacks_getSize),
            ("getFramebufferSize", mbgl_CApiView_Callbacks_getFramebufferSize),
            ("activate"          , mbgl_CApiView_Callbacks_activate),
            ("deactivate"        , mbgl_CApiView_Callbacks_deactivate),
            ("notify"            , mbgl_CApiView_Callbacks_notify),
            ("invalidate"        , mbgl_CApiView_Callbacks_invalidate),
            ("swap"              , mbgl_CApiView_Callbacks_swap)]

# mbgl_CApiView_init
#
# SEE : int mbgl_CApiView_init(mbgl_CApiView_Callbacks_t* callbacks, void* userdata, mbgl_CApiView_t** out);
# 
mbgl_CApiView_init = _dll.mbgl_CApiView_init
mbgl_CApiView_init.argtypes = [ ctypes.POINTER(mbgl_CApiView_Callbacks), ctypes.c_void_p, ctypes.POINTER(mbgl_CApiView) ]
mbgl_CApiView_init.restype = ctypes.c_int

# mbgl_CApiView_close
#
# SEE : int mbgl_CApiView_close(mbgl_CApiView_t* view);
# 
mbgl_CApiView_close = _dll.mbgl_CApiView_close
mbgl_CApiView_close.argtypes = [ mbgl_CApiView ]
mbgl_CApiView_close.restype = ctypes.c_int


# mbgl_SQLiteCache_t pointer type
# 
mbgl_SQLiteCache = ctypes.c_void_p

# mbgl_SQLiteCache_init
#
# SEE : int mbgl_SQLiteCache_init(const char* path, mbgl_SQLiteCache_t** out);
# 
mbgl_SQLiteCache_init = _dll.mbgl_SQLiteCache_init
mbgl_SQLiteCache_init.argtypes = [ ctypes.c_char_p, ctypes.POINTER(mbgl_SQLiteCache) ]
mbgl_SQLiteCache_init.restype = ctypes.c_int

# mbgl_SQLiteCache_close
#
# SEE : int mbgl_SQLiteCache_close(mbgl_SQLiteCache_t* sqliteCache);
# 
mbgl_SQLiteCache_close = _dll.mbgl_SQLiteCache_close
mbgl_SQLiteCache_close.argtypes = [ mbgl_SQLiteCache ]
mbgl_SQLiteCache_close.restype = ctypes.c_int


# mbgl_DefaultFileSource_t pointer type
# 
mbgl_DefaultFileSource = ctypes.c_void_p

# mbgl_DefaultFileSource_init
#
# SEE : int mbgl_DefaultFileSource_init(mbgl_SQLiteCache_t* cache, mbgl_DefaultFileSource_t** out);
# 
mbgl_DefaultFileSource_init = _dll.mbgl_DefaultFileSource_init
mbgl_DefaultFileSource_init.argtypes = [ mbgl_SQLiteCache, ctypes.POINTER(mbgl_DefaultFileSource) ]
mbgl_DefaultFileSource_init.restype = ctypes.c_int

# mbgl_DefaultFileSource_close
#
# SEE : int mbgl_DefaultFileSource_close(mbgl_DefaultFileSource_t* defaultFileSource);
# 
mbgl_DefaultFileSource_close = _dll.mbgl_DefaultFileSource_close
mbgl_DefaultFileSource_close.argtypes = [ mbgl_DefaultFileSource ]
mbgl_DefaultFileSource_close.restype = ctypes.c_int

# mbgl_DefaultFileSource_setAccessToken
#
# SEE : void mbgl_DefaultFileSource_setAccessToken(mbgl_DefaultFileSource_t* fileSource, const char* accessToken);
# 
mbgl_DefaultFileSource_setAccessToken = _dll.mbgl_DefaultFileSource_setAccessToken
mbgl_DefaultFileSource_setAccessToken.argtypes = [ mbgl_DefaultFileSource, ctypes.c_char_p ]
mbgl_DefaultFileSource_setAccessToken.restype = None

# mbgl_DefaultFileSource_getAccessToken
#
# SEE : const char* mbgl_DefaultFileSource_getAccessToken(mbgl_DefaultFileSource_t* fileSource);
# 
mbgl_DefaultFileSource_getAccessToken = _dll.mbgl_DefaultFileSource_getAccessToken
mbgl_DefaultFileSource_getAccessToken.argtypes = [ mbgl_DefaultFileSource ]
mbgl_DefaultFileSource_getAccessToken.restype = ctypes.c_char_p
