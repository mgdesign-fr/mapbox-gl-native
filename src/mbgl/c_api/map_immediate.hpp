#ifndef MBGL_C_API_MAP_IMMEDIATE
#define MBGL_C_API_MAP_IMMEDIATE

#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/transform.hpp>

namespace mbgl {

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

    // NOTE(nico) user should call 'resize' before any rendering or update can occur
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
        // TODO(nico) ? give the clock in a known unit as a parameter, then convert to mbgl::Clock (which is std::chrono::steady_clock)
        //   this may not be a good thing, as mbgl::Clock::now() is used as a time source in internal mbgl code.
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

    // Register the current 'RunLoop' used by MapContext
    // and the current 'ThreadContext' as 'ThreadType::Map'
    MapThreadContext() : runLoop(uvLoop.get()) {
    }

    ~MapThreadContext() {
        // make sure not to lose events
        uv_run(uvLoop.get(), UV_RUN_NOWAIT);
    }

    // Process the events for MapContext
    void process() {
        assert(&runLoop == mbgl::util::RunLoop::Get());
        uv_run(uvLoop.get(), UV_RUN_NOWAIT);
    }
};

}

#endif
