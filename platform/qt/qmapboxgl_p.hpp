#ifndef QMAPBOXGL_P_H
#define QMAPBOXGL_P_H

#include "qfilesource_p.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

#include <QObject>

namespace mbgl {

class Map;
class FileSource;

}  // namespace mbgl

class QGLContext;
class QMapboxGL;

class QMapboxGLPrivate : public QObject, public mbgl::View
{
    Q_OBJECT

public:
    explicit QMapboxGLPrivate(QMapboxGL *q);
    virtual ~QMapboxGLPrivate();

    // mbgl::View implementation.
    float getPixelRatio() const final;
    std::array<uint16_t, 2> getSize() const final;
    std::array<uint16_t, 2> getFramebufferSize() const final;
    void activate() final;
    void deactivate() final;
    void notify() final;
    void invalidate() final;
    void swap() final;

    std::array<uint16_t, 2> size;

    bool contextIsCurrent;
    QGLContext *context;

    QFileSourcePrivate fileSourceObj;
    mbgl::Map mapObj;

    QMapboxGL *q_ptr;

signals:
    void viewInvalidated();

public slots:
    void triggerRender();
};

#endif // QMAPBOXGL_P_H
