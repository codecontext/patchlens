#pragma once

#include <QWidget>

#include "../core/patch/patchmodel.h"

class QComboBox;
class QFrame;
class QPlainTextEdit;
class QScrollBar;

class DiffViewer : public QWidget
{
    Q_OBJECT

public:
    explicit DiffViewer(QWidget *parent = nullptr);

    void setFile(const FileDiff &file);

private slots:
    void viewModeChanged(int index);

private:
    enum class ViewMode
    {
        Unified,
        SideBySide
    };

    void setupUi();
    void connectScrollBars();

    void renderFile();

    void renderUnified(
        const FileDiff &file);

    void renderSideBySide(
        const FileDiff &file);

    void updateHorizontalScrollBar();

    QComboBox *viewModeCombo = nullptr;

    QPlainTextEdit *unifiedView = nullptr;

    QPlainTextEdit *oldView = nullptr;
    QPlainTextEdit *newView = nullptr;

    QFrame *separator = nullptr;

    QScrollBar *horizontalScrollBar = nullptr;

    FileDiff currentFile;

    ViewMode viewMode =
        ViewMode::SideBySide;
};