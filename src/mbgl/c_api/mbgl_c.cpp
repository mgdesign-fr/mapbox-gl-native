#include <mbgl/mbgl_c.h>
#include <mbgl/mbgl.hpp>
#include <mbgl/platform/default/glfw_view.hpp>

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

