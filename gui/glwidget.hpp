/*
* Copyright (C) 2014
 * Computer Graphics Group, University of Siegen
 * Written by Martin Lambers <martin.lambers@uni-siegen.de>
 * All rights reserved.
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <memory>

#include <QElapsedTimer>
#include <QMessageBox>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QTimer>
#include <QPoint>

class Planet;
class Skybox;
class CoordinateSystem;

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

private:

    QTimer _updateTimer;
    QElapsedTimer _stopWatch;

    std::shared_ptr<Planet> _earth;
    std::shared_ptr<Skybox> _skybox;
    std::shared_ptr<CoordinateSystem> _coordSystem;

    bool _isMousePressed = false;
    QPoint _lastMousePos;
    float _cameraAngleX = 0.0f;
    float _cameraAngleY = 0.0f;
    float _cameraDistance = 5.0f;

    int _width = 1;
    int _height = 1;


private slots:
    void animateGL();

public:
    static void setGLFormat ()
    {
        QSurfaceFormat format;
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        format.setSwapInterval(::getenv("COREGL_FPS") ? 0 : 1);
        format.setVersion(4, 0);
        format.setProfile(QSurfaceFormat::CoreProfile);
        QSurfaceFormat::setDefaultFormat(format);
        format.setDepthBufferSize(16);
    }

    GLWidget(QWidget*& parent);

    virtual void show();

    virtual void initializeGL() override;

    virtual void resizeGL(int width, int height) override;

    virtual void paintGL() override;

    virtual void mousePressEvent(QMouseEvent * event) override;

    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    virtual void mouseMoveEvent(QMouseEvent* event) override;

    virtual void wheelEvent(QWheelEvent *event) override;

public slots:
    void setPolygonResolution(int segments);

};

#endif