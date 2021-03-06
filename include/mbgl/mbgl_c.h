#ifndef MBGL_MAIN_C_H
#define MBGL_MAIN_C_H

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef MBGL_C_EXPORTS
    #ifdef __GNUC__
    #define MBGL_C_EXPORT __attribute__ ((dllexport))
    #else
    #define MBGL_C_EXPORT __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
    #define MBGL_C_EXPORT __attribute__ ((dllimport))
    #else
    #define MBGL_C_EXPORT __declspec(dllimport)
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define MBGL_C_EXPORT __attribute__ ((visibility ("default")))
    #else
    #define MBGL_C_EXPORT
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

/*****************************************************************************/

typedef struct mbgl_View_t mbgl_View_t;

/*****************************************************************************/

typedef struct mbgl_CApiView_t mbgl_CApiView_t;

struct mbgl_CApiView_Callbacks_t
{
  float (*getPixelRatio)(mbgl_CApiView_t* view, void* userdata);
  void (*getSize)(mbgl_CApiView_t* view, void* userdata, uint16_t* width, uint16_t* height);
  void (*getFramebufferSize)(mbgl_CApiView_t* view, void* userdata, uint16_t* fbWidth, uint16_t* fbHeight);
  void (*activate)(mbgl_CApiView_t* view, void* userdata);
  void (*deactivate)(mbgl_CApiView_t* view, void* userdata);
  void (*notify)(mbgl_CApiView_t* view, void* userdata);
  void (*invalidate)(mbgl_CApiView_t* view, void* userdata);
  void (*beforeRender)(mbgl_CApiView_t* view, void* userdata);
  void (*afterRender)(mbgl_CApiView_t* view, void* userdata);
};

MBGL_C_EXPORT
int mbgl_CApiView_init(mbgl_CApiView_Callbacks_t* callbacks, void* userdata, mbgl_CApiView_t** out);

MBGL_C_EXPORT
int mbgl_CApiView_close(mbgl_CApiView_t* view);

/*****************************************************************************/

typedef struct mbgl_GLFWView_t mbgl_GLFWView_t;

MBGL_C_EXPORT
int mbgl_GLFWView_init(int fullscreen, int benchmark, mbgl_GLFWView_t** out);

MBGL_C_EXPORT
int mbgl_GLFWView_close(mbgl_GLFWView_t* view);

MBGL_C_EXPORT
void mbgl_GLFWView_setChangeStyleCallback(mbgl_GLFWView_t* view, void (*callback)(mbgl_GLFWView_t*, void*), void* userdata);

MBGL_C_EXPORT
void mbgl_GLFWView_setShouldClose(mbgl_GLFWView_t* view);

MBGL_C_EXPORT
void mbgl_GLFWView_setWindowTitle(mbgl_GLFWView_t* view, const char* title);

MBGL_C_EXPORT
void mbgl_GLFWView_run(mbgl_GLFWView_t* view);

MBGL_C_EXPORT
void mbgl_GLFWView_report(mbgl_GLFWView_t* view, float duration);

/*****************************************************************************/

// TODO typedef mbgl_Cache_t mbgl_Cache_t;
typedef struct mbgl_SQLiteCache_t mbgl_SQLiteCache_t;

MBGL_C_EXPORT
int mbgl_SQLiteCache_init(const char* path, mbgl_SQLiteCache_t** out);

MBGL_C_EXPORT
int mbgl_SQLiteCache_close(mbgl_SQLiteCache_t* sqliteCache);

/*****************************************************************************/

// TODO typedef mbgl_FileSource_t mbgl_FileSource_t;
typedef struct mbgl_DefaultFileSource_t mbgl_DefaultFileSource_t;

// Replace mbgl_SQLiteCache_t with mbgl_Cache_t
MBGL_C_EXPORT
int mbgl_DefaultFileSource_init(mbgl_SQLiteCache_t* cache, const char* assetRoot, mbgl_DefaultFileSource_t** out);

MBGL_C_EXPORT
int mbgl_DefaultFileSource_close(mbgl_DefaultFileSource_t* defaultFileSource);

MBGL_C_EXPORT
void mbgl_DefaultFileSource_setAccessToken(mbgl_DefaultFileSource_t* fileSource, const char* accessToken);

MBGL_C_EXPORT
const char* mbgl_DefaultFileSource_getAccessToken(mbgl_DefaultFileSource_t* fileSource);

/*****************************************************************************/

typedef struct mbgl_Map_t mbgl_Map_t;

// Replace mbgl_DefaultFileSource_t with mbgl_FileSource_t
MBGL_C_EXPORT
int mbgl_Map_init(mbgl_View_t* view, mbgl_DefaultFileSource_t* fileSource, mbgl_Map_t** out);

MBGL_C_EXPORT
int mbgl_Map_close(mbgl_Map_t* map);

// Styling
MBGL_C_EXPORT
void mbgl_Map_setStyleURL(mbgl_Map_t* map, const char* url);

MBGL_C_EXPORT
void mbgl_Map_setStyleJSON(mbgl_Map_t* map, const char* json);

MBGL_C_EXPORT
const char* mbgl_Map_getStyleURL(mbgl_Map_t* map);

MBGL_C_EXPORT
const char* mbgl_Map_getStyleJSON(mbgl_Map_t* map);

// Scale
MBGL_C_EXPORT
void mbgl_Map_setLatLngZoom(mbgl_Map_t* map, double latitude, double longitude, double zoom);

// Rotation
MBGL_C_EXPORT
void mbgl_Map_rotateBy(mbgl_Map_t* map, double sx, double sy, double ex, double ey);

MBGL_C_EXPORT
void mbgl_Map_setBearing(mbgl_Map_t* map, double degrees);

MBGL_C_EXPORT
double mbgl_Map_getBearing(mbgl_Map_t* map);

MBGL_C_EXPORT
void mbgl_Map_resetNorth(mbgl_Map_t* map);

// Debug
MBGL_C_EXPORT
void mbgl_Map_setDebug(mbgl_Map_t* map, int value);

MBGL_C_EXPORT
void mbgl_Map_toggleDebug(mbgl_Map_t* map);

MBGL_C_EXPORT
int mbgl_Map_getDebug(mbgl_Map_t* map);

MBGL_C_EXPORT
void mbgl_Map_setCollisionDebug(mbgl_Map_t* map, int value);

MBGL_C_EXPORT
void mbgl_Map_toggleCollisionDebug(mbgl_Map_t* map);

MBGL_C_EXPORT
int mbgl_Map_getCollisionDebug(mbgl_Map_t* map);

MBGL_C_EXPORT
int mbgl_Map_isFullyLoaded(mbgl_Map_t* map);

/*****************************************************************************/

typedef struct mbgl_Transform_t mbgl_Transform_t;

MBGL_C_EXPORT
int mbgl_Transform_init(mbgl_View_t* view, mbgl_Transform_t** out);

MBGL_C_EXPORT
int mbgl_Transform_close(mbgl_Transform_t* transform);

MBGL_C_EXPORT
int mbgl_Transform_resize(mbgl_Transform_t* transform, uint16_t width, uint16_t height);

// Position
MBGL_C_EXPORT
void mbgl_Transform_setLatLng(mbgl_Transform_t* transform, double latitude, double longitude);

MBGL_C_EXPORT
void mbgl_Transform_getLatLng(mbgl_Transform_t* transform, double* outLatitude, double* outLongitude);

// Zoom
MBGL_C_EXPORT
void mbgl_Transform_setZoom(mbgl_Transform_t* transform, double zoom);

MBGL_C_EXPORT
double mbgl_Transform_getZoom(mbgl_Transform_t* transform);

// Position + zoom
MBGL_C_EXPORT
void mbgl_Transform_setLatLngZoom(mbgl_Transform_t* transform, double latitude, double longitude, double zoom);

// Angle
MBGL_C_EXPORT
void mbgl_Transform_setAngle(mbgl_Transform_t* transform, double angle);

MBGL_C_EXPORT
double mbgl_Transform_getAngle(mbgl_Transform_t* transform);

// Pitch
MBGL_C_EXPORT
void mbgl_Transform_setPitch(mbgl_Transform_t* transform, double pitch);

MBGL_C_EXPORT
double mbgl_Transform_getPitch(mbgl_Transform_t* transform);

/*****************************************************************************/

typedef struct mbgl_MapData_t mbgl_MapData_t;

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_MapData_init(int mode, float pixelRatio, mbgl_MapData_t** out);

MBGL_C_EXPORT
int mbgl_MapData_close(mbgl_MapData_t* mapData);

MBGL_C_EXPORT
int mbgl_MapData_getDebug(mbgl_MapData_t* mapData);

MBGL_C_EXPORT
void mbgl_MapData_setDebug(mbgl_MapData_t* mapData, int value);

/*****************************************************************************/

typedef struct mbgl_MapContext_t mbgl_MapContext_t;

/*****************************************************************************/

// Replace mbgl_DefaultFileSource_t with mbgl_FileSource_t
MBGL_C_EXPORT
int mbgl_MapContext_init(mbgl_View_t* view, mbgl_DefaultFileSource_t* fileSource, mbgl_MapData_t* mapData, mbgl_MapContext_t** out);

MBGL_C_EXPORT
int mbgl_MapContext_close(mbgl_MapContext_t* mapContext);

MBGL_C_EXPORT
void mbgl_MapContext_setStyleURL(mbgl_MapContext_t* mapContext, const char* styleUrl);

MBGL_C_EXPORT
void mbgl_MapContext_setStyleJSON(mbgl_MapContext_t* mapContext, const char* json);

MBGL_C_EXPORT
void mbgl_MapContext_triggerUpdate(mbgl_MapContext_t* mapContext, mbgl_Transform_t* transform, int flags);

MBGL_C_EXPORT
void mbgl_MapContext_cleanup(mbgl_MapContext_t* mapContext);

/*****************************************************************************/

typedef struct mbgl_MapThreadContext_t mbgl_MapThreadContext_t;

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_MapThreadContext_init(mbgl_MapThreadContext_t** out);

MBGL_C_EXPORT
int mbgl_MapThreadContext_close(mbgl_MapThreadContext_t* mapThreadContext);

MBGL_C_EXPORT
void mbgl_MapThreadContext_process(mbgl_MapThreadContext_t* mapThreadContext);

/*****************************************************************************/

typedef struct mbgl_MapImmediate_t mbgl_MapImmediate_t;

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_MapImmediate_init(mbgl_MapData_t* mapData, mbgl_MapContext_t* mapContext, mbgl_Transform_t* transform, mbgl_MapImmediate_t** out);

MBGL_C_EXPORT
int mbgl_MapImmediate_close(mbgl_MapImmediate_t* map);

MBGL_C_EXPORT
int mbgl_MapImmediate_render(mbgl_MapImmediate_t* map, mbgl_View_t* view);

MBGL_C_EXPORT
void mbgl_MapImmediate_resize(mbgl_MapImmediate_t* map, mbgl_View_t* view);

MBGL_C_EXPORT
void mbgl_MapImmediate_update(mbgl_MapImmediate_t* map);

/*****************************************************************************/

MBGL_C_EXPORT
int mbgl_gl_initializeExtensions();

/*****************************************************************************/

MBGL_C_EXPORT
void mbgl_Util_setSpriteAtlasSize(int size);
MBGL_C_EXPORT
int mbgl_Util_getSpriteAtlasSize();

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
