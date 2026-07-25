#include "mainwindow.h"

#include "../core/patch/patchparser.h"

#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
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


    diffViewer = new QPlainTextEdit(splitter);

    diffViewer->setReadOnly(true);

    diffViewer->setPlainText(
        "Select a file to view changes.");

    diffViewer->setFont(
        QFontDatabase::systemFont(
            QFontDatabase::FixedFont));


    splitter->addWidget(fileList);
    splitter->addWidget(diffViewer);


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

    showFileDetails(
        currentPatch.files[index]);
}


void MainWindow::showFileDetails(const FileDiff &file)
{
    QString text;

    for (const DiffHunk &hunk : file.hunks)
    {
        text += QString(
            "@@ -%1,%2 +%3,%4 @@")
                    .arg(hunk.oldStart)
                    .arg(hunk.oldCount)
                    .arg(hunk.newStart)
                    .arg(hunk.newCount);

        if (!hunk.functionName.isEmpty())
        {
            text += " " + hunk.functionName;
        }

        text += "\n";

        for (const DiffLine &line : hunk.lines)
        {
            switch (line.type)
            {
            case DiffLineType::Context:
                text += " ";
                break;

            case DiffLineType::Added:
                text += "+";
                break;

            case DiffLineType::Removed:
                text += "-";
                break;
            }

            text += line.text;
            text += "\n";
        }

        text += "\n";
    }

    diffViewer->setPlainText(text);
}