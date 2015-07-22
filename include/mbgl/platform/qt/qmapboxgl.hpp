#ifndef QMAPBOXGL_H
#define QMAPBOXGL_H

#include <QObject>

class QGLContext;
class QImage;
class QPointF;
class QString;

class QMapboxGLPrivate;

class QMapboxGL : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom)
    Q_PROPERTY(double bearing READ bearing WRITE setBearing)

public:
    QMapboxGL(QGLContext *context);
    ~QMapboxGL();

    void setAccessToken(const QString &token);

    void setStyleJSON(const QString &style);
    void setStyleURL(const QString &url);

    double latitude() const;
    void setLatitude(double latitude);

    double longitude() const;
    void setLongitude(double longitude);

    double zoom() const;
    void setZoom(double zoom);

    double bearing() const;
    void setBearing(double degrees);
    void setBearing(double degrees, double cx, double xy);

    void setLatLngZoom(double latitude, double longitude, double zoom);
    void setGestureInProgress(bool inProgress);

    void moveBy(double dx, double dy);
    void scaleBy(double ds, double cx, double cy, int milliseconds);
    void rotateBy(double sx, double sy, double ex, double ey);

    void resize(int w, int h);

    QPointF coordinatesForPixel(const QPointF &pixel) const;

    void setSprite(const QString &name, const QImage &sprite);
    quint32 addPointAnnotation(const QString &name, const QPointF &position);

signals:
    void initialized();

private:
    Q_DISABLE_COPY(QMapboxGL)

    QMapboxGLPrivate *d_ptr;
};

#endif // QMAPBOXGL_H
