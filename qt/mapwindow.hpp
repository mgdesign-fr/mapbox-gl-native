#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include "../platform/default/default_styles.hpp"

#include <mbgl/platform/qt/QMapboxGL>

#include <QApplication>
#include <QGLWidget>

class QGLContext;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class MapWindow : public QGLWidget
{
public:
    MapWindow(QGLContext *ctx);

private:
    void changeStyle();

    // QGLWidget implementation.
    void keyPressEvent(QKeyEvent *ev) final;
    void mousePressEvent(QMouseEvent *ev) final;
    void mouseMoveEvent(QMouseEvent *ev) final;
    void wheelEvent(QWheelEvent *ev) final;
    void resizeGL(int w, int h) final;

    int lastX = 0;
    int lastY = 0;

    QMapboxGL m_map;
};

#endif
