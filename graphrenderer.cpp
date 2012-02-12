#include "graphrenderer.hpp"

GraphRenderer::GraphRenderer(QGraphicsView *view, QTextEdit *graphValuesView, const QString &xText,
                             const QString &yText, int xStep, int yStep) :
    mView(view),
    mGraphValuesView(graphValuesView),
    mScene(),
    mXTextItem(mScene.addText(xText)),
    mYTextItem(mScene.addText(yText)),
    mXStep(xStep),
    mYStep(yStep),
    mValues(),
    mHighestValue(0),
    mXLineItems(),
    mYLineItem(0),
    mXTextItems(),
    mYLineTextItem(0),
    mAxisPolygonItem(0),
    mLineItems(),
    mRectWidth(0),
    mRectHeight(0)
{
    mView->setScene(&mScene);

    mValues.append(0);

    mGraphValuesView->append("<center>0 0</center>");

    QGraphicsLineItem *xLineItem = mScene.addLine(0, -5, 0, 5);
    mXLineItems.append(xLineItem);

    QGraphicsTextItem *xTextItem = mScene.addText("0");
    xTextItem->setPos(-(xTextItem->boundingRect().width() / 2), 0);
    mXTextItems.append(xTextItem);

    recomputeRectSize();
    recomputeAxisPolygonAndTexts();
    recomputeSceneRect();
}

GraphRenderer::~GraphRenderer()
{
    mGraphValuesView->clear();
}

void GraphRenderer::addValue(int value)
{
    if (value > mHighestValue) mHighestValue = value;
    mValues.append(value);

    mGraphValuesView->append(QString::number(mValues.size() - 1) + " " + QString::number(value));

    // Recompute previous line items
    for (int i = 0; i < mLineItems.size(); ++i)
    {
        if (mHighestValue != 0)
        {
            mLineItems[i]->setLine( mRectWidth * ( (double) i / (mLineItems.size() + 1)),
                                    -mRectHeight * ( (double) mValues[i] / mHighestValue),
                                    mRectWidth * ( (double) (i + 1) / (mLineItems.size() + 1)),
                                    -mRectHeight * ( (double) mValues[i + 1] / mHighestValue ) );
        }
        else
        {
            mLineItems[i]->setLine( mRectWidth * ( (double) i / (mLineItems.size() + 1)),
                                    0,
                                    mRectWidth * ( (double) (i + 1) / (mLineItems.size() + 1)),
                                    0);
        }
    }
    // Add new line item
    QGraphicsLineItem *lineItem = 0;
    if (mLineItems.size() > 0)
    {
        if (mHighestValue != 0)
        {
            lineItem = mScene.addLine( mLineItems[mLineItems.size() - 1]->line().x2(),
                                       mLineItems[mLineItems.size() - 1]->line().y2(),
                                       mRectWidth,
                                       -mRectHeight * ( (double) value / mHighestValue ) );
        }
        else
        {
            lineItem = mScene.addLine( mLineItems[mLineItems.size() - 1]->line().x2(),
                                       mLineItems[mLineItems.size() - 1]->line().y2(),
                                       mRectWidth,
                                       0 );
        }
    }
    else
    {
        if (mHighestValue != 0)
        {
            lineItem = mScene.addLine(0, 0, mRectWidth, -mRectHeight);
        }
        else
        {
            lineItem = mScene.addLine(0, 0, mRectWidth, 0);
        }
    }
    mLineItems.append(lineItem);
    QPen pen;
    pen.setWidth(5);
    pen.setColor(Qt::red);
    lineItem->setPen(pen);

    // Recompute previous x line items
    for (int i = 0; i < mXLineItems.size(); ++i)
    {
        mXLineItems[i]->setLine( mRectWidth * ( (double) i / mXLineItems.size()),
                                 -5,
                                 mRectWidth * ( (double) i / mXLineItems.size()),
                                 5 );
    }
    // Add new x line item
    QGraphicsLineItem *xLineItem = mScene.addLine(mRectWidth, -5, mRectWidth, 5);
    mXLineItems.append(xLineItem);

    // Recompute previous x text items
    for (int i = 0; i < mXTextItems.size(); ++i)
    {
        mXTextItems[i]->setPos( (mRectWidth * ( (double) i / mXTextItems.size())) -
                                (mXTextItems[i]->boundingRect().width() / 2), 0 );
    }
    // Add new x text item
    QGraphicsTextItem *xTextItem = mScene.addText( QString::number(mXLineItems.size() - 1) );
    xTextItem->setPos(mRectWidth - (xTextItem->boundingRect().width() / 2), 0);
    mXTextItems.append(xTextItem);

    // Add y line and text item, if none there yet
    if (mYLineItem == 0 && mHighestValue > 0)
    {
        mYLineItem = mScene.addLine(-5, -mRectHeight, 5, -mRectHeight);
        mYLineTextItem = mScene.addText( QString::number(mHighestValue) );
        mYLineTextItem->setPos( -mYLineTextItem->boundingRect().width() - 1,
                                -mRectHeight - (mYLineTextItem->boundingRect().height() / 2) );
    }
    // Else update that value
    else if (mYLineItem != 0)
    {
        mYLineTextItem->setPlainText( QString::number(mHighestValue) );
        mYLineTextItem->setPos( -mYLineTextItem->boundingRect().width() - 1,
                                -mRectHeight - (mYLineTextItem->boundingRect().height() / 2) );
    }
}

void GraphRenderer::updateSize()
{
    recomputeRectSize();
    recomputeAxisPolygonAndTexts();
    recomputeSceneRect();
    recomputeLineItems();
}

void GraphRenderer::recomputeRectSize()
{
    mRectWidth = mView->width() - ( 10 + (mYTextItem->boundingRect().width() / 2) + 20 +
                                    5 + mXTextItem->boundingRect().width() + 5 );
    mRectHeight = mView->height() - ( 10 + (mXTextItem->boundingRect().height() / 2) + 20 +
                                      5 + mYTextItem->boundingRect().height() + 5 );
}

void GraphRenderer::recomputeAxisPolygonAndTexts()
{
    QPolygonF polygon;
    polygon << QPointF(0, -mRectHeight - 20) << QPointF(-5, -mRectHeight - 10) << QPointF(0, -mRectHeight - 20)
            << QPointF(5, -mRectHeight - 10) << QPointF(0, -mRectHeight - 20) << QPointF(0, 0)
            << QPointF(mRectWidth + 20, 0) << QPointF(mRectWidth + 10, -5) << QPointF(mRectWidth + 20, 0)
            << QPointF(mRectWidth + 10, 5) << QPointF(mRectWidth + 20, 0) << QPointF(0, 0);

    if (mAxisPolygonItem != 0) delete mAxisPolygonItem;
    mAxisPolygonItem = mScene.addPolygon(polygon);
    mXTextItem->setPos(mRectWidth + 20 + 5, -(mXTextItem->boundingRect().height() / 2));
    mYTextItem->setPos(-(mYTextItem->boundingRect().width() / 2), -mRectHeight - 20 - 5 - mYTextItem->boundingRect().height());
}

void GraphRenderer::recomputeSceneRect()
{
    mScene.setSceneRect(mYTextItem->x() - 10, mYTextItem->y() - 5, mView->width() - 2, mView->height() - 2);
}

void GraphRenderer::recomputeLineItems()
{
    // Recompute line items
    for (int i = 0; i < mLineItems.size(); ++i)
    {
        if (mHighestValue != 0)
        {
            mLineItems[i]->setLine( mRectWidth * ( (double) i / (mLineItems.size())),
                                    -mRectHeight * ( (double) mValues[i] / mHighestValue),
                                    mRectWidth * ( (double) (i + 1) / (mLineItems.size())),
                                    -mRectHeight * ( (double) mValues[i + 1] / mHighestValue ) );
        }
        else
        {
            mLineItems[i]->setLine( mRectWidth * ( (double) i / (mLineItems.size())),
                                    0,
                                    mRectWidth * ( (double) (i + 1) / (mLineItems.size())),
                                    0);
        }
    }

    // Recompute previous x line items
    for (int i = 0; i < mXLineItems.size(); ++i)
    {
        mXLineItems[i]->setLine( mRectWidth * ( (double) i / (mXLineItems.size() - 1)),
                                 -5,
                                 mRectWidth * ( (double) i / (mXLineItems.size() - 1)),
                                 5 );
    }

    // Recompute previous x text items
    for (int i = 0; i < mXTextItems.size(); ++i)
    {
        mXTextItems[i]->setPos( (mRectWidth * ( (double) i / (mXTextItems.size() - 1))) -
                                (mXTextItems[i]->boundingRect().width() / 2), 0 );
    }

    // Recompute y value
    if (mYLineItem != 0)
    {
        delete mYLineItem;
        mYLineItem = mScene.addLine(-5, -mRectHeight, 5, -mRectHeight);
        mYLineTextItem->setPos( -mYLineTextItem->boundingRect().width() - 1,
                                -mRectHeight - (mYLineTextItem->boundingRect().height() / 2) );
    }
}
