#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QDebug>
#include "gui/config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow constructor called.";
    ui->setupUi(this);

    connect(this->ui->sliderAnimationSpeed, SIGNAL(valueChanged(int)), this, SLOT(setAnimationSpeed(int)));
    connect(this->ui->checkBoxLocalRotation, SIGNAL(clicked(bool)), this, SLOT(setLocalRotation(bool)));
    connect(this->ui->checkBoxSunLight, SIGNAL(clicked(bool)), this, SLOT(setSunLight(bool)));
    connect(this->ui->checkBoxshowWireframe, SIGNAL(clicked(bool)), this, SLOT(setshowWireframe(bool)));
    connect(this->ui->checkBoxshowOrbits, SIGNAL(clicked(bool)), this, SLOT(setshowOrbits(bool)));
    connect(this->ui->checkBoxGlobalRotation, SIGNAL(clicked(bool)), this, SLOT(setGlobalRotation(bool)));
    connect(this->ui->checkBoxCoordinateSystem, SIGNAL(clicked(bool)), this, SLOT(setCoordinateSystem(bool)));
    connect(this->ui->checkBox3DOrbits, SIGNAL(clicked(bool)), this, SLOT(set3DOrbits(bool)));
    connect(this->ui->checkBoxLocalOrbits, SIGNAL(clicked(bool)), this, SLOT(setLocalOrbits(bool)));
    connect(this->ui->sliderResolution, SIGNAL(valueChanged(int)), this, SLOT(setPolygonResolution(int)));

    connect(this->ui->sliderLaserCutoff, SIGNAL(valueChanged(int)), this, SLOT(setLaserCutoff(int)));
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow destructor called.";
    delete ui;
}

void MainWindow::setAnimationSpeed(int value)
{
    qDebug() << "setAnimationSpeed called with value:" << value;
    Config::animationSpeed = float(value) / 2.0f;
    QString title = QString("Animation: ") + QString::number(Config::animationSpeed, 'x', 1) + "x";
    this->ui->groupBox_4->setTitle(title);
}

void MainWindow::setPolygonResolution(int value)
{
    qDebug() << "setPolygonResolution called with value:" << value;
    QString title = QString("Auflösung: ") + QString::number(value);
    this->ui->groupBox_Resolution->setTitle(title);

    this->ui->openGLWidget->setPolygonResolution(value);
}

void MainWindow::setLaserCutoff(int value)
{
    qDebug() << "setLaserCutoff called with value:" << value;
    Config::laserCutoff = static_cast<float>(value);

    QString title = QString("Laser Cutoff: ") + QString::number(value) + "\xC2\xB0";
    this->ui->groupBox_Laser->setTitle(title);
}

void MainWindow::setLocalRotation(bool value)
{
    qDebug() << "setLocalRotation called with value:" << value;
    Config::localRotation = value;
}

void MainWindow::setGlobalRotation(bool value)
{
    qDebug() << "setGlobalRotation called with value:" << value;
    Config::GlobalRotation = value;
}

void MainWindow::setSunLight(bool value)
{
    qDebug() << "setSunLight called with value:" << value;
    Config::sunLight = value;
}

void MainWindow::setshowWireframe(bool value)
{
    qDebug() << "setshowWireframe called with value:" << value;
    Config::showWireframe = value;
}

void MainWindow::setshowOrbits(bool value)
{
    qDebug() << "setshowOrbits called with value:" << value;
    Config::showOrbits = value;
}

void MainWindow::setCoordinateSystem(bool value)
{
    qDebug() << "setCoordinateSystem called with value:" << value;
    Config::showCoordinateSystem = value;
}

void MainWindow::set3DOrbits(bool value)
{
    qDebug() << "set3DOrbits called with value:" << value;
    Config::show3DOrbits = value;
}

void MainWindow::setLocalOrbits(bool value)
{
    qDebug() << "setLocalOrbits called with value:" << value;
    Config::localOrbits = value;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    qDebug() << "keyPressEvent called with key:" << event->key();
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