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

#ifdef __cplusplus
}
#endif

#endif
