#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mSimulation(0)
{
    ui->setupUi(this);

    ui->simulationLabel->setEnabled(false);
    ui->graphValuesLabel->setEnabled(false);
    ui->graphLabel->setEnabled(false);
    ui->simulationView->setEnabled(false);
    ui->graphView->setEnabled(false);

    //move(QApplication::desktop()->screen()->rect().center()-this->rect().center());   // Center window
}

MainWindow::~MainWindow()
{
    if (mSimulation != 0)
    {
        delete mSimulation; mSimulation = 0;
    }
    delete ui; ui = 0;
}

void MainWindow::onSimulationButtonClicked()
{
    // If simulation is not running
    if (mSimulation == 0 || mSimulation->isStopped())
    {
        // Delete previous simulation if needed
        if (mSimulation != 0)
        {
            delete mSimulation; mSimulation = 0;
        }

        ui->simulationButton->setText("Stop simulation");
        toggleWidgetStates(false);

        mSimulation = new Simulation(ui->numberOfElevatorsSpinBox->value(), ui->elevatorCapacitySpinBox->value(),
                                     (double) ui->trafficSlider->value() / 10,
                                     (double) ui->breakdownSlider->value() / 20, ui->simulationView,
                                     ui->graphValuesView, ui->graphView);

        mSimulation->planPersonArrival();
        mSimulation->planElevatorBreakdown();
        mSimulation->planGraphUpdate();
    }
    // If simulation is running
    else
    {
        mSimulation->stop();

        ui->simulationButton->setText("Simulate");
        toggleWidgetStates(true);
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    if (ui->graphView->isEnabled()) mSimulation->graphRenderer().updateSize();
}

void MainWindow::toggleWidgetStates(bool value)
{
    if (!ui->simulationView->isEnabled()) ui->simulationView->setEnabled(true);
    if (!ui->graphView->isEnabled()) ui->graphView->setEnabled(true);
    ui->numberOfElevatorsLabel->setEnabled(value);
    ui->numberOfElevatorsSpinBox->setEnabled(value);
    ui->elevatorCapacityLabel->setEnabled(value);
    ui->elevatorCapacitySpinBox->setEnabled(value);
    ui->trafficLabel->setEnabled(value);
    ui->trafficLowLabel->setEnabled(value);
    ui->trafficSlider->setEnabled(value);
    ui->trafficHighLabel->setEnabled(value);
    ui->breakdownLabel->setEnabled(value);
    ui->breakdownLowLabel->setEnabled(value);
    ui->breakdownSlider->setEnabled(value);
    ui->breakdownHighLabel->setEnabled(value);
    ui->simulationLabel->setEnabled(!value);
    ui->graphValuesLabel->setEnabled(!value);
    ui->graphLabel->setEnabled(!value);
}
