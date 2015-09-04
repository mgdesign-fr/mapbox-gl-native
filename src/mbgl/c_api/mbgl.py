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

# mbgl_View_t type
# 
class mbgl_View_t(ctypes.Structure):
  pass

# mbgl_CApiView_t type
# 
class mbgl_CApiView_t(ctypes.Structure):
  pass

# mbgl_CApiView_Callbacks_t and callbacks functions signatures
#
mbgl_CApiView_Callbacks_t_getPixelRatio      = ctypes.CFUNCTYPE(ctypes.c_float, ctypes.POINTER(mbgl_CApiView_t), ctypes.c_void_p)
mbgl_CApiView_Callbacks_t_getSize            = ctypes.CFUNCTYPE(None, ctypes.POINTER(mbgl_CApiView_t), ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint16), ctypes.POINTER(ctypes.c_uint16))
mbgl_CApiView_Callbacks_t_getFramebufferSize = ctypes.CFUNCTYPE(None, ctypes.POINTER(mbgl_CApiView_t), ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint16), ctypes.POINTER(ctypes.c_uint16))
mbgl_CApiView_Callbacks_t_activate           = ctypes.CFUNCTYPE(None, ctypes.POINTER(mbgl_CApiView_t), ctypes.c_void_p)
mbgl_CApiView_Callbacks_t_deactivate         = ctypes.CFUNCTYPE(None, ctypes.POINTER(mbgl_CApiView_t), ctypes.c_void_p)
mbgl_CApiView_Callbacks_t_notify             = ctypes.CFUNCTYPE(None, ctypes.POINTER(mbgl_CApiView_t), ctypes.c_void_p)
mbgl_CApiView_Callbacks_t_invalidate         = ctypes.CFUNCTYPE(None, ctypes.POINTER(mbgl_CApiView_t), ctypes.c_void_p)
mbgl_CApiView_Callbacks_t_swap               = ctypes.CFUNCTYPE(None, ctypes.POINTER(mbgl_CApiView_t), ctypes.c_void_p)
class mbgl_CApiView_Callbacks_t(ctypes.Structure):
  _fields_=[("getPixelRatio"     , mbgl_CApiView_Callbacks_t_getPixelRatio),
            ("getSize"           , mbgl_CApiView_Callbacks_t_getSize),
            ("getFramebufferSize", mbgl_CApiView_Callbacks_t_getFramebufferSize),
            ("activate"          , mbgl_CApiView_Callbacks_t_activate),
            ("deactivate"        , mbgl_CApiView_Callbacks_t_deactivate),
            ("notify"            , mbgl_CApiView_Callbacks_t_notify),
            ("invalidate"        , mbgl_CApiView_Callbacks_t_invalidate),
            ("swap"              , mbgl_CApiView_Callbacks_t_swap)]

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


# mbgl_SQLiteCache_t type
# 
class mbgl_SQLiteCache_t(ctypes.Structure):
  pass

# mbgl_SQLiteCache_init
#
# SEE : int mbgl_SQLiteCache_init(const char* path, mbgl_SQLiteCache_t** out);
# 
mbgl_SQLiteCache_init = _dll.mbgl_SQLiteCache_init
mbgl_SQLiteCache_init.argtypes = [ ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(mbgl_SQLiteCache_t)) ]
mbgl_SQLiteCache_init.restype = ctypes.c_int

# mbgl_SQLiteCache_close
#
# SEE : int mbgl_SQLiteCache_close(mbgl_SQLiteCache_t* sqliteCache);
# 
mbgl_SQLiteCache_close = _dll.mbgl_SQLiteCache_close
mbgl_SQLiteCache_close.argtypes = [ ctypes.POINTER(mbgl_SQLiteCache_t) ]
mbgl_SQLiteCache_close.restype = ctypes.c_int


# mbgl_DefaultFileSource_t type
# 
class mbgl_DefaultFileSource_t(ctypes.Structure):
  pass

# mbgl_DefaultFileSource_init
#
# SEE : int mbgl_DefaultFileSource_init(mbgl_SQLiteCache_t* cache, mbgl_DefaultFileSource_t** out);
# 
mbgl_DefaultFileSource_init = _dll.mbgl_DefaultFileSource_init
mbgl_DefaultFileSource_init.argtypes = [ ctypes.POINTER(mbgl_SQLiteCache_t), ctypes.POINTER(ctypes.POINTER(mbgl_DefaultFileSource_t)) ]
mbgl_DefaultFileSource_init.restype = ctypes.c_int

# mbgl_DefaultFileSource_close
#
# SEE : int mbgl_DefaultFileSource_close(mbgl_DefaultFileSource_t* defaultFileSource);
# 
mbgl_DefaultFileSource_close = _dll.mbgl_DefaultFileSource_close
mbgl_DefaultFileSource_close.argtypes = [ ctypes.POINTER(mbgl_DefaultFileSource_t) ]
mbgl_DefaultFileSource_close.restype = ctypes.c_int

# mbgl_DefaultFileSource_setAccessToken
#
# SEE : void mbgl_DefaultFileSource_setAccessToken(mbgl_DefaultFileSource_t* fileSource, const char* accessToken);
# 
mbgl_DefaultFileSource_setAccessToken = _dll.mbgl_DefaultFileSource_setAccessToken
mbgl_DefaultFileSource_setAccessToken.argtypes = [ ctypes.POINTER(mbgl_DefaultFileSource_t), ctypes.c_char_p ]
mbgl_DefaultFileSource_setAccessToken.restype = None

# mbgl_DefaultFileSource_getAccessToken
#
# SEE : const char* mbgl_DefaultFileSource_getAccessToken(mbgl_DefaultFileSource_t* fileSource);
# 
mbgl_DefaultFileSource_getAccessToken = _dll.mbgl_DefaultFileSource_getAccessToken
mbgl_DefaultFileSource_getAccessToken.argtypes = [ ctypes.POINTER(mbgl_DefaultFileSource_t) ]
mbgl_DefaultFileSource_getAccessToken.restype = ctypes.c_char_p


# mbgl_Transform_t type
# 
class mbgl_Transform_t(ctypes.Structure):
  pass

# mbgl_Transform_init
#
# SEE : int mbgl_Transform_init(mbgl_View_t* view, mbgl_Transform_t** out);
# 
mbgl_Transform_init = _dll.mbgl_Transform_init
mbgl_Transform_init.argtypes = [ ctypes.POINTER(mbgl_View_t), ctypes.POINTER(ctypes.POINTER(mbgl_Transform_t)) ]
mbgl_Transform_init.restype = ctypes.c_int

# mbgl_Transform_close
#
# SEE : int mbgl_Transform_close(mbgl_Transform_t* transform);
# 
mbgl_Transform_close = _dll.mbgl_Transform_close
mbgl_Transform_close.argtypes = [ ctypes.POINTER(mbgl_Transform_t) ]
mbgl_Transform_close.restype = ctypes.c_int

# mbgl_Transform_resize
#
# SEE : int mbgl_Transform_resize(mbgl_Transform_t* transform, uint16_t width, uint16_t height);
# 
mbgl_Transform_resize = _dll.mbgl_Transform_resize
mbgl_Transform_resize.argtypes = [ ctypes.POINTER(mbgl_Transform_t), ctypes.c_uint16, ctypes.c_uint16]
mbgl_Transform_resize.restype = ctypes.c_int

# mbgl_Transform_setLatLng
#
# SEE : void mbgl_Transform_setLatLng(mbgl_Transform_t* transform, double latitude, double longitude);
# 
mbgl_Transform_setLatLng = _dll.mbgl_Transform_setLatLng
mbgl_Transform_setLatLng.argtypes = [ ctypes.POINTER(mbgl_Transform_t), ctypes.c_double, ctypes.c_double]
mbgl_Transform_setLatLng.restype = None

# mbgl_Transform_getLatLng
#
# SEE : void mbgl_Transform_getLatLng(mbgl_Transform_t* transform, double* outLatitude, double* outLongitude);
# 
mbgl_Transform_getLatLng = _dll.mbgl_Transform_getLatLng
mbgl_Transform_getLatLng.argtypes = [ ctypes.POINTER(mbgl_Transform_t), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)]
mbgl_Transform_getLatLng.restype = None

# mbgl_Transform_setZoom
#
# SEE : void mbgl_Transform_setZoom(mbgl_Transform_t* transform, double zoom);
# 
mbgl_Transform_setZoom = _dll.mbgl_Transform_setZoom
mbgl_Transform_setZoom.argtypes = [ ctypes.POINTER(mbgl_Transform_t), ctypes.c_double]
mbgl_Transform_setZoom.restype = None

# mbgl_Transform_getZoom
#
# SEE : double mbgl_Transform_getZoom(mbgl_Transform_t* transform);
# 
mbgl_Transform_getZoom = _dll.mbgl_Transform_getZoom
mbgl_Transform_getZoom.argtypes = [ ctypes.POINTER(mbgl_Transform_t)]
mbgl_Transform_getZoom.restype = ctypes.c_double

# mbgl_Transform_setLatLngZoom
#
# SEE : void mbgl_Transform_setLatLngZoom(mbgl_Transform_t* transform, double latitude, double longitude, double zoom);
# 
mbgl_Transform_setLatLngZoom = _dll.mbgl_Transform_setLatLngZoom
mbgl_Transform_setLatLngZoom.argtypes = [ ctypes.POINTER(mbgl_Transform_t), ctypes.c_double, ctypes.c_double, ctypes.c_double]
mbgl_Transform_setLatLngZoom.restype = None

# mbgl_Transform_setAngle
#
# SEE : void mbgl_Transform_setAngle(mbgl_Transform_t* transform, double angle);
# 
mbgl_Transform_setAngle = _dll.mbgl_Transform_setAngle
mbgl_Transform_setAngle.argtypes = [ ctypes.POINTER(mbgl_Transform_t), ctypes.c_double]
mbgl_Transform_setAngle.restype = None

# mbgl_Transform_getAngle
#
# SEE : double mbgl_Transform_getAngle(mbgl_Transform_t* transform);
# 
mbgl_Transform_getAngle = _dll.mbgl_Transform_getAngle
mbgl_Transform_getAngle.argtypes = [ ctypes.POINTER(mbgl_Transform_t)]
mbgl_Transform_getAngle.restype = ctypes.c_double

# mbgl_Transform_setPitch
#
# SEE : void mbgl_Transform_setPitch(mbgl_Transform_t* transform, double pitch);
# 
mbgl_Transform_setPitch = _dll.mbgl_Transform_setPitch
mbgl_Transform_setPitch.argtypes = [ ctypes.POINTER(mbgl_Transform_t), ctypes.c_double]
mbgl_Transform_setPitch.restype = None

# mbgl_Transform_getPitch
#
# SEE : double mbgl_Transform_getPitch(mbgl_Transform_t* transform);
# 
mbgl_Transform_getPitch = _dll.mbgl_Transform_getPitch
mbgl_Transform_getPitch.argtypes = [ ctypes.POINTER(mbgl_Transform_t)]
mbgl_Transform_getPitch.restype = ctypes.c_double


# mbgl_MapData_t type
# 
class mbgl_MapData_t(ctypes.Structure):
  pass

# mbgl_MapData_init
#
# SEE : int mbgl_MapData_init(int mode, float pixelRatio, mbgl_MapData_t** out);
# 
mbgl_MapData_init = _dll.mbgl_MapData_init
mbgl_MapData_init.argtypes = [ ctypes.c_int, ctypes.c_float, ctypes.POINTER(ctypes.POINTER(mbgl_MapData_t)) ]
mbgl_MapData_init.restype = ctypes.c_int

# mbgl_MapData_close
#
# SEE : int mbgl_MapData_close(mbgl_MapData_t* mapData);
# 
mbgl_MapData_close = _dll.mbgl_MapData_close
mbgl_MapData_close.argtypes = [ ctypes.POINTER(mbgl_MapData_t) ]
mbgl_MapData_close.restype = ctypes.c_int


# mbgl_MapContext_t type
# 
class mbgl_MapContext_t(ctypes.Structure):
  pass

# mbgl_MapContext_init
#
# SEE : int mbgl_MapContext_init(mbgl_View_t* view, mbgl_DefaultFileSource_t* fileSource, mbgl_MapData_t* mapData, mbgl_MapContext_t** out);
# 
mbgl_MapContext_init = _dll.mbgl_MapContext_init
mbgl_MapContext_init.argtypes = [ ctypes.POINTER(mbgl_View_t), ctypes.POINTER(mbgl_DefaultFileSource_t), ctypes.POINTER(mbgl_MapData_t), ctypes.POINTER(ctypes.POINTER(mbgl_MapContext_t)) ]
mbgl_MapContext_init.restype = ctypes.c_int

# mbgl_MapContext_close
#
# SEE : int mbgl_MapContext_close(mbgl_MapContext_t* mapContext);
# 
mbgl_MapContext_close = _dll.mbgl_MapContext_close
mbgl_MapContext_close.argtypes = [ ctypes.POINTER(mbgl_MapContext_t) ]
mbgl_MapContext_close.restype = ctypes.c_int

# mbgl_MapContext_setStyleURL
#
# SEE : void mbgl_MapContext_setStyleURL(mbgl_MapContext_t* mapContext, const char* styleUrl);
# 
mbgl_MapContext_setStyleURL = _dll.mbgl_MapContext_setStyleURL
mbgl_MapContext_setStyleURL.argtypes = [ ctypes.POINTER(mbgl_MapContext_t), ctypes.c_char_p ]
mbgl_MapContext_setStyleURL.restype = None

# mbgl_MapContext_triggerUpdate
#
# SEE : void mbgl_MapContext_triggerUpdate(mbgl_MapContext_t* mapContext,  mbgl_Transform_t* transform, int flags);
# 
mbgl_MapContext_triggerUpdate = _dll.mbgl_MapContext_triggerUpdate
mbgl_MapContext_triggerUpdate.argtypes = [ ctypes.POINTER(mbgl_MapContext_t), ctypes.POINTER(mbgl_Transform_t), ctypes.c_int ]
mbgl_MapContext_triggerUpdate.restype = None


# mbgl_MapThreadContext_t type
# 
class mbgl_MapThreadContext_t(ctypes.Structure):
  pass

# mbgl_MapThreadContext_init
#
# SEE : int mbgl_MapThreadContext_init(mbgl_MapThreadContext_t** out);
# 
mbgl_MapThreadContext_init = _dll.mbgl_MapThreadContext_init
mbgl_MapThreadContext_init.argtypes = [ ctypes.POINTER(ctypes.POINTER(mbgl_MapThreadContext_t)) ]
mbgl_MapThreadContext_init.restype = ctypes.c_int

# mbgl_MapThreadContext_close
#
# SEE : int mbgl_MapThreadContext_close(mbgl_MapThreadContext_t* mapThreadContext);
# 
mbgl_MapThreadContext_close = _dll.mbgl_MapThreadContext_close
mbgl_MapThreadContext_close.argtypes = [ ctypes.POINTER(mbgl_MapThreadContext_t) ]
mbgl_MapThreadContext_close.restype = ctypes.c_int

# mbgl_MapThreadContext_process
#
# SEE : void mbgl_MapThreadContext_process(mbgl_MapThreadContext_t* mapThreadContext);
# 
mbgl_MapThreadContext_process = _dll.mbgl_MapThreadContext_process
mbgl_MapThreadContext_process.argtypes = [ ctypes.POINTER(mbgl_MapThreadContext_t) ]
mbgl_MapThreadContext_process.restype = None


# mbgl_MapImmediate_t type
# 
class mbgl_MapImmediate_t(ctypes.Structure):
  pass

# mbgl_MapImmediate_init
#
# SEE : int mbgl_MapImmediate_init(mbgl_MapData_t* mapData, mbgl_MapContext_t* mapContext, mbgl_Transform_t* transform, mbgl_MapImmediate_t** out);
# 
mbgl_MapImmediate_init = _dll.mbgl_MapImmediate_init
mbgl_MapImmediate_init.argtypes = [ ctypes.POINTER(mbgl_MapData_t), ctypes.POINTER(mbgl_MapContext_t), ctypes.POINTER(mbgl_Transform_t), ctypes.POINTER(ctypes.POINTER(mbgl_MapImmediate_t)) ]
mbgl_MapImmediate_init.restype = ctypes.c_int

# mbgl_MapImmediate_close
#
# SEE : int mbgl_MapImmediate_close(mbgl_MapImmediate_t* map);
# 
mbgl_MapImmediate_close = _dll.mbgl_MapImmediate_close
mbgl_MapImmediate_close.argtypes = [ ctypes.POINTER(mbgl_MapImmediate_t) ]
mbgl_MapImmediate_close.restype = ctypes.c_int

# mbgl_MapImmediate_render
#
# SEE : void mbgl_MapImmediate_render(mbgl_MapImmediate_t* map, mbgl_View_t* view);
# 
mbgl_MapImmediate_render = _dll.mbgl_MapImmediate_render
mbgl_MapImmediate_render.argtypes = [ ctypes.POINTER(mbgl_MapImmediate_t), ctypes.POINTER(mbgl_View_t) ]
mbgl_MapImmediate_render.restype = None

# mbgl_MapImmediate_resize
#
# SEE : void mbgl_MapImmediate_resize(mbgl_MapImmediate_t* map, mbgl_View_t* view);
# 
mbgl_MapImmediate_resize = _dll.mbgl_MapImmediate_resize
mbgl_MapImmediate_resize.argtypes = [ ctypes.POINTER(mbgl_MapImmediate_t), ctypes.POINTER(mbgl_View_t) ]
mbgl_MapImmediate_resize.restype = None

# mbgl_MapImmediate_update
#
# SEE : void mbgl_MapImmediate_update(mbgl_MapImmediate_t* map);
# 
mbgl_MapImmediate_update = _dll.mbgl_MapImmediate_update
mbgl_MapImmediate_update.argtypes = [ ctypes.POINTER(mbgl_MapImmediate_t) ]
mbgl_MapImmediate_update.restype = None
