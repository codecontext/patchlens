#pragma once

#include <QWidget>

#include "../core/patch/patchmodel.h"

class QFrame;
class QPlainTextEdit;
class QScrollBar;

class DiffViewer : public QWidget
{
    Q_OBJECT

public:
    explicit DiffViewer(QWidget *parent = nullptr);

    void setFile(const FileDiff &file);

private:
    void setupUi();
    void connectScrollBars();

    void renderSideBySide(const FileDiff &file);

    QPlainTextEdit *oldView = nullptr;
    QPlainTextEdit *newView = nullptr;

    QFrame *separator = nullptr;

    QScrollBar *horizontalScrollBar = nullptr;
};