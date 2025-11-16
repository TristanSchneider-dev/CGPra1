#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include "gui/config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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

    // --- NEU HINZUGEFÜGT ---
    connect(this->ui->sliderLaserCutoff, SIGNAL(valueChanged(int)), this, SLOT(setLaserCutoff(int)));
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

void MainWindow::setPolygonResolution(int value)
{
    // Update den Titel der GroupBox
    QString title = QString("Auflösung: ") + QString::number(value);
    this->ui->groupBox_Resolution->setTitle(title);

    // Leite den Wert an das GLWidget weiter
    // (ui->openGLWidget ist der Name des Widgets aus der .ui Datei)
    this->ui->openGLWidget->setPolygonResolution(value);
}

// --- NEU HINZUGEFÜGT ---
void MainWindow::setLaserCutoff(int value)
{
    // 1. Setze die globale Config-Variable
    Config::laserCutoff = static_cast<float>(value);

    // 2. Update den Titel der GroupBox
    // \xC2\xB0 ist die UTF-8 Kodierung für das Grad-Symbol °
    QString title = QString("Laser Cutoff: ") + QString::number(value) + "\xC2\xB0";
    this->ui->groupBox_Laser->setTitle(title);
}
// --- ENDE NEU ---

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

void MainWindow::setCoordinateSystem(bool value)
{
    Config::showCoordinateSystem = value;
}

void MainWindow::set3DOrbits(bool value)
{
    Config::show3DOrbits = value;
}

void MainWindow::setLocalOrbits(bool value)
{
    Config::localOrbits = value;
}

/* Qt mouse and keyboard events */
void MainWindow::keyPressEvent(QKeyEvent* event)
{
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