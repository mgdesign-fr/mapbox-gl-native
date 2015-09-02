#include <mbgl/mbgl_c.h>
#include <mbgl/mbgl.hpp>
#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

/*****************************************************************************/

struct mbgl_GLFWView_t {
	GLFWView* view;
};

/*****************************************************************************/

int mbgl_GLFWView_init(int fullscreen, int benchmark, mbgl_GLFWView_t** out)
{
	mbgl_GLFWView_t* result = (mbgl_GLFWView_t*)malloc(sizeof(*result));
	result->view = new GLFWView;
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
void mbgl_GLFWView_setChangeStyleCallback(mbgl_GLFWView_t* view, void (*callback)(void*), void* userdata) {
	view->view->setChangeStyleCallback([&] {
		callback(userdata);
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

