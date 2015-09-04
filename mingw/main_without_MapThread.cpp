#include <mbgl/mbgl.hpp>
#include "../platform/default/default_styles.hpp"
#include <mbgl/util/uv.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/default/settings_json.hpp>
#include <mbgl/platform/default/glfw_view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/transform.hpp>

#include <signal.h>
#include <getopt.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

/*****************************************************************************/

struct MapImmediate {

    mbgl::MapData* mapData = nullptr;
    mbgl::MapContext* mapContext = nullptr;
    mbgl::Transform* transform = nullptr;

    enum class RenderState {
        never,
        partial,
        fully
    };
    RenderState renderState = RenderState::never;

    MapImmediate(mbgl::MapData* mapData_, mbgl::MapContext* mapContext_, mbgl::Transform* transform_) 
    : mapData(mapData_), mapContext(mapContext_), transform(transform_) {
        assert(mapData_);
        assert(mapContext_);
        assert(transform_);
    }

    void render(mbgl::View* view) {
        if (renderState == RenderState::never) {
            view->notifyMapChange(mbgl::MapChangeWillStartRenderingMap);
        }

        view->notifyMapChange(mbgl::MapChangeWillStartRenderingFrame);

        const bool fullyLoaded = mapContext->renderSync(transform->getState(), mbgl::FrameData { view->getFramebufferSize() });

        view->notifyMapChange(fullyLoaded ?
            mbgl::MapChangeDidFinishRenderingFrameFullyRendered :
            mbgl::MapChangeDidFinishRenderingFrame);

        if (!fullyLoaded) {
            renderState = RenderState::partial;
        } else if (renderState != RenderState::fully) {
            renderState = RenderState::fully;
            view->notifyMapChange(mbgl::MapChangeDidFinishRenderingMapFullyRendered);
        }
    }

    void resize(mbgl::View* view) {
        transform->resize(view->getSize());
        mapContext->triggerUpdate(transform->getState(), mbgl::Update::Dimensions);
    }

    void update() {
        // TODO(nico) give the clock in a known unit as a parameter, then convert to mbgl::Clock (which is std::chrono::steady_clock)
        mbgl::Update flags = transform->updateTransitions(mbgl::Clock::now());
        if (mapData->getNeedsRepaint()) {
            flags |= mbgl::Update::Repaint;
        }
        mapContext->triggerUpdate(transform->getState(), flags);
    }

};

struct MapThreadContext {

    uv::loop uvLoop;
    mbgl::util::RunLoop runLoop;
    mbgl::util::MapThreadContextRegistrar fakeMapThread;

    MapThreadContext() : runLoop(uvLoop.get()) {
    }

    ~MapThreadContext() {
        // make sure not to lose events
        uv_run(uvLoop.get(), UV_RUN_NOWAIT);
    }

    void process() {
        assert(&runLoop == mbgl::util::RunLoop::Get());
        uv_run(uvLoop.get(), UV_RUN_NOWAIT);
    }
};

/*****************************************************************************/

namespace {

std::unique_ptr<GLFWView> view;

}

void quit_handler(int) {
    if (view) {
        mbgl::Log::Info(mbgl::Event::Setup, "waiting for quit...");
        view->setShouldClose();
    } else {
        exit(0);
    }
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

    if (benchmark) {
        mbgl::Log::Info(mbgl::Event::General, "BENCHMARK MODE: Some optimizations are disabled.");
    }

    view = std::make_unique<GLFWView>(fullscreen, benchmark);

    mbgl::SQLiteCache cache("cache.sqlite");
    mbgl::DefaultFileSource fileSource(&cache);

    // Set access token if present
    const char *token = getenv("MAPBOX_ACCESS_TOKEN");
    if (token == nullptr) {
        mbgl::Log::Warning(mbgl::Event::Setup, "no access token set. mapbox.com tiles won't work.");
    } else {
        fileSource.setAccessToken(std::string(token));
    }

    //mbgl::Map map(*view, fileSource);

    MapThreadContext imMapThreadContext;                                    // IMPORTANT(nico) must be created before MapContext (implicit dependency)

    mbgl::MapData mapData(mbgl::MapMode::Continuous, 1.0f);
    mbgl::MapContext mapContext(*view, fileSource, mapData);
    mbgl::Transform transform(*view);

    MapImmediate imMap(&mapData, &mapContext, &transform);
    imMap.resize(view.get());

    //mapData.setDebug(true);
    /*
    // Load settings
    mbgl::Settings_JSON settings;

    if (skipConfig) {
        map.setLatLngZoom(mbgl::LatLng(latitude, longitude), zoom);
        map.setBearing(bearing);
        mbgl::Log::Info(mbgl::Event::General, "Location: %f/%f (z%.2f, %.2f deg)", latitude, longitude, zoom, bearing);
    } else {
        map.setLatLngZoom(mbgl::LatLng(settings.latitude, settings.longitude), settings.zoom);
        map.setBearing(settings.bearing);
        map.setDebug(settings.debug);
    }
    */

    view->setChangeStyleCallback([&mapContext] () {
        static uint8_t currentStyleIndex;

        if (++currentStyleIndex == mbgl::util::defaultStyles.size()) {
            currentStyleIndex = 0;
        }

        const auto& newStyle = mbgl::util::defaultStyles[currentStyleIndex];
        mapContext.setStyleURL(newStyle.first);
        view->setWindowTitle(newStyle.second);

        mbgl::Log::Info(mbgl::Event::Setup, std::string("Changed style to: ") + newStyle.first);
    });

    // Load style
    if (style.empty()) {
        const auto& newStyle = mbgl::util::defaultStyles[1];
        style = newStyle.first;
        view->setWindowTitle(newStyle.second);
    }

    mapContext.setStyleURL(style);

    //view->run();
    while (!glfwWindowShouldClose(view->window)) {
        
        //glfwWaitEvents();
        glfwPollEvents();
        imMapThreadContext.process();

        const bool dirty = true; //!view->clean.test_and_set();

        longitude += 1e-3;
        zoom = 5.0 + 0.25 * cos(longitude * 8.0) - 2.0 * sin(longitude * 1.33);

        transform.setLatLngZoom(mbgl::LatLng{latitude, longitude}, zoom, mbgl::CameraOptions());
        mapContext.triggerUpdate(transform.getState(), mbgl::Update::Zoom);
        
        if (dirty) {
            const double started = glfwGetTime();
            
            //map->renderSync();
            imMap.render(view.get());

            //TODO//report(1000 * (glfwGetTime() - started));

            if (benchmark) {
                //map->setNeedsRepaint();
                mapData.setNeedsRepaint(true);
            }

            //map->nudgeTransitions();
            imMap.update();
        }
    }

    mapContext.cleanup();

    /*
    // Save settings
    mbgl::LatLng latLng = transform.getLatLng();
    settings.latitude = latLng.latitude;
    settings.longitude = latLng.longitude;
    settings.zoom = transform.getZoom();
    settings.bearing = transform.getBearing();
    settings.debug = mapData.getDebug();
    if (!skipConfig) {
        settings.save();
    }
    mbgl::Log::Info(mbgl::Event::General,
                    "Exit location: --lat=\"%f\" --lon=\"%f\" --zoom=\"%f\" --bearing \"%f\"",
                    settings.latitude, settings.longitude, settings.zoom, settings.bearing);
    */

    return 0;
}
