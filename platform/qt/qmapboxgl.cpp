#include "qmapboxgl_p.hpp"

#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/sprite_image.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/qt/qmapboxgl.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include <QGLContext>
#include <QImage>
#include <QString>

#include <chrono>

QMapboxGL::QMapboxGL(QGLContext *context) : d_ptr(new QMapboxGLPrivate(this))
{
    d_ptr->context = context;
}

QMapboxGL::~QMapboxGL()
{
    delete d_ptr;
}

void QMapboxGL::setAccessToken(const QString &token)
{
    d_ptr->fileSourceObj.setAccessToken(token.toUtf8().constData());
}

void QMapboxGL::setStyleJSON(const QString &style)
{
    d_ptr->mapObj.setStyleJSON(style.toUtf8().constData());
}

void QMapboxGL::setStyleURL(const QString &url)
{
    d_ptr->mapObj.setStyleURL(url.toUtf8().constData());
}

double QMapboxGL::latitude() const
{
    return d_ptr->mapObj.getLatLng().latitude;
}

void QMapboxGL::setLatitude(double latitude_)
{
    d_ptr->mapObj.setLatLng({ latitude_, longitude() });
}

double QMapboxGL::longitude() const
{
    return d_ptr->mapObj.getLatLng().longitude;
}

void QMapboxGL::setLongitude(double longitude_)
{
    d_ptr->mapObj.setLatLng({ latitude(), longitude_ });
}

double QMapboxGL::zoom() const
{
    return d_ptr->mapObj.getZoom();
}

void QMapboxGL::setZoom(double zoom_)
{
    d_ptr->mapObj.setZoom(zoom_);
}

void QMapboxGL::setLatLngZoom(double latitude_, double longitude_, double zoom_)
{
    d_ptr->mapObj.setLatLngZoom({ latitude_, longitude_ }, zoom_);
}

double QMapboxGL::bearing() const
{
    return d_ptr->mapObj.getBearing();
}

void QMapboxGL::setBearing(double degrees_)
{
    d_ptr->mapObj.setBearing(degrees_);
}

void QMapboxGL::setBearing(double degrees_, double cx, double cy)
{
    d_ptr->mapObj.setBearing(degrees_, cx, cy);
}

void QMapboxGL::setGestureInProgress(bool inProgress)
{
    d_ptr->mapObj.setGestureInProgress(inProgress);
}

void QMapboxGL::moveBy(double dx, double dy)
{
    d_ptr->mapObj.moveBy(dx, dy);
}

void QMapboxGL::scaleBy(double ds, double cx, double cy, int milliseconds) {
    d_ptr->mapObj.scaleBy(ds, cx, cy, std::chrono::milliseconds(milliseconds));
}

void QMapboxGL::rotateBy(double sx, double sy, double ex, double ey)
{
    d_ptr->mapObj.rotateBy(sx, sy, ex, ey);
}

void QMapboxGL::resize(int w, int h)
{
    d_ptr->size = {{ static_cast<uint16_t>(w), static_cast<uint16_t>(h) }};
    d_ptr->mapObj.update(mbgl::Update::Dimensions);
}

QPointF QMapboxGL::coordinatesForPixel(const QPointF &pixel) const
{
    // 0x0 on Qt is the top/left corner, mbgl uses bottom/left
    const auto position = d_ptr->mapObj.latLngForPixel({ pixel.x(), d_ptr->size[1] - pixel.y() });

    return QPointF(position.latitude, position.longitude);
}

void QMapboxGL::setSprite(const QString &name, const QImage &sprite)
{
    if (sprite.isNull()) {
        return;
    }

    const QImage swapped = sprite.rgbSwapped();

    d_ptr->mapObj.setSprite(name.toUtf8().constData(), std::make_shared<mbgl::SpriteImage>(
        swapped.width(), swapped.height(), 1.0,
        std::string(reinterpret_cast<const char*>(swapped.constBits()), swapped.byteCount())));
}

quint32 QMapboxGL::addPointAnnotation(const QString &name, const QPointF &position) {
    return d_ptr->mapObj.addPointAnnotation({ { position.x(), position.y() }, name.toUtf8().data() });
}

QMapboxGLPrivate::QMapboxGLPrivate(QMapboxGL *q)
    : size({0, 0})
    , contextIsCurrent(false)
    , fileSourceObj(nullptr)
    , mapObj(*this, fileSourceObj)
    , q_ptr(q)
{
    connect(this, SIGNAL(viewInvalidated(void)), this, SLOT(triggerRender()), Qt::QueuedConnection);
}

QMapboxGLPrivate::~QMapboxGLPrivate()
{
}

float QMapboxGLPrivate::getPixelRatio() const
{
    return 1.0;
}

std::array<uint16_t, 2> QMapboxGLPrivate::getSize() const
{
    return size;
}

std::array<uint16_t, 2> QMapboxGLPrivate::getFramebufferSize() const
{
    return size;
}

void QMapboxGLPrivate::activate()
{
    // Map thread
}

void QMapboxGLPrivate::deactivate()
{
    // Map thread
    context->doneCurrent();
}

void QMapboxGLPrivate::notify()
{
    // Map thread
}

void QMapboxGLPrivate::invalidate()
{
    // Map thread
    if (!context->isValid()) {
        return;
    }

    if (!contextIsCurrent) {
        context->makeCurrent();
        contextIsCurrent = true;

        mbgl::gl::InitializeExtensions([](const char *name) {
            const QGLContext *thisContext = QGLContext::currentContext();
            return reinterpret_cast<mbgl::gl::glProc>(thisContext->getProcAddress(name));
        });

        QMetaObject::invokeMethod(q_ptr, "initialized");
    }

    emit viewInvalidated();
}

void QMapboxGLPrivate::swap()
{
    // Map thread
    context->swapBuffers();
}

void QMapboxGLPrivate::triggerRender()
{
    mapObj.renderSync();
    mapObj.nudgeTransitions();
}
