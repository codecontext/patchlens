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
        /*
         * Hunk header
         *
         * Example:
         * @@ -114,11 +114,13 @@ function_name
         */
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


        /*
         * Diff lines
         *
         * Old line | New line | Type | Content
         */
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


            /*
             * Format:
             *
             *   old   new  +/- content
             *
             * Example:
             *
             *   114   114   static DEVICE_API(...)
             *   115         -IRQ_CONNECT(...)
             *         115   +IRQ_CONNECT(...)
             */
            text += QString("%1 %2 %3 %4\n")
                        .arg(oldLineNumber, 5)
                        .arg(newLineNumber, 5)
                        .arg(prefix)
                        .arg(line.text);
        }


        text += "\n";
    }

    return text;
}