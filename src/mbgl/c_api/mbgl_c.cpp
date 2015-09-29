#include <mbgl/mbgl_c.h>
#include <mbgl/mbgl.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

#include <mbgl/c_api/c_api_view.hpp>
#include <mbgl/c_api/map_immediate.hpp>

#include <cassert>

/*****************************************************************************/

int mbgl_CApiView_init(mbgl_CApiView_Callbacks_t* callbacks, void* userdata, mbgl_CApiView_t** out) {
  mbgl_CApiView_t* result = (mbgl_CApiView_t*)malloc(sizeof(*result));
  result->callbacks = callbacks;
  result->userdata = userdata;
  result->view = new mbgl::CApiView(result);
  *out = result;
  return 0;
}

int mbgl_CApiView_close(mbgl_CApiView_t* view) {
  if (view != 0) {
    delete view->view;
    view->view = 0;
    free(view);
    
    view->callbacks = {0};
    view->userdata = 0;
  }
  return 0;
}

/*****************************************************************************/

struct mbgl_GLFWView_t {
  union {
    mbgl_View_t base;
    GLFWView* view;
  };
};

/*****************************************************************************/

int mbgl_GLFWView_init(int fullscreen, int benchmark, mbgl_GLFWView_t** out)
{
  mbgl_GLFWView_t* result = (mbgl_GLFWView_t*)malloc(sizeof(*result));
  result->view = new GLFWView(fullscreen, benchmark);
  *out = result;
  return 0;
}

int mbgl_GLFWView_close(mbgl_GLFWView_t* view)
{
  if (view != NULL) {
    delete view->view;
    view->view = NULL;
    free(view);
  }
  return 0;
}

MBGL_C_EXPORT
void mbgl_GLFWView_setChangeStyleCallback(mbgl_GLFWView_t* view, void (*callback)(mbgl_GLFWView_t* view, void*), void* userdata) {
  view->view->setChangeStyleCallback([view, callback, userdata]() {
    callback(view, userdata);
  });
}

MBGL_C_EXPORT
void mbgl_GLFWView_setShouldClose(mbgl_GLFWView_t* view) {
  view->view->setShouldClose();
}

MBGL_C_EXPORT
void mbgl_GLFWView_setWindowTitle(mbgl_GLFWView_t* view, const char* title) {
  view->view->setWindowTitle(std::string(title));
}

MBGL_C_EXPORT
void mbgl_GLFWView_run(mbgl_GLFWView_t* view) {
  view->view->run();
}

MBGL_C_EXPORT
void mbgl_GLFWView_report(mbgl_GLFWView_t* view, float duration) {
  view->view->report(duration);
}

/*****************************************************************************/

struct mbgl_SQLiteCache_t {
  mbgl::SQLiteCache* cache;
};

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_SQLiteCache_init(const char* path, mbgl_SQLiteCache_t** out) {
  mbgl_SQLiteCache_t* result = (mbgl_SQLiteCache_t*)malloc(sizeof(*result));
  result->cache = new mbgl::SQLiteCache(std::string(path));
  *out = result;
  return 0;
}

MBGL_C_EXPORT
int mbgl_SQLiteCache_close(mbgl_SQLiteCache_t* sqliteCache) {
  if(sqliteCache != 0) {
    delete sqliteCache->cache;
    sqliteCache->cache = 0;
    free(sqliteCache);
  }
  return 0;
}

/*****************************************************************************/

struct mbgl_DefaultFileSource_t {
  mbgl::DefaultFileSource* fileSource;
};

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_DefaultFileSource_init(mbgl_SQLiteCache_t* cache, const char* assetRoot, mbgl_DefaultFileSource_t** out) {
  mbgl_DefaultFileSource_t* result = (mbgl_DefaultFileSource_t*)malloc(sizeof(*result));
  result->fileSource = new mbgl::DefaultFileSource(cache->cache, assetRoot);
  *out = result;
  return 0;
}

MBGL_C_EXPORT
int mbgl_DefaultFileSource_close(mbgl_DefaultFileSource_t* defaultFileSource) {
  if(defaultFileSource != 0) {
    delete defaultFileSource->fileSource;
    defaultFileSource->fileSource = 0;
    free(defaultFileSource);
  }
  return 0;
}

MBGL_C_EXPORT
void mbgl_DefaultFileSource_setAccessToken(mbgl_DefaultFileSource_t* fileSource, const char* accessToken) {
  fileSource->fileSource->setAccessToken(std::string(accessToken));
}

MBGL_C_EXPORT
const char* mbgl_DefaultFileSource_getAccessToken(mbgl_DefaultFileSource_t* fileSource) {
  return fileSource->fileSource->getAccessToken().c_str();
}

/*****************************************************************************/

struct mbgl_Map_t {
  mbgl::Map* map;
};

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_Map_init(mbgl_View_t* view, mbgl_DefaultFileSource_t* fileSource, mbgl_Map_t** out) {
  mbgl_Map_t* result = (mbgl_Map_t*)malloc(sizeof(*result));
  result->map = new mbgl::Map(*view->view, *fileSource->fileSource);
  *out = result;
  return 0;
}

MBGL_C_EXPORT
int mbgl_Map_close(mbgl_Map_t* map) {
  if(map != 0) {
    delete map->map;
    map->map = 0;
    free(map);
  }
  return 0;
}

MBGL_C_EXPORT
void mbgl_Map_setStyleURL(mbgl_Map_t* map, const char* url) {
  map->map->setStyleURL(std::string(url));
}

MBGL_C_EXPORT
void mbgl_Map_setStyleJSON(mbgl_Map_t* map, const char* json) {
  map->map->setStyleJSON(std::string(json));
}

MBGL_C_EXPORT
const char* mbgl_Map_getStyleURL(mbgl_Map_t* map) {
  return map->map->getStyleURL().c_str();
}

MBGL_C_EXPORT
const char* mbgl_Map_getStyleJSON(mbgl_Map_t* map) {
  return map->map->getStyleJSON().c_str();
}

MBGL_C_EXPORT
void mbgl_Map_setLatLngZoom(mbgl_Map_t* map, double latitude, double longitude, double zoom/*TODO , const Duration& = Duration::zero()*/) {
  map->map->setLatLngZoom(mbgl::LatLng(latitude, longitude), zoom);
}

MBGL_C_EXPORT
void mbgl_Map_rotateBy(mbgl_Map_t* map, double sx, double sy, double ex, double ey/*TODO, const Duration& = Duration::zero()*/) {
  map->map->rotateBy(sx, sy, ex, ey);
}

MBGL_C_EXPORT
void mbgl_Map_setBearing(mbgl_Map_t* map, double degrees/*TODO, const Duration& = Duration::zero()*/) {
  map->map->setBearing(degrees);
}

MBGL_C_EXPORT
double mbgl_Map_getBearing(mbgl_Map_t* map) {
  return map->map->getBearing();
}

MBGL_C_EXPORT
void mbgl_Map_resetNorth(mbgl_Map_t* map) {
  map->map->resetNorth();
}

MBGL_C_EXPORT
void mbgl_Map_setDebug(mbgl_Map_t* map, int value) {
  map->map->setDebug(value);
}

MBGL_C_EXPORT
void mbgl_Map_toggleDebug(mbgl_Map_t* map) {
  map->map->toggleDebug();
}

MBGL_C_EXPORT
int mbgl_Map_getDebug(mbgl_Map_t* map) {
  return map->map->getDebug();
}

MBGL_C_EXPORT
void mbgl_Map_setNeedsRepaint(mbgl_Map_t* map) {
  return map->map->setNeedsRepaint();
}

MBGL_C_EXPORT
void mbgl_Map_setCollisionDebug(mbgl_Map_t* map, int value) {
  map->map->setCollisionDebug(value);
}

MBGL_C_EXPORT
void mbgl_Map_toggleCollisionDebug(mbgl_Map_t* map) {
  map->map->toggleCollisionDebug();
}

MBGL_C_EXPORT
int mbgl_Map_getCollisionDebug(mbgl_Map_t* map) {
  return map->map->getCollisionDebug();
}

MBGL_C_EXPORT
int mbgl_Map_isFullyLoaded(mbgl_Map_t* map) {
  return map->map->isFullyLoaded();
}

/*****************************************************************************/

struct mbgl_Transform_t {
  mbgl::Transform* transform;
};

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_Transform_init(mbgl_View_t* view, mbgl_Transform_t** out) {
  mbgl_Transform_t* result = (mbgl_Transform_t*)malloc(sizeof(*result));
  result->transform = new mbgl::Transform(*view->view);
  *out = result;
  return 0;
}

MBGL_C_EXPORT
int mbgl_Transform_close(mbgl_Transform_t* transform) {
  if(transform != 0) {
    delete transform->transform;
    transform->transform = 0;
    free(transform);
  }
  return 0;
}

MBGL_C_EXPORT
int mbgl_Transform_resize(mbgl_Transform_t* transform, uint16_t width, uint16_t height) {
  return transform->transform->resize({{ width, height }});
}

// Position
MBGL_C_EXPORT
void mbgl_Transform_setLatLng(mbgl_Transform_t* transform, double latitude, double longitude) {
  mbgl::CameraOptions defaultCameraOptions;
  transform->transform->setLatLng(mbgl::LatLng(latitude,longitude), defaultCameraOptions);
}

MBGL_C_EXPORT
void mbgl_Transform_getLatLng(mbgl_Transform_t* transform, double* outLatitude, double* outLongitude) {
  mbgl::LatLng latLng = transform->transform->getLatLng();
  *outLatitude = latLng.latitude;
  *outLongitude = latLng.longitude;
}

// Zoom
MBGL_C_EXPORT
void mbgl_Transform_setZoom(mbgl_Transform_t* transform, double zoom) {
  transform->transform->setZoom(zoom);
}

MBGL_C_EXPORT
double mbgl_Transform_getZoom(mbgl_Transform_t* transform) {
  return transform->transform->getZoom();
}

// Position + zoom
MBGL_C_EXPORT
void mbgl_Transform_setLatLngZoom(mbgl_Transform_t* transform, double latitude, double longitude, double zoom) {
  mbgl::CameraOptions defaultCameraOptions;
  transform->transform->setLatLngZoom(mbgl::LatLng(latitude,longitude), zoom, defaultCameraOptions);
}

// Angle
MBGL_C_EXPORT
void mbgl_Transform_setAngle(mbgl_Transform_t* transform, double angle) {
  mbgl::CameraOptions defaultCameraOptions;
  transform->transform->setAngle(angle, defaultCameraOptions);
}

MBGL_C_EXPORT
double mbgl_Transform_getAngle(mbgl_Transform_t* transform) {
  return transform->transform->getAngle();
}

// Pitch
MBGL_C_EXPORT
void mbgl_Transform_setPitch(mbgl_Transform_t* transform, double pitch) {
  mbgl::CameraOptions defaultCameraOptions;
  transform->transform->setPitch(pitch, defaultCameraOptions);
}

MBGL_C_EXPORT
double mbgl_Transform_getPitch(mbgl_Transform_t* transform) {
  return transform->transform->getPitch();
}

/*****************************************************************************/

struct mbgl_MapData_t {
  mbgl::MapData* mapData;
};

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_MapData_init(int mode, float pixelRatio, mbgl_MapData_t** out) {
  mbgl_MapData_t* result = (mbgl_MapData_t*)malloc(sizeof(*result));
  result->mapData = new mbgl::MapData((mbgl::MapMode)mode, pixelRatio);
  *out = result;
  return 0;
}

MBGL_C_EXPORT
int mbgl_MapData_close(mbgl_MapData_t* mapData) {
  if(mapData != 0) {
    delete mapData->mapData;
    mapData->mapData = 0;
    free(mapData);
  }
  return 0;
}


MBGL_C_EXPORT
int mbgl_MapData_getDebug(mbgl_MapData_t* mapData) {
  return mapData->mapData->getDebug();
}

MBGL_C_EXPORT
void mbgl_MapData_setDebug(mbgl_MapData_t* mapData, int value) {
  mapData->mapData->setDebug(!!value);
}

/*****************************************************************************/

struct mbgl_MapContext_t {
  mbgl::MapContext* mapContext;
};

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_MapContext_init(mbgl_View_t* view, mbgl_DefaultFileSource_t* fileSource, mbgl_MapData_t* mapData, mbgl_MapContext_t** out) {
  mbgl_MapContext_t* result = (mbgl_MapContext_t*)malloc(sizeof(*result));
  result->mapContext = new mbgl::MapContext(*view->view, *fileSource->fileSource, *mapData->mapData);
  *out = result;
  return 0;
}

MBGL_C_EXPORT
int mbgl_MapContext_close(mbgl_MapContext_t* mapContext) {
  if(mapContext != 0) {
    delete mapContext->mapContext;
    mapContext->mapContext = 0;
    free(mapContext);
  }
  return 0;
}

MBGL_C_EXPORT
void mbgl_MapContext_setStyleURL(mbgl_MapContext_t* mapContext, const char* styleUrl) {
  mapContext->mapContext->setStyleURL(std::string(styleUrl));
}

MBGL_C_EXPORT
void mbgl_MapContext_setStyleJSON(mbgl_MapContext_t* mapContext, const char* json) {
  mapContext->mapContext->setStyleJSON(json, "");
}


MBGL_C_EXPORT
void mbgl_MapContext_triggerUpdate(mbgl_MapContext_t* mapContext,  mbgl_Transform_t* transform, int flags) {
  mapContext->mapContext->triggerUpdate(transform->transform->getState(), (mbgl::Update)flags);
}

MBGL_C_EXPORT
void mbgl_MapContext_cleanup(mbgl_MapContext_t* mapContext) {
  mapContext->mapContext->cleanup();
}

/*****************************************************************************/

struct mbgl_MapThreadContext_t {
  mbgl::MapThreadContext* mapThreadContext;
};

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_MapThreadContext_init(mbgl_MapThreadContext_t** out) {
  mbgl_MapThreadContext_t* result = (mbgl_MapThreadContext_t*)malloc(sizeof(*result));
  result->mapThreadContext = new mbgl::MapThreadContext();
  *out = result;
  return 0;
}

MBGL_C_EXPORT
int mbgl_MapThreadContext_close(mbgl_MapThreadContext_t* mapThreadContext) {
  if(mapThreadContext != 0) {
    delete mapThreadContext->mapThreadContext;
    mapThreadContext->mapThreadContext = 0;
    free(mapThreadContext);
  }
  return 0;
}

MBGL_C_EXPORT
void mbgl_MapThreadContext_process(mbgl_MapThreadContext_t* mapThreadContext) {
  mapThreadContext->mapThreadContext->process();
}

/*****************************************************************************/

struct mbgl_MapImmediate_t {
  mbgl::MapImmediate* map;
};

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_MapImmediate_init(mbgl_MapData_t* mapData, mbgl_MapContext_t* mapContext, mbgl_Transform_t* transform, mbgl_MapImmediate_t** out) {
  mbgl_MapImmediate_t* result = (mbgl_MapImmediate_t*)malloc(sizeof(*result));
  result->map = new mbgl::MapImmediate(mapData->mapData, mapContext->mapContext, transform->transform);
  *out = result;
  return 0;
}

MBGL_C_EXPORT
int mbgl_MapImmediate_close(mbgl_MapImmediate_t* map) {
  if(map != 0) {
    delete map->map;
    map->map = 0;
    free(map);
  }
  return 0;
}

MBGL_C_EXPORT
int mbgl_MapImmediate_render(mbgl_MapImmediate_t* map, mbgl_View_t* view) {
  int result;
  result = map->map->render(view->view);
  return result;
}

MBGL_C_EXPORT
void mbgl_MapImmediate_resize(mbgl_MapImmediate_t* map, mbgl_View_t* view) {
  map->map->resize(view->view);
}

MBGL_C_EXPORT
void mbgl_MapImmediate_update(mbgl_MapImmediate_t* map) {
  map->map->update();
}

/*****************************************************************************/

#include <mbgl/platform/gl.hpp>

typedef void (*glProc)();
static glProc myGetProcAddress(const char* name) {
  // NOTE(nico) to get rid of __stdcall convention
  return (glProc)wglGetProcAddress(name);
}

MBGL_C_EXPORT
int mbgl_gl_initializeExtensions() {
  // TODO(nico) should allow user to pass his function pointer for getProcAddress ?
  mbgl::gl::InitializeExtensions(myGetProcAddress);  
}
