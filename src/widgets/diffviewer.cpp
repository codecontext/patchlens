#include "diffviewer.h"

#include <QFontDatabase>

DiffViewer::DiffViewer(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);

    setFont(
        QFontDatabase::systemFont(
            QFontDatabase::FixedFont));

    setPlainText(
        "Select a file to view changes.");
}


void DiffViewer::setFile(const FileDiff &file)
{
    setPlainText(
        buildUnifiedDiff(file));
}


QString DiffViewer::buildUnifiedDiff(
    const FileDiff &file)
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
            text += " ";
            text += hunk.functionName;
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

    return text;
}