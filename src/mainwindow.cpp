#include "mainwindow.h"
#include "drawingwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    DrawingWidget *canvas = new DrawingWidget;
    mainLayout->addWidget(canvas);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *btnSlow = new QPushButton("Run SLOWCH");
    QPushButton *btnFast = new QPushButton("Run FastCH");
    QPushButton *btnClear = new QPushButton("Clear");

    btnLayout->addWidget(btnSlow);
    btnLayout->addWidget(btnFast);
    btnLayout->addWidget(btnClear);

    mainLayout->addLayout(btnLayout);
    setCentralWidget(central);

    connect(btnSlow, &QPushButton::clicked, canvas, &DrawingWidget::runSlowCH);
    connect(btnFast, &QPushButton::clicked, canvas, &DrawingWidget::runFastCH);
    connect(btnClear, &QPushButton::clicked, canvas, &DrawingWidget::clearCanvas);

    resize(800,600);
    setWindowTitle("Convex Hull (Slow & Fast)");
}

MainWindow::~MainWindow()
{
    // = default;
}
