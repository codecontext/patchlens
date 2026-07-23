#include "mainwindow.h"

#include "../core/patch/patchparser.h"

#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("PatchLens");
    resize(1200, 800);

    createMenus();
    createWelcomeScreen();
}


void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *openAction = fileMenu->addAction("&Open Patch...");
    connect(openAction,
            &QAction::triggered,
            this,
            &MainWindow::openPatch);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("E&xit");
    connect(exitAction,
            &QAction::triggered,
            this,
            &QWidget::close);
}


void MainWindow::createWelcomeScreen()
{
    welcomeWidget = new QWidget(this);

    auto *layout = new QVBoxLayout(welcomeWidget);

    auto *title = new QLabel(
        "PatchLens\n\nGit Patch Viewer",
        welcomeWidget);

    title->setAlignment(Qt::AlignCenter);


    openButton = new QPushButton(
        "Open Patch File",
        welcomeWidget);


    connect(openButton,
            &QPushButton::clicked,
            this,
            &MainWindow::openPatch);


    layout->addStretch();

    layout->addWidget(
        title);

    layout->addWidget(
        openButton,
        0,
        Qt::AlignCenter);

    layout->addStretch();


    setCentralWidget(welcomeWidget);
}


void MainWindow::createWorkspace()
{
    QWidget *workspace = new QWidget(this);

    auto *mainLayout = new QVBoxLayout(workspace);

    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);


    patchNameLabel = new QLabel(workspace);

    patchNameLabel->setText("Patch:");

    patchNameLabel->setMinimumHeight(25);


    splitter = new QSplitter(workspace);


    fileList = new QListWidget(splitter);

    connect(fileList,
            &QListWidget::itemClicked,
            this,
            &MainWindow::fileSelected);

    placeholder = new QLabel(
        "Select a file to view changes.",
        splitter);

    placeholder->setAlignment(
        Qt::AlignCenter);


    splitter->addWidget(fileList);
    splitter->addWidget(placeholder);


    splitter->setSizes(
    {
        300,
        900
    });


    mainLayout->addWidget(
        patchNameLabel,
        0);


    mainLayout->addWidget(
        splitter,
        1);


    setCentralWidget(workspace);
}


void MainWindow::populateFileList()
{
    fileList->clear();


    for (const FileDiff &file : currentPatch.files)
    {
        fileList->addItem(
            file.newPath);
    }
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
    {
        return;
    }


    QFile file(fileName);


    if (!file.open(
            QIODevice::ReadOnly |
            QIODevice::Text))
    {
        QMessageBox::critical(
            this,
            "Error",
            "Failed to open patch file.");

        return;
    }


    QTextStream stream(&file);

    QString patchText =
        stream.readAll();


    PatchParser parser;

    currentPatch =
        parser.parse(patchText);


    createWorkspace();


    patchNameLabel->setText(
        "Patch: " +
        QFileInfo(fileName).fileName());


    populateFileList();
}

void MainWindow::fileSelected(QListWidgetItem *item)
{
    int index = fileList->row(item);

    if (index < 0 ||
        index >= currentPatch.files.size())
    {
        return;
    }

    showFileDetails(currentPatch.files[index]);
}


void MainWindow::showFileDetails(const FileDiff &file)
{
    QString text;

    text += "File:     " + file.newPath + "\n";
    text += "Old Path: " + file.oldPath + "\n";
    text += "New Path: " + file.newPath + "\n";
    text += "Hunks:    " + QString::number(file.hunks.size());


    placeholder->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    placeholder->setText(text);
}