/*
 * Copyright (C) 2013, 2014
 * Computer Graphics Group, University of Siegen
 * Written by Martin Lambers <martin.lambers@uni-siegen.de>
 * All rights reserved.
 */

#include <iostream>
#include <GL/glew.h>

#include "glwidget.hpp"

#include <QMouseEvent>

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtx/transform.hpp>

#include "gui/config.h"

#include "planets/coordinatesystem.h"
#include "planets/deathstar.h"
#include "planets/planet.h"
#include "planets/sun.h"
#include "planets/skybox.h"

GLWidget::GLWidget(QWidget *&parent) : QOpenGLWidget(parent),//static_cast<QWidget*>(0)),
    _updateTimer(this), _stopWatch()
{
    // update the scene periodically
    QObject::connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(animateGL()));
    _updateTimer.start(18);
    _stopWatch.start();


    // create all drawable elements
    _skybox = std::make_shared<Skybox>("Skybox");
    _coordSystem = std::make_shared<CoordinateSystem>("Coordinate system");

    /***************************
     * DO NOT CHANGE days/year *
     * *************************/
                                                        // radius, distance, h/day, days/year
    _earth          = std::make_shared<Planet> ("Erde",     1.0,    0.0,    24.0,   1, ":/res/images/earth.bmp");
    auto moon       = std::make_shared<Planet>("Mond",      0.215,  2.0,    27.3,   27, ":/res/images/moon.bmp");
    auto sun        = std::make_shared<Sun>("Sonne",        1.2,    6.0,    50.0,   350, ":/res/images/sun.bmp");
    auto mercury    = std::make_shared<Planet>("Merkur",    0.34,   2.32,   1407.5, 150, ":/res/images/mercury.bmp");
    auto venus      = std::make_shared<Planet>("Venus",     0.34,   3.0,    2802.0, 100, ":/res/images/venus.bmp");

    auto mars       = std::make_shared<Planet>("Mars",      0.453,  10.6,   24.7,   700, ":/res/images/mars.bmp");
    auto jupiter    = std::make_shared<Planet>("Jupiter",   0.453,  13.32,  9.9,    3500, ":/res/images/jupiter.bmp");
    auto saturn     = std::make_shared<Planet>("Saturn",    0.453,  15.92,  10.6,   10500, ":/res/images/saturn.bmp");

    // jupiter moons
    auto io         = std::make_shared<Planet>("Io",        0.036,  0.8,    10.6,   30, ":/res/images/moon.bmp");
    auto europa     = std::make_shared<Planet>("Europa",    0.031,  1.0,    10.6,   60, ":/res/images/moon.bmp");
    auto ganymede   = std::make_shared<Planet>("Ganymed",   0.052,  1.2,    10.6,   120, ":/res/images/moon.bmp");
    auto callisto   = std::make_shared<Planet>("Callisto",  0.048,  1.8,    10.6,   350, ":/res/images/moon.bmp");

    auto deathStar  = std::make_shared<DeathStar>("Todesstern", 0.315,  2.0,    27.3,    50, ":/res/images/moon.bmp");

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

    /// TODO: Init all drawables here
    _earth->init();
}

void GLWidget::resizeGL(int width, int height)
{
    // update the viewport
    glViewport(0, 0, width, height);

    /// TODO: store the resolution in the config in case someone needs it
}

void GLWidget::paintGL()
{
    /// TODO: recreate the scene if needed

    // Render: set up view
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    /// TODO: calculate projection matrix from resolution
    glm::mat4 projection_matrix = glm::perspective(glm::radians(50.0f),
                783.0f / 691,
                0.1f, 100.0f);


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /// TODO: call draw for all drawables
    _earth->draw(projection_matrix);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    // ignore if it was not the left mouse button
    if(!event->button() & Qt::LeftButton)
        return;

    /// TODO: handle left press here
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // ignore if it was not the left mouzse button
    if(!event->button() & Qt::LeftButton)
        return;

    /// TODO: handle left release here
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    /// TODO: handle camera movement here
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    /// TODO: handle zoom here

    // Hint: you can use:
    // event->angleDelta().ry()
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
    /// TODO: use your camera logic here
    glm::mat4 modelViewMatrix = glm::lookAt(glm::vec3(0.0, 0.0, -5.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));

    // update drawables
    /// TODO update all drawables
    _earth->update(timeElapsedMs, modelViewMatrix);

    // update the widget (do not remove this!)
    update();
}


