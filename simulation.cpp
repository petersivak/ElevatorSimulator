#include "simulation.hpp"

#include <QTimer>

#include "mymath.hpp"

Simulation::Simulation(int numberOfElevators, int elevatorCapacity, double traffic, double elevatorBreakdown,
                       QGraphicsView *simulationView, QTextEdit *graphValuesView, QGraphicsView *graphView) :
    mNumberOfElevators(numberOfElevators),
    mElevatorCapacity(elevatorCapacity),
    mTraffic(traffic),
    mElevatorBreakdown(elevatorBreakdown),
    mWaitingQueue(),
    mWaitingQueueXPos( ((mNumberOfElevators * (2 * ELEVATOR_SPACE + ELEVATOR_WIDTH)) / 2) - (PERSON_WIDTH / 2) ),
    mSimulationScene(),
    mElevators(),
    mGraphRenderer(graphView, graphValuesView, "time\n(seconds)", "waiting\npersons", 1, 1),
    mIsStopped(false)
{
    simulationView->setScene(&mSimulationScene);

    QPolygonF elevatorPolygon;
    elevatorPolygon << QPointF(ELEVATOR_WIDTH / 2 - 5, 0) << QPointF(ELEVATOR_WIDTH / 2 + 5, 0)
                    << QPointF(ELEVATOR_WIDTH / 2 + 5, 15) << QPointF(ELEVATOR_WIDTH, 15)
                    << QPointF(ELEVATOR_WIDTH, ELEVATOR_HEIGHT) << QPointF(ELEVATOR_WIDTH - 10, ELEVATOR_HEIGHT)
                    << QPointF(ELEVATOR_WIDTH - 10, 35) << QPointF(10, 35)
                    << QPointF(10, ELEVATOR_HEIGHT) << QPointF(0, ELEVATOR_HEIGHT)
                    << QPointF(0, 15) << QPointF(ELEVATOR_WIDTH / 2 - 5, 15);

    // Add elevators to the list of elevators
    for (int i = 0; i < mNumberOfElevators; ++i)
    {
        mElevators.append(new Elevator(*this, i, mSimulationScene, elevatorPolygon));   // Add elevator
    }
}

Simulation::~Simulation()
{
    /*for (int i = 0; i < mElevators.size(); ++i)
    {
        delete mElevators[i];
    }*/
}

void Simulation::planPersonArrival()
{
    if (mTraffic != 0)
    {
        double editedTraffic = mTraffic + ( (((realRandom() % 9) - 4) / 10.0) * mTraffic );
        QTimer::singleShot( 1000 / editedTraffic, this, SLOT(onPersonArrived()) );
    }
}

void Simulation::planElevatorBreakdown()
{
    if (mElevatorBreakdown != 0)
    {
        double editedBreakdown = mElevatorBreakdown + ( (((realRandom() % 9) - 4) / 10.0) * mElevatorBreakdown );
        QTimer::singleShot( 1000 / editedBreakdown, this, SLOT(onElevatorBroke()) );
    }
}

bool Simulation::enterFirstFreeElevator()
{
    // Find first free elevator
    for (QMutableListIterator<Elevator *> it(mElevators); it.hasNext(); )
    {
        Elevator *&elevator = it.next();

        if ( (elevator->state() == Elevator::NORMAL_STATE) && (elevator->occupancy() < mElevatorCapacity) )
        {
            elevator->incrementOccupancy();

            // If the elevator is full, it should plan leave
            if (elevator->occupancy() == mElevatorCapacity)
            {
                elevator->planLeave(500);
            }
            // If the elevator is not yet full
            else
            {
                elevator->planLeave(2000);
            }

            return true;
        }
    }

    return false;
}

void Simulation::enterWaitingQueue()
{
    QGraphicsEllipseItem *ellipseItem = mSimulationScene.addEllipse(
                mWaitingQueueXPos, ELEVATOR_HEIGHT + 5 + (mWaitingQueue.size() * 20), PERSON_WIDTH, PERSON_HEIGHT);
    ellipseItem->setBrush(Qt::blue);

    mWaitingQueue.enqueue(WaitingPerson(ellipseItem));
}

void Simulation::onPersonArrived()
{
    if (mIsStopped) return;

    if (!enterFirstFreeElevator()) enterWaitingQueue();

    planPersonArrival();
}

void Simulation::onElevatorBroke()
{
    if (mIsStopped) return;

    // Choose random elevator to break (try at least 5 times if cannot find any non-broken one)
    for (int i = 0; i < 5; ++i)
    {
        int elevatorNumberToBreak = realRandom() % mNumberOfElevators;
        if (mElevators[elevatorNumberToBreak]->state() == Elevator::BROKEN_STATE) continue;
        else
        {
            mElevators[elevatorNumberToBreak]->breakdown(); break;
        }
    }

    planElevatorBreakdown();
}

void Simulation::stop()
{
    mIsStopped = true;

    for (int i = 0; i < mElevators.size(); ++i)
    {
        mElevators[i]->stopSimulation();
    }
}

bool Simulation::isStopped() const { return mIsStopped; }

void Simulation::planGraphUpdate()
{
    QTimer::singleShot(1000, this, SLOT(onGraphUpdate()));
}

void Simulation::onGraphUpdate()
{
    if (mIsStopped) return;

    int currentWaitingPersons = 0;
    for (QMutableListIterator<Elevator *> it(mElevators); it.hasNext(); )
    {
        Elevator *&elevator = it.next();

        currentWaitingPersons += mWaitingQueue.size();
    }
    mGraphRenderer.addValue(mWaitingQueue.size());

    planGraphUpdate();
}

GraphRenderer &Simulation::graphRenderer() { return mGraphRenderer; }

int Simulation::elevatorCapacity() const { return mElevatorCapacity; }
QQueue<WaitingPerson> &Simulation::waitingQueue() { return mWaitingQueue; }
