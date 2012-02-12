#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <QList>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTextEdit>

#include "elevator.hpp"
#include "graphrenderer.hpp"

class Simulation : public QObject
{
    Q_OBJECT
public:
    static const int PERSON_WIDTH = 15;
    static const int PERSON_HEIGHT = 15;
    static const int ELEVATOR_WIDTH = 10 + 5 + PERSON_WIDTH + 5 + 10;
    static const int ELEVATOR_HEIGHT = 15 + 20 + 5 + PERSON_HEIGHT + 5;
    static const int ELEVATOR_SPACE = 10;
    static const int ELEVATOR_FULL_WIDTH = ELEVATOR_WIDTH + (2 * ELEVATOR_SPACE);

    Simulation(int numberOfElevators, int elevatorCapacity, double traffic, double elevatorBreakdown,
               QGraphicsView *simulationView, QTextEdit *graphValuesView, QGraphicsView *graphView);
    ~Simulation();

    void planPersonArrival();
    void planElevatorBreakdown();
    bool enterFirstFreeElevator();
    void enterWaitingQueue();
    void stop();
    bool isStopped() const;
    void planGraphUpdate();
    GraphRenderer &graphRenderer();

    int elevatorCapacity() const;
    QQueue<WaitingPerson> &waitingQueue();

public slots:
    void onPersonArrived();
    void onElevatorBroke();
    void onGraphUpdate();

private:
    int mNumberOfElevators;
    int mElevatorCapacity;
    double mTraffic;             // Value from 0.0 (minimum) to 0.99 (maximum)
    double mElevatorBreakdown;   // Value from 0.0 (minimum) to 0.99 (maximum)

    QQueue<WaitingPerson> mWaitingQueue;
    int mWaitingQueueXPos;

    QGraphicsScene mSimulationScene;
    QList<Elevator *> mElevators;
    GraphRenderer mGraphRenderer;

    bool mIsStopped;
};

#endif // SIMULATION_HPP
