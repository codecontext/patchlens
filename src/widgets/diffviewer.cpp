#include "diffviewer.h"

#include <QFontDatabase>
#include <QTextCharFormat>
#include <QTextCursor>

#include <vector>


DiffViewer::DiffViewer(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);

    setFont(
        QFontDatabase::systemFont(
            QFontDatabase::FixedFont));

    setLineWrapMode(
        QPlainTextEdit::NoWrap);

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
     * Column widths.
     *
     * Old line number + separator + code
     * New line number + separator + code
     */
    constexpr int lineNumberWidth = 5;
    constexpr int separatorWidth = 3;
    constexpr int codeWidth = 70;


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


        QTextCharFormat hunkFormat;

        hunkFormat.setForeground(
            QColor("#0969da"));

        hunkFormat.setBackground(
            QColor("#ddf4ff"));

        hunkFormat.setFontWeight(
            QFont::Bold);


        cursor.insertText(
            hunkHeader + "\n",
            hunkFormat);


        /*
         * Process the hunk in pairs.
         *
         * Context:
         *
         *     OLD                  NEW
         *     context             context
         *
         * Removed:
         *
         *     OLD                  NEW
         *     removed              empty
         *
         * Added:
         *
         *     OLD                  NEW
         *     empty                added
         *
         * Consecutive removed/added lines are paired.
         */
        size_t index = 0;

        while (index < hunk.lines.size())
        {
            const DiffLine &line =
                hunk.lines[index];


            /*
             * Context line.
             */
            if (line.type ==
                DiffLineType::Context)
            {
                QString oldText =
                    QString("%1 | %2")
                        .arg(line.oldLine,
                             lineNumberWidth)
                        .arg(line.text);

                QString newText =
                    QString("%1 | %2")
                        .arg(line.newLine,
                             lineNumberWidth)
                        .arg(line.text);


                QString row =
                    QString("%1 | %2\n")
                        .arg(oldText.left(codeWidth +
                                          lineNumberWidth +
                                          separatorWidth),
                             -codeWidth)
                        .arg(newText);


                QTextCharFormat format;

                cursor.insertText(
                    row,
                    format);

                ++index;

                continue;
            }


            /*
             * Collect consecutive removed lines.
             */
            std::vector<const DiffLine *> removedLines;

            while (index < hunk.lines.size() &&
                   hunk.lines[index].type ==
                       DiffLineType::Removed)
            {
                removedLines.push_back(
                    &hunk.lines[index]);

                ++index;
            }


            /*
             * Collect consecutive added lines.
             */
            std::vector<const DiffLine *> addedLines;

            while (index < hunk.lines.size() &&
                   hunk.lines[index].type ==
                       DiffLineType::Added)
            {
                addedLines.push_back(
                    &hunk.lines[index]);

                ++index;
            }


            /*
             * Pair removed and added lines.
             *
             * If the number of lines differs,
             * remaining lines appear on only
             * one side.
             */
            size_t pairCount =
                std::max(
                    removedLines.size(),
                    addedLines.size());


            for (size_t pair = 0;
                 pair < pairCount;
                 ++pair)
            {
                const DiffLine *oldLine =
                    pair < removedLines.size()
                        ? removedLines[pair]
                        : nullptr;

                const DiffLine *newLine =
                    pair < addedLines.size()
                        ? addedLines[pair]
                        : nullptr;


                QString oldNumber;
                QString oldText;

                QString newNumber;
                QString newText;


                if (oldLine)
                {
                    oldNumber =
                        QString::number(
                            oldLine->oldLine);

                    oldText =
                        oldLine->text;
                }


                if (newLine)
                {
                    newNumber =
                        QString::number(
                            newLine->newLine);

                    newText =
                        newLine->text;
                }


                QString oldColumn =
                    QString("%1 | %2")
                        .arg(oldNumber,
                             lineNumberWidth)
                        .arg(oldText);


                QString newColumn =
                    QString("%1 | %2")
                        .arg(newNumber,
                             lineNumberWidth)
                        .arg(newText);


                /*
                 * Pad the old column so that
                 * the new column always begins
                 * at the same position.
                 */
                oldColumn =
                    oldColumn.leftJustified(
                        codeWidth +
                        lineNumberWidth +
                        separatorWidth);


                QString row =
                    oldColumn +
                    " | " +
                    newColumn +
                    "\n";


                QTextCharFormat oldFormat;
                QTextCharFormat newFormat;


                if (oldLine)
                {
                    oldFormat.setForeground(
                        QColor("#cf222e"));

                    oldFormat.setBackground(
                        QColor("#ffebe9"));
                }


                if (newLine)
                {
                    newFormat.setForeground(
                        QColor("#1a7f37"));

                    newFormat.setBackground(
                        QColor("#dafbe1"));
                }


                /*
                 * Insert the complete row first.
                 *
                 * Formatting individual columns
                 * will be handled in the next
                 * rendering refinement.
                 */
                if (oldLine)
                {
                    cursor.insertText(
                        oldColumn,
                        oldFormat);
                }
                else
                {
                    cursor.insertText(
                        oldColumn);
                }


                cursor.insertText(
                    " | ");


                if (newLine)
                {
                    cursor.insertText(
                        newColumn,
                        newFormat);
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
}