#include "diffviewer.h"

#include <QFontDatabase>
#include <QTextCharFormat>
#include <QTextCursor>

#include <algorithm>
#include <vector>


namespace
{

constexpr int LineNumberWidth = 5;
constexpr int CodeWidth = 70;
constexpr int ColumnWidth =
    LineNumberWidth + 3 + CodeWidth;


QString formatColumn(
    const QString &lineNumber,
    const QString &text)
{
    QString column =
        QString("%1 | %2")
            .arg(lineNumber, LineNumberWidth)
            .arg(text);

    return column.leftJustified(ColumnWidth);
}


QTextCharFormat contextFormat()
{
    QTextCharFormat format;

    format.setForeground(
        QColor("#24292f"));

    return format;
}


QTextCharFormat addedFormat()
{
    QTextCharFormat format;

    format.setForeground(
        QColor("#1a7f37"));

    format.setBackground(
        QColor("#dafbe1"));

    return format;
}


QTextCharFormat removedFormat()
{
    QTextCharFormat format;

    format.setForeground(
        QColor("#cf222e"));

    format.setBackground(
        QColor("#ffebe9"));

    return format;
}


QTextCharFormat hunkFormat()
{
    QTextCharFormat format;

    format.setForeground(
        QColor("#0969da"));

    format.setBackground(
        QColor("#ddf4ff"));

    format.setFontWeight(
        QFont::Bold);

    return format;
}


QTextCharFormat headerFormat()
{
    QTextCharFormat format;

    format.setFontWeight(
        QFont::Bold);

    format.setForeground(
        QColor("#57606a"));

    return format;
}

} // namespace


DiffViewer::DiffViewer(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);

    setLineWrapMode(
        QPlainTextEdit::NoWrap);

    setFont(
        QFontDatabase::systemFont(
            QFontDatabase::FixedFont));

    setPlainText(
        "Select a file to view changes.");
}


void DiffViewer::setFile(const FileDiff &file)
{
    clear();

    renderSideBySide(file);
}


void DiffViewer::renderSideBySide(
    const FileDiff &file)
{
    QTextCursor cursor(document());


    /*
     * Column headers.
     */
    QString oldHeader =
        QString("OLD").leftJustified(
            ColumnWidth);

    QString newHeader =
        "NEW";


    cursor.insertText(
        oldHeader,
        headerFormat());

    cursor.insertText(
        " | ");

    cursor.insertText(
        newHeader + "\n",
        headerFormat());


    /*
     * Render each hunk.
     */
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

        cursor.insertText(
            hunkHeader + "\n",
            hunkFormat());


        size_t index = 0;

        while (index < hunk.lines.size())
        {
            const DiffLine &line =
                hunk.lines[index];


            /*
             * Context line:
             *
             * OLD                              NEW
             * 114 | foo();                     114 | foo();
             */
            if (line.type ==
                DiffLineType::Context)
            {
                QString oldColumn =
                    formatColumn(
                        QString::number(
                            line.oldLine),
                        line.text);

                QString newColumn =
                    formatColumn(
                        QString::number(
                            line.newLine),
                        line.text);


                cursor.insertText(
                    oldColumn,
                    contextFormat());

                cursor.insertText(
                    " | ");

                cursor.insertText(
                    newColumn + "\n",
                    contextFormat());

                ++index;

                continue;
            }


            /*
             * Collect removed lines.
             */
            std::vector<const DiffLine *> removed;

            while (index < hunk.lines.size() &&
                   hunk.lines[index].type ==
                       DiffLineType::Removed)
            {
                removed.push_back(
                    &hunk.lines[index]);

                ++index;
            }


            /*
             * Collect added lines.
             */
            std::vector<const DiffLine *> added;

            while (index < hunk.lines.size() &&
                   hunk.lines[index].type ==
                       DiffLineType::Added)
            {
                added.push_back(
                    &hunk.lines[index]);

                ++index;
            }


            /*
             * Pair removed and added lines.
             */
            const size_t rowCount =
                std::max(
                    removed.size(),
                    added.size());


            for (size_t row = 0;
                 row < rowCount;
                 ++row)
            {
                const DiffLine *oldLine =
                    row < removed.size()
                        ? removed[row]
                        : nullptr;

                const DiffLine *newLine =
                    row < added.size()
                        ? added[row]
                        : nullptr;


                QString oldColumn;
                QString newColumn;


                if (oldLine)
                {
                    oldColumn =
                        formatColumn(
                            QString::number(
                                oldLine->oldLine),
                            oldLine->text);
                }
                else
                {
                    oldColumn =
                        QString(
                            ColumnWidth,
                            ' ');
                }


                if (newLine)
                {
                    newColumn =
                        formatColumn(
                            QString::number(
                                newLine->newLine),
                            newLine->text);
                }
                else
                {
                    newColumn =
                        QString(
                            ColumnWidth,
                            ' ');
                }


                /*
                 * Removed side.
                 */
                if (oldLine)
                {
                    cursor.insertText(
                        oldColumn,
                        removedFormat());
                }
                else
                {
                    cursor.insertText(
                        oldColumn);
                }


                /*
                 * Central separator.
                 */
                cursor.insertText(
                    " | ");


                /*
                 * Added side.
                 */
                if (newLine)
                {
                    cursor.insertText(
                        newColumn,
                        addedFormat());
                }
                else
                {
                    cursor.insertText(
                        newColumn);
                }


                cursor.insertText(
                    "\n");
            }
        }


        cursor.insertText("\n");
    }


    setTextCursor(cursor);

    /*
     * Start at the beginning of the document.
     */
    moveCursor(
        QTextCursor::Start);

    ensureCursorVisible();
}