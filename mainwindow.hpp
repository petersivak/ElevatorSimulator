#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QResizeEvent>

#include "simulation.hpp"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onSimulationButtonClicked();

protected:
    void resizeEvent(QResizeEvent *);

private:
    void toggleWidgetStates(bool value);

    Ui::MainWindow *ui;

    Simulation *mSimulation;
};

#endif // MAINWINDOW_HPP
