#include "diffviewer.h"

#include <QFontDatabase>
#include <QTextCharFormat>
#include <QTextCursor>


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
    clear();

    QTextCursor cursor(document());

    for (const DiffHunk &hunk : file.hunks)
    {
        QString hunkHeader =
            QString("@@ -%1,%2 +%3,%4 @@")
                .arg(hunk.oldStart)
                .arg(hunk.oldCount)
                .arg(hunk.newStart)
                .arg(hunk.newCount);

        if (!hunk.functionName.isEmpty())
        {
            hunkHeader += " ";
            hunkHeader += hunk.functionName;
        }

        hunkHeader += "\n";

        QTextCharFormat hunkFormat;

        hunkFormat.setForeground(
            QColor("#0969da"));

        hunkFormat.setBackground(
            QColor("#ddf4ff"));

        hunkFormat.setFontWeight(
            QFont::Bold);

        cursor.insertText(
            hunkHeader,
            hunkFormat);


        for (const DiffLine &line : hunk.lines)
        {
            QString oldLineNumber;
            QString newLineNumber;
            QString prefix;

            if (line.oldLine >= 0)
            {
                oldLineNumber =
                    QString::number(line.oldLine);
            }

            if (line.newLine >= 0)
            {
                newLineNumber =
                    QString::number(line.newLine);
            }


            switch (line.type)
            {
            case DiffLineType::Context:
                prefix = " ";
                break;

            case DiffLineType::Added:
                prefix = "+";
                break;

            case DiffLineType::Removed:
                prefix = "-";
                break;
            }


            QString formattedLine =
                QString("%1 %2 %3 %4\n")
                    .arg(oldLineNumber, 5)
                    .arg(newLineNumber, 5)
                    .arg(prefix)
                    .arg(line.text);


            QTextCharFormat lineFormat;

            switch (line.type)
            {
            case DiffLineType::Context:
                break;

            case DiffLineType::Added:
                lineFormat.setForeground(
                    QColor("#1a7f37"));

                lineFormat.setBackground(
                    QColor("#dafbe1"));
                break;

            case DiffLineType::Removed:
                lineFormat.setForeground(
                    QColor("#cf222e"));

                lineFormat.setBackground(
                    QColor("#ffebe9"));
                break;
            }


            cursor.insertText(
                formattedLine,
                lineFormat);
        }

        cursor.insertText("\n");
    }

    setTextCursor(cursor);
}