#ifndef WAITINGPERSON_HPP
#define WAITINGPERSON_HPP

#include <QGraphicsEllipseItem>

class WaitingPerson
{
public:
    WaitingPerson(QGraphicsEllipseItem *ellipseItem);
    ~WaitingPerson();

    QGraphicsEllipseItem *ellipseItem();

    void moveForward();

private:
    QGraphicsEllipseItem *mEllipseItem;    // Ellipse graphics item
};

#endif // WAITINGPERSON_HPP
