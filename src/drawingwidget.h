#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>

class DrawingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingWidget(QWidget *parent = nullptr);

    void runSlowCH();       // Slow Convex Hull
    void runFastCH();       // Convex Hull
    void clearCanvas();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<QPointF> points;
    QVector<QPointF> hullSlow;
    QVector<QPointF> hullFast;

    static double cross(const QPointF &a, const QPointF &b, const QPointF &c);
    QVector<QPointF> slowConvexHull(const QVector<QPointF> &P);
    QVector<QPointF> convexHull(const QVector<QPointF> &P);

    int slowIterations = 0;
    int fastIterations = 0;

};

#endif // DRAWINGWIDGET_H
