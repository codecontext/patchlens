#pragma once

#include <QMainWindow>

#include "../core/patch/patchmodel.h"

class QLabel;
class QListWidget;
class QPushButton;
class QSplitter;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void openPatch();

private:
    void createMenus();

    void createWelcomeScreen();
    void createWorkspace();
    void populateFileList();

    QWidget *welcomeWidget = nullptr;

    QLabel *patchNameLabel = nullptr;

    QSplitter *splitter = nullptr;
    QListWidget *fileList = nullptr;
    QLabel *placeholder = nullptr;

    QPushButton *openButton = nullptr;

    PatchFile currentPatch;
};
