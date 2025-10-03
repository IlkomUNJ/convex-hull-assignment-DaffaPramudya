#include "drawingwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <algorithm>
#include <map>
#include <cmath>

DrawingWidget::DrawingWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(600, 400);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

// Tambah titik dengan klik mouse
void DrawingWidget::mousePressEvent(QMouseEvent *event)
{
    points.append(event->pos());
    hullSlow.clear();
    hullFast.clear();
    update();
}

void DrawingWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // gambar semua titik
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    for (const QPointF &pt : points)
        p.drawEllipse(pt, 3, 3);

    // gambar hull slow (merah)
    if (hullSlow.size() > 1) {
        p.setPen(QPen(Qt::red, 2));
        p.setBrush(Qt::NoBrush);                 // stroke only
        QPolygonF poly(hullSlow);
        p.drawPolygon(poly);
        p.drawLine(hullSlow.last(), hullSlow.first());
    }

    // gambar hull fast (biru)
    if (hullFast.size() > 1) {
        p.setPen(QPen(Qt::blue, 2, Qt::DashLine));
        p.setBrush(Qt::NoBrush);                 // stroke only
        QPolygonF poly(hullFast);
        p.drawPolygon(poly);
        p.drawLine(hullFast.last(), hullFast.first());
    }

    // tampilkan jumlah iterasi
    p.setPen(Qt::black);
    p.setFont(QFont("Arial", 12));
    p.drawText(10, 20, QString("SlowCH Iterations: %1").arg(slowIterations));
    p.drawText(10, 40, QString("FastCH Iterations: %1").arg(fastIterations));
}

void DrawingWidget::runSlowCH()
{
    slowIterations = 0;                   // reset sebelum hitung
    hullSlow = slowConvexHull(points);
    update();
}

void DrawingWidget::runFastCH()
{
    fastIterations = 0;                    // reset
    hullFast = convexHull(points);
    update();
}

void DrawingWidget::clearCanvas()
{
    points.clear();
    hullSlow.clear();
    hullFast.clear();
    slowIterations = 0;
    fastIterations = 0;
    update();
}

double DrawingWidget::cross(const QPointF &a, const QPointF &b, const QPointF &c)
{
    return (b.x()-a.x())*(c.y()-a.y()) - (b.y()-a.y())*(c.x()-a.x());
}

struct PointComparator {
    bool operator()(const QPointF &a, const QPointF &b) const {
        if (a.x() < b.x()) return true;
        if (a.x() > b.x()) return false;
        return a.y() < b.y();
    }
};

// SLOWCONVEXHULL
QVector<QPointF> DrawingWidget::slowConvexHull(const QVector<QPointF> &P)
{
    int n = P.size();
    if (n < 3) return P;

    // pakai comparator custom
    std::multimap<QPointF, QPointF, PointComparator> edges;

    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            if (i==j) continue;
            bool valid = true;
            for (int k=0;k<n;k++){
                if (k==i || k==j) continue;
                ++slowIterations;  // HITUNG iterasi
                if (cross(P[i], P[j], P[k]) > 0) { // r di kiri
                    valid = false;
                    break;
                }
            }
            if (valid) edges.insert({P[i], P[j]});
        }
    }

    // Bangun urutan L searah jarum jam
    int start = 0;
    for (int i=1;i<n;i++){
        if (P[i].x() < P[start].x() ||
            (P[i].x() == P[start].x() && P[i].y() < P[start].y()))
            start = i;
    }

    QVector<QPointF> L;
    QPointF cur = P[start];
    L << cur;

    while (true) {
        auto range = edges.equal_range(cur);
        if (range.first == range.second) break;

        QPointF next;
        bool found = false;
        for (auto it = range.first; it != range.second; ++it) {
            next = it->second;
            if (L.size() < 2 || next != L[L.size()-2]) {
                found = true;
                break;
            }
        }

        if (!found) break;
        if (next == L.first()) break;
        L << next;
        cur = next;

        if (L.size() > n) break;
    }

    return L;
}


// CONVEXHULL
QVector<QPointF> DrawingWidget::convexHull(const QVector<QPointF> &P)
{
    int n = P.size();
    if (n < 3) return P;

    QVector<QPointF> pts = P;
    std::sort(pts.begin(), pts.end(), [](const QPointF &a, const QPointF &b){
        return (a.x()<b.x()) || (a.x()==b.x() && a.y()<b.y());
    });

    QVector<QPointF> upper, lower;

    // upper hull
    upper.push_back(pts[0]);
    upper.push_back(pts[1]);
    for (int i=2;i<n;i++){
        upper.push_back(pts[i]);
        while (upper.size()>2 && cross(upper[upper.size()-3], upper[upper.size()-2], upper.back()) >=0){
            ++fastIterations;       // HITUNG iterasi
            upper.remove(upper.size()-2);
        }
    }

    // lower hull
    lower.push_back(pts[n-1]);
    lower.push_back(pts[n-2]);
    for (int i=n-3;i>=0;i--){
        lower.push_back(pts[i]);
        while (lower.size()>2 && cross(lower[lower.size()-3], lower[lower.size()-2], lower.back()) >=0){
            ++fastIterations;       // HITUNG iterasi
            lower.remove(lower.size()-2);
        }
    }

    // hilangkan titik duplikat pertama & terakhir
    lower.pop_front();
    lower.pop_back();

    // gabungkan
    QVector<QPointF> L = upper;
    L += lower;
    return L;
}
