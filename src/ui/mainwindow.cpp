#include "mainwindow.h"

#include "../core/patch/patchparser.h"

#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("PatchLens");
    resize(1200, 800);

    createMenus();
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *openAction = fileMenu->addAction("&Open Patch...");
    connect(openAction, &QAction::triggered,
            this, &MainWindow::openPatch);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("E&xit");
    connect(exitAction, &QAction::triggered,
            this, &QWidget::close);
}

void MainWindow::openPatch()
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            "Open Patch",
            QString(),
            "Patch Files (*.patch *.diff);;All Files (*)");

    if (fileName.isEmpty())
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(
            this,
            "Error",
            "Failed to open patch file.");

        return;
    }

    QTextStream stream(&file);
    QString patchText = stream.readAll();

    PatchParser parser;
    currentPatch = parser.parse(patchText);

    QString summary;

    summary += "Subject : " + currentPatch.subject + "\n";
    summary += "Author  : " + currentPatch.author + "\n";
    summary += "Date    : " + currentPatch.date + "\n";
    summary += "Files   : " + QString::number(currentPatch.files.size());

    QMessageBox::information(
        this,
        "Patch Loaded",
        summary);
}
