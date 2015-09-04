#include <mbgl/mbgl_c.h>
#include <mbgl/mbgl.hpp>
#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

#include <mbgl/c_api/c_api_view.hpp>

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
int mbgl_DefaultFileSource_init(mbgl_SQLiteCache_t* cache, mbgl_DefaultFileSource_t** out) {
  mbgl_DefaultFileSource_t* result = (mbgl_DefaultFileSource_t*)malloc(sizeof(*result));
  result->fileSource = new mbgl::DefaultFileSource(cache->cache);
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
