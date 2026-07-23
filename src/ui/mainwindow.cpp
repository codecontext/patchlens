#include "mainwindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("PatchLens");

    auto *label = new QLabel(
        "PatchLens - Git Patch Viewer",
        this
    );

    label->setAlignment(Qt::AlignCenter);

    setCentralWidget(label);

    resize(800, 600);
}
