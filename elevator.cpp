#include "elevator.hpp"

#include <QTimer>

#include "mymath.hpp"
#include "simulation.hpp"

Elevator::Elevator(Simulation& simulation, int number, QGraphicsScene &scene, const QPolygonF &elevatorPolygon) :
    mSimulation(simulation),
    mNumber(number),
    mOccupancy(0),
    mState(NORMAL_STATE),
    mScene(scene),
    mPolygonItem(0),
    mTextItem(0),
    mEllipseItem(0),
    mAwayTextItem(0),
    mBrokenTextItem(0),
    mIsAllowedLeave(false),
    mIsSimulationStopped(false),
    mLeaveTimer(this)
{
    // Constructs leave timer
    mLeaveTimer.setSingleShot(true);
    QObject::connect(&mLeaveTimer, SIGNAL(timeout()), this, SLOT(onLeave()));

    // Polygon item
    mPolygonItem = mScene.addPolygon(elevatorPolygon);
    mPolygonItem->setPos( Simulation::ELEVATOR_SPACE + (mNumber * Simulation::ELEVATOR_FULL_WIDTH), 0 );
    mPolygonItem->setBrush(Qt::yellow);

    // Text item
    mTextItem = mScene.addText("0");
    centerTextItem();
    mTextItem->hide();

    // Ellipse item
    mEllipseItem = mScene.addEllipse(Simulation::ELEVATOR_SPACE + 15 + (mNumber * Simulation::ELEVATOR_FULL_WIDTH), 40,
                                     Simulation::PERSON_WIDTH, Simulation::PERSON_HEIGHT);
    mEllipseItem->setBrush(Qt::green);
    mEllipseItem->hide();

    // Away text item
    mAwayTextItem = mScene.addText("Away");
    centerAwayTextItem();
    mAwayTextItem->hide();

    // Broken text item
    mBrokenTextItem = mScene.addText("Broken");
    centerBrokenTextItem();
    mBrokenTextItem->hide();
}

Elevator::~Elevator()
{
    QObject::disconnect(&mLeaveTimer, SIGNAL(timeout()), this, SLOT(onLeave()));
}

void Elevator::centerTextItem()
{
    mTextItem->setPos(Simulation::ELEVATOR_SPACE + (Simulation::ELEVATOR_WIDTH / 2)
                      + (mNumber * Simulation::ELEVATOR_FULL_WIDTH) - mTextItem->boundingRect().width() / 2, 13);
}

void Elevator::centerAwayTextItem()
{
    mAwayTextItem->setPos(Simulation::ELEVATOR_SPACE + (Simulation::ELEVATOR_WIDTH / 2)
                      + (mNumber * Simulation::ELEVATOR_FULL_WIDTH) - mAwayTextItem->boundingRect().width() / 2, 13);
}

void Elevator::centerBrokenTextItem()
{
    mBrokenTextItem->setPos(Simulation::ELEVATOR_SPACE + (Simulation::ELEVATOR_WIDTH / 2)
                      + (mNumber * Simulation::ELEVATOR_FULL_WIDTH) - mBrokenTextItem->boundingRect().width() / 2, 13);
}

int Elevator::occupancy() const { return mOccupancy; }
Elevator::State Elevator::state() const { return mState; }

void Elevator::incrementOccupancy()
{
    ++mOccupancy;

    if ( !textItem()->isVisible() ) textItem()->show();
    textItem()->setPlainText( QString::number(mOccupancy) );
    if ( !ellipseItem()->isVisible() ) ellipseItem()->show();

    centerTextItem();
}

void Elevator::enterFromWaitingQueue()
{
    // One person enter elevator
    delete mSimulation.waitingQueue().dequeue().ellipseItem();

    // The other persons move one step forward
    for (QMutableListIterator<WaitingPerson> it(mSimulation.waitingQueue()); it.hasNext(); )
    {
        WaitingPerson &waitingPerson = it.next();

        waitingPerson.moveForward();
    }

    // If the elevator is full, it should plan leave
    if (mOccupancy == mSimulation.elevatorCapacity())
    {
        planLeave(2000);
    }
}

void Elevator::zeroOccupancy()
{
    mOccupancy = 0;

    textItem()->hide();
    textItem()->setPlainText("0");
    centerTextItem();
    ellipseItem()->hide();
}

int Elevator::zeroWaitingQueue() { /*int temp = mWaitingQueue; mWaitingQueue = 0; return temp;*/ }
void Elevator::setState(Elevator::State state)
{
    mState = state;

    switch (mState)
    {
    case NORMAL_STATE:
        mPolygonItem->show();
        //mTextItem->show();
        //mEllipseItem->show();
        mAwayTextItem->hide();
        mBrokenTextItem->hide();
        break;

    case AWAY_STATE:
        mPolygonItem->hide();
        mTextItem->hide();
        mEllipseItem->hide();
        mAwayTextItem->show();
        mBrokenTextItem->hide();
        break;

    case BROKEN_STATE:
        mPolygonItem->hide();
        mTextItem->hide();
        mEllipseItem->hide();
        mAwayTextItem->hide();
        mBrokenTextItem->show();
        break;
    }
}

void Elevator::breakdown()
{
    setState(Elevator::BROKEN_STATE);
    if (mOccupancy > 0)
    {
        for (int i = 0; i < mOccupancy; ++i)
        {
            if (!mSimulation.enterFirstFreeElevator()) mSimulation.enterWaitingQueue();
            //incrementWaitingQueue();
        }
    }
    if (mOccupancy > 0)
    {
        // Put the people from elevator to waiting queue
        for (int i = 0; i < mOccupancy; ++i)
        {
            mSimulation.enterWaitingQueue();
        }
    }
    zeroOccupancy();
    planRepair();
}

QGraphicsPolygonItem *Elevator::polygonItem() { return mPolygonItem; }
QGraphicsTextItem *Elevator::textItem() { return mTextItem; }
QGraphicsEllipseItem *Elevator::ellipseItem() { return mEllipseItem; }
QGraphicsTextItem *Elevator::awayTextItem() { return mAwayTextItem; }
QGraphicsTextItem *Elevator::brokenTextItem() { return mBrokenTextItem; }

void Elevator::planLeave(double coef)
{
    double editedCoef = coef + ( (((realRandom() % 9) - 4) / 10.0) * coef );
    mLeaveTimer.start(editedCoef);
}

void Elevator::planArrival()
{
    QTimer::singleShot( realRandom() % 7000, this, SLOT(onArrival()) );
}

void Elevator::stopSimulation()
{
    mIsSimulationStopped = true;
}

void Elevator::onLeave()
{
    if (mIsSimulationStopped) return;

    setState(Elevator::AWAY_STATE);
    zeroOccupancy();
    planArrival();
}

void Elevator::onArrival()
{
    if (mIsSimulationStopped) return;

    setState(Elevator::NORMAL_STATE);

    for (int i = 0; i < mSimulation.elevatorCapacity(); ++i)
    {
        if (mSimulation.waitingQueue().empty()) break;

        enterFromWaitingQueue();
    }
}

void Elevator::planRepair()
{
    QTimer::singleShot( realRandom() % 10000, this, SLOT(onRepair()) );
}

void Elevator::onRepair()
{
    /*if (mIsSimulationStopped) return;

    setState(Elevator::NORMAL_STATE);

    for (int i = 0; i < mSimulation.elevatorCapacity(); ++i)
    {
        if (mWaitingQueue.empty()) break;

        enterFromWaitingQueue();
    }*/
}
