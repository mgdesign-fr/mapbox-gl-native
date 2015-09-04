#include <mbgl/mbgl.hpp>
#include "../platform/default/default_styles.hpp"
#include <mbgl/util/uv.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/default/settings_json.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

#include <mbgl/platform/default/glfw_view.hpp>          // only to include GLFW
#include <mbgl/c_api/map_immediate.hpp>
#include <mbgl/mbgl_c.h>
#include <mbgl/c_api/c_api_view.hpp>

#include <signal.h>
#include <getopt.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

/*****************************************************************************/

struct GLFWdata {
    GLFWwindow* window;
    int dirty;
    uint16_t width, height;
    uint16_t fbWidth, fbHeight;
};

float getPixelRatio(mbgl_CApiView_t* view, void* userdata) {
    return 1.0;
}

void getSize(mbgl_CApiView_t* view, void* userdata, uint16_t* width, uint16_t* height) {
    GLFWdata* data = (GLFWdata*)userdata;
    *width = data->width;
    *height = data->height;
}

void getFramebufferSize(mbgl_CApiView_t* view, void* userdata, uint16_t* width, uint16_t* height) {
    GLFWdata* data = (GLFWdata*)userdata;
    *width = data->fbWidth;
    *height = data->fbHeight;
}

void activate(mbgl_CApiView_t* view, void* userdata) {
    GLFWdata* data = (GLFWdata*)userdata;
    glfwMakeContextCurrent(data->window);    
}

void deactivate(mbgl_CApiView_t* view, void* userdata) {
    glfwMakeContextCurrent(NULL);    
}

void notify(mbgl_CApiView_t* view, void* userdata) {
    glfwPostEmptyEvent();
}

void invalidate(mbgl_CApiView_t* view, void* userdata) {
    GLFWdata* data = (GLFWdata*)userdata;
    data->dirty = 1;
    glfwPostEmptyEvent();
}

void swap(mbgl_CApiView_t* view, void* userdata) {
    GLFWdata* data = (GLFWdata*)userdata;
    glfwSwapBuffers(data->window);
}

/*****************************************************************************/

namespace {

int shouldCloseWindow = 0;

}

void quit_handler(int) {
    mbgl::Log::Info(mbgl::Event::Setup, "waiting for quit...");
    shouldCloseWindow = 1;
}

int main(int argc, char *argv[]) {
    bool fullscreen = false;
    bool benchmark = false;
    std::string style;
    double latitude = 0, longitude = 0;
    double bearing = 0, zoom = 1;
    bool skipConfig = false;

    const struct option long_options[] = {
        {"fullscreen", no_argument, 0, 'f'},
        {"benchmark", no_argument, 0, 'b'},
        {"style", required_argument, 0, 's'},
        {"lon", required_argument, 0, 'x'},
        {"lat", required_argument, 0, 'y'},
        {"zoom", required_argument, 0, 'z'},
        {"bearing", required_argument, 0, 'r'},
        {0, 0, 0, 0}
    };

    while (true) {
        int option_index = 0;
        int opt = getopt_long(argc, argv, "fbs:", long_options, &option_index);
        if (opt == -1) break;
        switch (opt)
        {
        case 0:
            if (long_options[option_index].flag != 0)
                break;
        case 'f':
            fullscreen = true;
            break;
        case 'b':
            benchmark = true;
            break;
        case 's':
            style = std::string("asset://") + std::string(optarg);
            break;
        case 'x':
            longitude = atof(optarg);
            skipConfig = true;
            break;
        case 'y':
            latitude = atof(optarg);
            skipConfig = true;
            break;
        case 'z':
            zoom = atof(optarg);
            skipConfig = true;
            break;
        case 'r':
            bearing = atof(optarg);
            skipConfig = true;
            break;
        default:
            break;
        }

    }

    /**/

    GLFWdata viewData = {};

    /**/

    if (!glfwInit()) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "failed to initialize glfw");
        exit(1);
    }

    int width=1024, height=768;
    GLFWmonitor *monitor = nullptr;
    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        auto videoMode = glfwGetVideoMode(monitor);
        width = videoMode->width;
        height = videoMode->height;
    }

#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    GLFWwindow* window = glfwCreateWindow(width, height, "Mapbox GL", monitor, NULL);
    if (!window) {
        glfwTerminate();
        mbgl::Log::Error(mbgl::Event::OpenGL, "failed to initialize window");
        exit(1);
    }

    glfwSetWindowUserPointer(window, &viewData);
    glfwMakeContextCurrent(window);
    if (benchmark) {
        // Disables vsync on platforms that support it.
        glfwSwapInterval(0);
    } else {
        glfwSwapInterval(1);
    }

    glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        GLFWdata *view = reinterpret_cast<GLFWdata *>(glfwGetWindowUserPointer(window));
        view->width = width;
        view->height = height;
        view->dirty = 1;
    });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        GLFWdata *view = reinterpret_cast<GLFWdata *>(glfwGetWindowUserPointer(window));
        view->fbWidth = width;
        view->fbHeight = height;
        view->dirty = 1;
    });

    mbgl::gl::InitializeExtensions(glfwGetProcAddress);

    glfwGetWindowSize(window, &width, &height);
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    float pixelRatio = static_cast<float>(fbWidth) / width;

    glfwMakeContextCurrent(nullptr);

    /**/    

    viewData.window = window;
    viewData.width = width;
    viewData.height = height;
    viewData.fbWidth = fbWidth;
    viewData.fbHeight = fbHeight;

    mbgl_CApiView_Callbacks_t viewFunctions = {
        getPixelRatio,
        getSize,
        getFramebufferSize,
        activate,
        deactivate,
        notify,
        invalidate,
        swap
    };

    mbgl_CApiView_t* view_c = NULL;
    mbgl_CApiView_init(&viewFunctions, &viewData, &view_c);

    mbgl::CApiView view(view_c);

    /**/    

    if (benchmark) {
        mbgl::Log::Info(mbgl::Event::General, "BENCHMARK MODE: Some optimizations are disabled.");
    }

    //mbgl::SQLiteCache cache("cache.sqlite");
    mbgl_SQLiteCache_t* cache = NULL;
    mbgl_SQLiteCache_init("cache.sqlite", &cache);
    
    //mbgl::DefaultFileSource fileSource(&cache);
    mbgl_DefaultFileSource_t* fileSource = NULL;
    mbgl_DefaultFileSource_init(cache, &fileSource);

    // Set access token if present
    const char *token = getenv("MAPBOX_ACCESS_TOKEN");
    if (token == nullptr) {
        mbgl::Log::Warning(mbgl::Event::Setup, "no access token set. mapbox.com tiles won't work.");
    } else {
        mbgl_DefaultFileSource_setAccessToken(fileSource, token);
    }

    // IMPORTANT(nico) must be created before MapContext (implicit dependency)
    //mbgl::MapThreadContext imMapThreadContext;
    mbgl_MapThreadContext_t* inMapThreadContext = NULL;
    mbgl_MapThreadContext_init(&inMapThreadContext);
    
    //mbgl::MapData mapData(mbgl::MapMode::Continuous, 1.0f);
    mbgl_MapData_t* mapData = NULL;
    mbgl_MapData_init((int)mbgl::MapMode::Continuous, 1.0f, &mapData);
    
    //mbgl::MapContext mapContext(view, fileSource, mapData);
    mbgl_MapContext_t* mapContext = NULL;
    mbgl_MapContext_init((mbgl_View_t*)view_c, fileSource, mapData, &mapContext);
    
    //mbgl::Transform transform(view);
    mbgl_Transform_t* transform = NULL;
    mbgl_Transform_init((mbgl_View_t*)view_c, &transform);

    //mbgl::MapImmediate imMap(&mapData, &mapContext, &transform);
    mbgl_MapImmediate_t* imMap = NULL;
    mbgl_MapImmediate_init(mapData, mapContext, transform, &imMap);
    
    //imMap.resize(&view);
    mbgl_MapImmediate_resize(imMap, (mbgl_View_t*)view_c);

    // Load style
    if (style.empty()) {
        const auto& newStyle = mbgl::util::defaultStyles[1];
        style = newStyle.first;
    }

    //mapContext.setStyleURL(style);
    mbgl_MapContext_setStyleURL(mapContext, style.c_str());

    while (!(shouldCloseWindow || glfwWindowShouldClose(window))) {
        
        glfwPollEvents();
        
        //imMapThreadContext.process();
        mbgl_MapThreadContext_process(inMapThreadContext);

        if (viewData.dirty) {
            //imMap.resize(&view);
            mbgl_MapImmediate_resize(imMap, (mbgl_View_t*)view_c);
            viewData.dirty = 0;
        }

        longitude += 1e-3;
        zoom = 5.0 + 0.25 * cos(longitude * 8.0) - 2.0 * sin(longitude * 1.33);
        
        //transform.setLatLngZoom(mbgl::LatLng{latitude, longitude}, zoom, mbgl::CameraOptions());
        mbgl_Transform_setLatLngZoom(transform, latitude, longitude, zoom);
        
        //mapContext.triggerUpdate(transform.getState(), mbgl::Update::Zoom);
        mbgl_MapContext_triggerUpdate(mapContext, transform, (int)mbgl::Update::Zoom);
        
        //imMap.render(&view);
        mbgl_MapImmediate_render(imMap, (mbgl_View_t*)view_c);
        
        //imMap.update();
        mbgl_MapImmediate_update(imMap);
    }

    // Cleanup
    mbgl_MapImmediate_close(imMap);
    mbgl_Transform_close(transform);
    mbgl_MapContext_close(mapContext); // TODO or not? mapContext.cleanup();
    mbgl_MapData_close(mapData);
    mbgl_MapThreadContext_close(inMapThreadContext);
    
    glfwDestroyWindow(viewData.window);
    glfwTerminate();

    return 0;
}
