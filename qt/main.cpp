#include "mapwindow.hpp"

#include <QApplication>
#include <QGLContext>
#include <QGLFormat>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QGLContext *context = new QGLContext(QGLFormat::defaultFormat());

    // MapWindow inherits from QGLWidget and will get
    // the ownership of the QGLContext while internally
    // it will create a QMapboxGL that will use the
    // context for rendering. QGLWidget will also handle
    // the input events and forward them to QMapboxGL
    // if necessary.
    MapWindow window(context);

    // The window will call show() internally
    // when QMapboxGL gets initialized.
    window.resize(800, 600);

    return app.exec();
}
