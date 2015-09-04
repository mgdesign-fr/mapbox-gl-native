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
  float (*getPixelRatio)(mbgl_CApiView_t*, void*);
  void (*getSize)(mbgl_CApiView_t*, void*, uint16_t*, uint16_t*);
  void (*getFramebufferSize)(mbgl_CApiView_t*, void*, uint16_t*, uint16_t*);
  void (*activate)(mbgl_CApiView_t*, void*);
  void (*deactivate)(mbgl_CApiView_t*, void*);
  void (*notify)(mbgl_CApiView_t*, void*);
  void (*invalidate)(mbgl_CApiView_t*, void*);
  void (*swap)(mbgl_CApiView_t*, void*);
};

MBGL_C_EXPORT
int mbgl_CApiView_init(mbgl_CApiView_t** out, mbgl_CApiView_Callbacks_t* callbacks, void* userdata);

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

typedef struct mbgl_SQLiteCache_t mbgl_SQLiteCache_t;

MBGL_C_EXPORT
int mbgl_SQLiteCache_init(const char* path, mbgl_SQLiteCache_t** out);

MBGL_C_EXPORT
int mbgl_SQLiteCache_close(mbgl_SQLiteCache_t* sqliteCache);

/*****************************************************************************/

typedef struct mbgl_DefaultFileSource_t mbgl_DefaultFileSource_t;

MBGL_C_EXPORT
int mbgl_DefaultFileSource_init(mbgl_SQLiteCache_t* cache, mbgl_DefaultFileSource_t** out);

MBGL_C_EXPORT
int mbgl_DefaultFileSource_close(mbgl_DefaultFileSource_t* defaultFileSource);

MBGL_C_EXPORT
void mbgl_DefaultFileSource_setAccessToken(mbgl_DefaultFileSource_t* fileSource, const char* accessToken);

MBGL_C_EXPORT
const char* mbgl_DefaultFileSource_getAccessToken(mbgl_DefaultFileSource_t* fileSource);

/*****************************************************************************/

typedef struct mbgl_Map_t mbgl_Map_t;

MBGL_C_EXPORT
int mbgl_Map_init(mbgl_View_t* view, mbgl_DefaultFileSource_t* fileSource, mbgl_Map_t** out);

MBGL_C_EXPORT
int mbgl_Map_close(mbgl_Map_t* map);

// Styling
MBGL_C_EXPORT
void mbgl_Map_setStyleURL(mbgl_Map_t* map, const char* url);

MBGL_C_EXPORT
void mbgl_Map_setStyleJSON(mbgl_Map_t* map, const char* json/*TODO, const char* base = ""*/);

MBGL_C_EXPORT
const char* mbgl_Map_getStyleURL(mbgl_Map_t* map);

MBGL_C_EXPORT
const char* mbgl_Map_getStyleJSON(mbgl_Map_t* map);

// Scale
// TODO other functions
MBGL_C_EXPORT
void mbgl_Map_setLatLngZoom(mbgl_Map_t* map, double latitude, double longitude, double zoom/*TODO , const Duration& = Duration::zero()*/);

// Rotation
MBGL_C_EXPORT
void mbgl_Map_rotateBy(mbgl_Map_t* map, double sx, double sy, double ex, double ey/*TODO, const Duration& = Duration::zero()*/);

MBGL_C_EXPORT
void mbgl_Map_setBearing(mbgl_Map_t* map, double degrees/*TODO, const Duration& = Duration::zero()*/);

/*TODO MBGL_C_EXPORT
void mbgl_Map_setBearing(mbgl_Map_t* map, double degrees, double cx, double cy);*/

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
void mbgl_Map_setNeedsRepaint(mbgl_Map_t* map);

MBGL_C_EXPORT
void mbgl_Map_setCollisionDebug(mbgl_Map_t* map, int value);

MBGL_C_EXPORT
void mbgl_Map_toggleCollisionDebug(mbgl_Map_t* map);

MBGL_C_EXPORT
int mbgl_Map_getCollisionDebug(mbgl_Map_t* map);

MBGL_C_EXPORT
int mbgl_Map_isFullyLoaded(mbgl_Map_t* map);

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
