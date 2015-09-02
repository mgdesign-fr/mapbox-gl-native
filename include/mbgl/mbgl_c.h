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

/*****************************************************************************/

typedef struct mbgl_GLFWView_t mbgl_GLFWView_t;

MBGL_C_EXPORT
int mbgl_GLFWView_init(int fullscreen, int benchmark, mbgl_GLFWView_t** out);

MBGL_C_EXPORT
int mbgl_GLFWView_close(mbgl_GLFWView_t* view);

MBGL_C_EXPORT
int mbgl_GLFWView_setChangeStyleCallback(mbgl_GLFWView_t* view, void (*callback)(), void* userdata);

MBGL_C_EXPORT
int mbgl_GLFWView_setShouldClose(mbgl_GLFWView_t* view);

MBGL_C_EXPORT
int mbgl_GLFWView_setWindowTitle(mbgl_GLFWView_t* view, const char* title);

MBGL_C_EXPORT
int mbgl_GLFWView_run(mbgl_GLFWView_t* view);

MBGL_C_EXPORT
int mbgl_GLFWView_report(mbgl_GLFWView_t* view);

/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// SQLiteCache type wrapper
typedef struct mbgl_SQLiteCache_t mbgl_SQLiteCache_t;

int mbgl_SQLiteCache_init(const char* path, mbgl_SQLiteCache_t** out);
int mbgl_SQLiteCache_close(mbgl_SQLiteCache_t* sqliteCache);

///////////////////////////////////////////////////////////////////////////////
// DefaultFileSource type wrapper
typedef struct mbgl_DefaultFileSource_t mbgl_DefaultFileSource_t;

int mbgl_DefaultFileSource_init(mbgl_SQLiteCache_t* cache, mbgl_DefaultFileSource_t** out);
int mbgl_DefaultFileSource_close(mbgl_DefaultFileSource_t* defaultFileSource);

void mbgl_DefaultFileSource_setAccessToken(mbgl_DefaultFileSource_t* fileSource, const char* accessToken);
const char* mbgl_DefaultFileSource_getAccessToken(mbgl_DefaultFileSource_t* fileSource);

///////////////////////////////////////////////////////////////////////////////
// Map type wrapper
typedef struct mbgl_Map_t mbgl_Map_t;

int mbgl_Map_init(mbgl_GLFWView_t* view, mbgl_DefaultFileSource_t* fileSource, mbgl_Map_t** out);
int mbgl_Map_close(mbgl_Map_t* map);

// Styling
void mbgl_Map_setStyleURL(mbgl_Map_t* map, const char* url);
void mbgl_Map_setStyleJSON(mbgl_Map_t* map, const char* json, const char* base = "");
const char* mbgl_Map_getStyleURL(mbgl_Map_t* map) const;
const char* mbgl_Map_getStyleJSON(mbgl_Map_t* map) const;

// Debug
void mbgl_Map_setDebug(mbgl_Map_t* map, int value);
void mbgl_Map_toggleDebug(mbgl_Map_t* map);
int mbgl_Map_getDebug(mbgl_Map_t* map) const;
void mbgl_Map_setNeedsRepaint(mbgl_Map_t* map);
void mbgl_Map_setCollisionDebug(mbgl_Map_t* map, int value);
void mbgl_Map_toggleCollisionDebug(mbgl_Map_t* map);
int mbgl_Map_getCollisionDebug(mbgl_Map_t* map) const;
int mbgl_Map_isFullyLoaded(mbgl_Map_t* map) const;

#ifdef __cplusplus
}
#endif

#endif
