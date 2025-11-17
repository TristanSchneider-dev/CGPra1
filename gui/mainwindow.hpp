#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void setAnimationSpeed(int value);
    void setLocalRotation(bool value);
    void setGlobalRotation(bool value);
    void setSunLight(bool value);
    void setshowWireframe(bool value);
    void setshowOrbits(bool value);
    void setCoordinateSystem(bool value);
    void set3DOrbits(bool value);
    void setLocalOrbits(bool value);
    void setPolygonResolution(int value);

    void setLaserCutoff(int value);


protected:
    virtual void keyPressEvent(QKeyEvent* event);
};

#endif // MAINWINDOW_H