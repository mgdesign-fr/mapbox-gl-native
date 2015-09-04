#ifndef MBGL_C_API_CAPIVIEW
#define MBGL_C_API_CAPIVIEW

#include <mbgl/map/view.hpp>
#include <mbgl/mbgl_c.h>

#include <cassert>

namespace mbgl {
  class CApiView;
}

/*****************************************************************************/

struct mbgl_View_t {
  mbgl::View* view;
};

struct mbgl_CApiView_t {
  union {
      mbgl_View_t base;
      mbgl::CApiView* view;
  };
  mbgl_CApiView_Callbacks_t* callbacks;
  void* userdata;
};

/*****************************************************************************/

namespace mbgl {

class CApiView : public mbgl::View {
private:
  mbgl_CApiView_t* opaqueView;

public:
  explicit CApiView(mbgl_CApiView_t* _opaqueView) : opaqueView(_opaqueView) {
    assert(opaqueView != 0);
  }

  ~CApiView() {
  }

  float getPixelRatio() const override {
    if(opaqueView->callbacks->getPixelRatio != 0)
      return opaqueView->callbacks->getPixelRatio(opaqueView, opaqueView->userdata);
    else
      return 1.0f;
  }
  
  std::array<uint16_t, 2> getSize() const override {
    uint16_t width = 0;
    uint16_t height = 0;
    if(opaqueView->callbacks->getSize != 0)
      opaqueView->callbacks->getSize(opaqueView, opaqueView->userdata, &width, &height);
    return {{ width, height }};
  }
  
  std::array<uint16_t, 2> getFramebufferSize() const override {
    uint16_t fbWidth = 0;
    uint16_t fbHeight = 0;
    if(opaqueView->callbacks->getFramebufferSize != 0)
      opaqueView->callbacks->getFramebufferSize(opaqueView, opaqueView->userdata, &fbWidth, &fbHeight);
    return {{ fbWidth, fbHeight }};
  }
  
  void activate() override {
    if(opaqueView->callbacks->activate != 0)
      opaqueView->callbacks->activate(opaqueView, opaqueView->userdata);
  }
  
  void deactivate() override {
    if(opaqueView->callbacks->deactivate != 0)
      opaqueView->callbacks->deactivate(opaqueView, opaqueView->userdata);
  }
  
  void notify() override {
    if(opaqueView->callbacks->notify != 0)
      opaqueView->callbacks->notify(opaqueView, opaqueView->userdata);
  }
  
  void invalidate() override {
    if(opaqueView->callbacks->invalidate != 0)
      opaqueView->callbacks->invalidate(opaqueView, opaqueView->userdata);
  }
  
  void swap() override {
    if(opaqueView->callbacks->swap != 0)
      opaqueView->callbacks->swap(opaqueView, opaqueView->userdata);
  }
};

}

#endif