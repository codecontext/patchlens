#pragma once

#include <QMainWindow>

#include "../core/patch/patchmodel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void openPatch();

private:
    void createMenus();

    PatchFile currentPatch;
};
