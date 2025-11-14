/*
 * Copyright (C) 2013, 2014
 * Computer Graphics Group, University of Siegen
 * Written by Martin Lambers <martin.lambers@uni-siegen.de>
 * All rights reserved.
 */

#include <iostream>
#include <GL/glew.h>

#include <cstdlib> // NEU: Für rand()
#include <ctime>   // NEU: Für time()

#include "glwidget.hpp"

#include <QMouseEvent>
#include <QWheelEvent>

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

#include "gui/config.h"

#include "planets/coordinatesystem.h"
#include "planets/deathstar.h"
#include "planets/planet.h"
#include "planets/sun.h"
#include "planets/skybox.h"

// Kleine Helferfunktion für Zufallswinkel
static float randAngle() {
    return static_cast<float>(rand() % 360);
}

GLWidget::GLWidget(QWidget *&parent) : QOpenGLWidget(parent),
    _updateTimer(this), _stopWatch()
{
    // update the scene periodically
    QObject::connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(animateGL()));
    _updateTimer.start(18);
    _stopWatch.start();

    // NEU: Zufallsgenerator initialisieren
    srand(static_cast<unsigned int>(time(NULL)));


    // create all drawable elements
    _skybox = std::make_shared<Skybox>("Skybox");
    _coordSystem = std::make_shared<CoordinateSystem>("Coordinate system");

    /***************************
     * DO NOT CHANGE days/year *
     * *************************/
                                                        // radius, distance, h/day, days/year, texture, startAngle, inclination
    _earth          = std::make_shared<Planet> ("Erde",     1.0,    0.0,    24.0,   1, ":/res/images/earth.bmp", 0.0f, 0.0f);
    _earth->setCloudTexture(":/res/images/clouds.bmp");
    auto moon       = std::make_shared<Planet>("Mond",      0.215,  2.0,    27.3,   27, ":/res/images/moon.bmp", randAngle(), 5.1f);
    auto sun        = std::make_shared<Sun>("Sonne",        1.2,    6.0,    50.0,   350, ":/res/images/sun.bmp", randAngle(), 7.25f); // 7.25 (Ekliptikneigung d. Sonne)

    // Planeten, die die Sonne umkreisen (im Tychonischen System)
    auto mercury    = std::make_shared<Planet>("Merkur",    0.34,   2.32,   1407.5, 150, ":/res/images/mercury.bmp", randAngle(), 7.0f);
    auto venus      = std::make_shared<Planet>("Venus",     0.34,   3.0,    2802.0, 100, ":/res/images/venus.bmp", randAngle(), 3.4f);
    auto mars       = std::make_shared<Planet>("Mars",      0.453,  10.6,   24.7,   700, ":/res/images/mars.bmp", randAngle(), 1.85f);
    auto jupiter    = std::make_shared<Planet>("Jupiter",   0.453,  13.32,  9.9,    3500, ":/res/images/jupiter.bmp", randAngle(), 1.3f);
    auto saturn     = std::make_shared<Planet>("Saturn",    0.453,  15.92,  10.6,   10500, ":/res/images/saturn.bmp", randAngle(), 2.5f);

    // jupiter moons
    auto io         = std::make_shared<Planet>("Io",        0.036,  0.8,    10.6,   30, ":/res/images/moon.bmp", randAngle(), 0.04f);
    auto europa     = std::make_shared<Planet>("Europa",    0.031,  1.0,    10.6,   60, ":/res/images/moon.bmp", randAngle(), 0.47f);
    auto ganymede   = std::make_shared<Planet>("Ganymed",   0.052,  1.2,    10.6,   120, ":/res/images/moon.bmp", randAngle(), 0.2f);
    auto callisto   = std::make_shared<Planet>("Callisto",  0.048,  1.8,    10.6,   350, ":/res/images/moon.bmp", randAngle(), 0.2f);

    auto deathStar  = std::make_shared<DeathStar>("Todesstern", 0.315,  2.0,    27.3,    50, ":/res/images/moon.bmp", randAngle(), 2.0f);

    // create hierarchy
    _earth->addChild(moon);
    _earth->addChild(sun);

    sun->addChild(mercury);
    sun->addChild(venus);
    sun->addChild(mars);
    sun->addChild(jupiter);
    sun->addChild(saturn);
    mars->addChild(deathStar);

    jupiter->addChild(io);
    jupiter->addChild(europa);
    jupiter->addChild(ganymede);
    jupiter->addChild(callisto);

    _earth->setLights(sun, deathStar->cone());
}

void GLWidget::show()
{
    QOpenGLWidget::show();

    // check for a valid context
    if (!isValid() || !context()->isValid() || context()->format().majorVersion() != 4) {
        QMessageBox::critical(this, "Error", "Cannot get a valid OpenGL 4 context.");
        exit(1);
    }
}

void GLWidget::initializeGL()
{
    /* Initialize OpenGL extensions */
    glewExperimental = GL_TRUE; // otherwise some function pointers are NULL...
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    glGetError(); // clear a gl error produced by glewInit

    // make sure the context is current
    makeCurrent();

    _earth->init();
    _coordSystem->init();
    _skybox->init();
}

void GLWidget::resizeGL(int width, int height)
{
    // update the viewport
    glViewport(0, 0, width, height);

    _width = width;
    _height = (height > 0) ? height : 1; // Division durch 0 verhindern
}

void GLWidget::paintGL()
{
    // Render: set up view
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    float aspectRatio = static_cast<float>(_width) / static_cast<float>(_height);
    glm::mat4 projection_matrix = glm::perspective(glm::radians(50.0f),
                aspectRatio,
                0.1f, 100.0f);


    if (Config::showWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // === 1. ERDE ZEICHNEN ===
    glDisable(GL_CULL_FACE);
    _earth->draw(projection_matrix);
    glEnable(GL_CULL_FACE);


    // === 2. KOORDINATENSYSTEM ZEICHNEN (NACH DER ERDE) ===
    if (Config::showCoordinateSystem)
    {
        glDisable(GL_DEPTH_TEST); // Tiefentest AUS
        _coordSystem->draw(projection_matrix);
        glEnable(GL_DEPTH_TEST);  // Tiefentest wieder AN
    }
    // ===================================================

    // === 3. SKYBOX ALS LETZTES ZEICHNEN ===
    _skybox->draw(projection_matrix);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        _isMousePressed = true;
        _lastMousePos = event->pos();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        _isMousePressed = false;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (_isMousePressed)
    {
        QPoint delta = event->pos() - _lastMousePos;
        _lastMousePos = event->pos();

        float sensitivity = 0.01f; // In Radian pro Pixel
        _cameraAngleX -= delta.x() * sensitivity;
        _cameraAngleY += delta.y() * sensitivity;

        // Clamping
        float maxAngleY = glm::radians(89.0f);
        _cameraAngleY = glm::clamp(_cameraAngleY, -maxAngleY, maxAngleY);
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().ry();
    float sensitivity = 0.01f;
    _cameraDistance -= delta * sensitivity;

    // Clamping
    _cameraDistance = glm::clamp(_cameraDistance, 0.5f, 50.0f);
}

void GLWidget::animateGL()
{
    // make the context current in case there are glFunctions called
    makeCurrent();

    // get the time delta
    float timeElapsedMs = _stopWatch.nsecsElapsed() / 1000000.0f;
    // restart stopwatch for next update
    _stopWatch.restart();

    // calculate current modelViewMatrix for the default camera
    float camX = _cameraDistance * cos(_cameraAngleY) * sin(_cameraAngleX);
    float camY = _cameraDistance * sin(_cameraAngleY);
    float camZ = _cameraDistance * cos(_cameraAngleY) * cos(_cameraAngleX);

    glm::vec3 cameraPosition = glm::vec3(camX, camY, camZ);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 modelViewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

    // update drawables
    _earth->update(timeElapsedMs, modelViewMatrix);
    _coordSystem->update(timeElapsedMs, modelViewMatrix);
    _skybox->update(timeElapsedMs, modelViewMatrix);

    // update the widget (do not remove this!)
    update();
}

// --- NEU HINZUGEFÜGT ---
void GLWidget::setPolygonResolution(int segments)
{
    // OpenGL-Kontext für diesen Thread (den UI-Thread) aktivieren!
    makeCurrent();

    // Leite den Wert an das Wurzelobjekt der Hierarchie weiter.
    // Die Objekte, die nicht Teil der Hierarchie sind (Skybox, CoordSystem),
    // sind keine Kugeln, Ringe oder Kegel und benötigen die Einstellung nicht.
    if (_earth)
    {
        _earth->setResolution(static_cast<unsigned int>(segments));
    }
}