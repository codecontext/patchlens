#pragma once

#include <QPlainTextEdit>

#include "../core/patch/patchmodel.h"

class DiffViewer : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit DiffViewer(QWidget *parent = nullptr);

    void setFile(const FileDiff &file);

private:
    QString buildUnifiedDiff(const FileDiff &file);
};