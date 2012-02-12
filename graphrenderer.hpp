#ifndef GRAPHRENDERER_HPP
#define GRAPHRENDERER_HPP

#include <QString>
#include <QList>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QTextEdit>

class GraphRenderer
{
public:
    GraphRenderer(QGraphicsView *view, QTextEdit *graphValuesView, const QString &xText,
                  const QString &yText, int xStep, int yStep);
    ~GraphRenderer();

    void addValue(int value);
    void updateSize();

private:
    void recomputeRectSize();
    void recomputeAxisPolygonAndTexts();
    void recomputeSceneRect();
    void recomputeLineItems();

    QGraphicsView *mView;
    QTextEdit *mGraphValuesView;
    QGraphicsScene mScene;
    QGraphicsTextItem *mXTextItem;
    QGraphicsTextItem *mYTextItem;
    int mXStep;
    int mYStep;
    QList<int> mValues;
    int mHighestValue;
    QList<QGraphicsLineItem *> mXLineItems;
    QGraphicsLineItem *mYLineItem;
    QList<QGraphicsTextItem *> mXTextItems;
    QGraphicsTextItem *mYLineTextItem;
    QGraphicsPolygonItem *mAxisPolygonItem;
    QList<QGraphicsLineItem *> mLineItems;
    int mRectWidth;
    int mRectHeight;
};

#endif // GRAPHRENDERER_HPP
