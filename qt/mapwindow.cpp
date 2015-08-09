#include "../platform/default/default_styles.hpp"

#include "mapwindow.hpp"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QString>

MapWindow::MapWindow(QGLContext *ctx)
    : QGLWidget(ctx) // QGLWidget takes the ownership of ctx.
    , m_map(ctx)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_AcceptTouchEvents);

    setAutoBufferSwap(false);

    connect(&m_map, SIGNAL(initialized()), this, SLOT(show()));

    m_map.setAccessToken(qgetenv("MAPBOX_ACCESS_TOKEN"));

    m_map.setCacheDatabase("/tmp/mbgl-cache.db");

    changeStyle();
}

void MapWindow::changeStyle()
{
    static uint8_t currentStyleIndex;

    const auto& newStyle = mbgl::util::defaultStyles[currentStyleIndex];
    QString url(newStyle.first.c_str());

    m_map.setStyleURL(url);

    QString name(newStyle.second.c_str());
    setWindowTitle(QString("Mapbox GL: ") + name);

    if (++currentStyleIndex == mbgl::util::defaultStyles.size()) {
        currentStyleIndex = 0;
    }
}

void MapWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_S) {
        changeStyle();
    }

    ev->accept();
}

void MapWindow::mousePressEvent(QMouseEvent *ev)
{
    lastX = ev->x();
    lastY = ev->y();

    if (ev->type() == QEvent::MouseButtonPress) {
        if (ev->buttons() == (Qt::LeftButton | Qt::RightButton)) {
            changeStyle();
        }
    }

    if (ev->type() == QEvent::MouseButtonDblClick) {
        if (ev->buttons() == Qt::LeftButton) {
            m_map.scaleBy(2.0, lastX, lastY, 500);
        } else if (ev->buttons() == Qt::RightButton) {
            m_map.scaleBy(0.5, lastX, lastY, 500);
        }
    }

    ev->accept();
}

void MapWindow::mouseMoveEvent(QMouseEvent *ev)
{
    int dx = ev->x() - lastX;
    int dy = ev->y() - lastY;

    if (dx || dy) {
        if (ev->buttons() == Qt::LeftButton) {
            m_map.moveBy(dx, dy);
        } else if (ev->buttons() == Qt::RightButton) {
            m_map.rotateBy(lastX, lastY, ev->x(), ev->y());
        }
    }

    lastX = ev->x();
    lastY = ev->y();

    ev->accept();
}

void MapWindow::wheelEvent(QWheelEvent *ev)
{
    if (ev->orientation() == Qt::Horizontal) {
        return;
    }

    float factor = ev->delta() / 1200.;
    if (ev->delta() < 0) {
        factor = factor > -1 ? factor : 1 / factor;
    }

    m_map.scaleBy(1 + factor, ev->x(), ev->y(), 50);
    ev->accept();
}

void MapWindow::resizeGL(int w, int h)
{
    m_map.resize(w, h);
}
