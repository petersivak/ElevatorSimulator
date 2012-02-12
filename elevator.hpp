#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP

#include <QQueue>
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QTimer>

#include "waitingperson.hpp"

class Simulation;   // Forward declaration

class Elevator : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        NORMAL_STATE,
        AWAY_STATE,
        BROKEN_STATE
    };

    Elevator(Simulation& simulation, int number, QGraphicsScene &scene, const QPolygonF &elevatorPolygon);
    ~Elevator();

    int occupancy() const;
    State state() const;

    void incrementOccupancy();
    void enterFromWaitingQueue();
    void zeroOccupancy();
    int zeroWaitingQueue();
    void setState(State state);
    void breakdown();

    QGraphicsPolygonItem *polygonItem();
    QGraphicsTextItem *textItem();
    QGraphicsEllipseItem *ellipseItem();
    QGraphicsTextItem *awayTextItem();
    QGraphicsTextItem *brokenTextItem();

    void planLeave(double coef);
    void planArrival();
    void planRepair();
    void stopSimulation();

    void centerTextItem();
    void centerAwayTextItem();
    void centerBrokenTextItem();

public slots:
    void onLeave();
    void onArrival();
    void onRepair();

private:
    Simulation& mSimulation;
    int mNumber;
    int mOccupancy;
    State mState;
    QGraphicsScene &mScene;
    QGraphicsPolygonItem *mPolygonItem;   // Polygon graphics item
    QGraphicsTextItem *mTextItem;         // Text graphics item
    QGraphicsEllipseItem *mEllipseItem;   // Ellipse graphics item
    QGraphicsTextItem *mAwayTextItem;     // Away text graphics item
    QGraphicsTextItem *mBrokenTextItem;   // Broken text graphics item
    bool mIsAllowedLeave;
    bool mIsSimulationStopped;

    QTimer mLeaveTimer;
};

#endif // ELEVATOR_HPP
