#pragma once

#include <QMainWindow>

#include "../core/patch/patchmodel.h"
#include "../widgets/diffviewer.h"

class QLabel;
class QListWidget;
class QListWidgetItem;
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
    void fileSelected(QListWidgetItem *item);

private:
    void createMenus();

    void createWelcomeScreen();
    void createWorkspace();
    void populateFileList();

    QWidget *welcomeWidget = nullptr;

    QLabel *patchNameLabel = nullptr;

    QSplitter *splitter = nullptr;
    QListWidget *fileList = nullptr;
    DiffViewer *diffViewer = nullptr;

    QPushButton *openButton = nullptr;

    PatchFile currentPatch;
};