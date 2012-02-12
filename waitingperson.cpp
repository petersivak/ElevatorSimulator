#include "waitingperson.hpp"

WaitingPerson::WaitingPerson(QGraphicsEllipseItem *ellipseItem) :
    mEllipseItem(ellipseItem)
{
}

WaitingPerson::~WaitingPerson()
{
    //delete mEllipseItem; mEllipseItem = 0;
}

QGraphicsEllipseItem *WaitingPerson::ellipseItem() { return mEllipseItem; }

void WaitingPerson::moveForward()
{
    mEllipseItem->setY(mEllipseItem->y() - 20);
}
