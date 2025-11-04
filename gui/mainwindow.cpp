#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QKeyEvent>

#include "gui/config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // setup ui elements
    ui->setupUi(this);

    // connect ui elements to slots
    connect(this->ui->sliderAnimationSpeed, SIGNAL(valueChanged(int)), this, SLOT(setAnimationSpeed(int)));
    connect(this->ui->checkBoxLocalRotation, SIGNAL(clicked(bool)), this, SLOT(setLocalRotation(bool)));
    connect(this->ui->checkBoxSunLight, SIGNAL(clicked(bool)), this, SLOT(setSunLight(bool)));
    connect(this->ui->checkBoxshowWireframe, SIGNAL(clicked(bool)), this, SLOT(setshowWireframe(bool)));
    connect(this->ui->checkBoxshowOrbits, SIGNAL(clicked(bool)), this, SLOT(setshowOrbits(bool)));
    connect(this->ui->checkBoxGlobalRotation, SIGNAL(clicked(bool)), this, SLOT(setGlobalRotation(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setAnimationSpeed(int value)
{
    Config::animationSpeed = float(value) / 2.0f;
    QString title = QString("Animation: ") + QString::number(Config::animationSpeed, 'x', 1) + "x";
    this->ui->groupBox_4->setTitle(title);
}

void MainWindow::setLocalRotation(bool value)
{
    Config::localRotation = value;
}

void MainWindow::setGlobalRotation(bool value)
{
    Config::GlobalRotation = value;
}

void MainWindow::setSunLight(bool value)
{
    Config::sunLight = value;
}

void MainWindow::setshowWireframe(bool value)
{
    Config::showWireframe = value;
}

void MainWindow::setshowOrbits(bool value)
{
    Config::showOrbits = value;
}

/* Qt mouse and keyboard events */

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // enable control via keyboard
    switch (event->key()) {
    case Qt::Key_F:
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
        break;
    case Qt::Key_Escape:
    case Qt::Key_Q:
        close();
        break;
    }
}


