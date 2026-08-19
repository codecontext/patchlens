#include "diffviewer.h"

#include <QFrame>
#include <QLabel>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QScrollBar>
//#include <QSignalBlocker>
#include <QVBoxLayout>


DiffViewer::DiffViewer(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    connectScrollBars();
}


void DiffViewer::setupUi()
{
    auto *mainLayout =
        new QVBoxLayout(this);

    mainLayout->setContentsMargins(
        0, 0, 0, 0);

    mainLayout->setSpacing(0);


    /*
     * Header.
     */
    auto *headerLayout =
        new QHBoxLayout();

    headerLayout->setContentsMargins(
        0, 0, 0, 0);

    headerLayout->setSpacing(0);


    auto *oldHeader =
        new QLabel("OLD", this);

    auto *newHeader =
        new QLabel("NEW", this);


    oldHeader->setAlignment(
        Qt::AlignCenter);

    newHeader->setAlignment(
        Qt::AlignCenter);


    separator =
        new QFrame(this);

    separator->setFrameShape(
        QFrame::VLine);

    separator->setFrameShadow(
        QFrame::Plain);


    headerLayout->addWidget(
        oldHeader,
        1);

    headerLayout->addWidget(
        separator);

    headerLayout->addWidget(
        newHeader,
        1);


    mainLayout->addLayout(
        headerLayout);


    /*
     * Diff views.
     */
    auto *contentLayout =
        new QHBoxLayout();

    contentLayout->setContentsMargins(
        0, 0, 0, 0);

    contentLayout->setSpacing(0);


    oldView =
        new QPlainTextEdit(this);

    newView =
        new QPlainTextEdit(this);


    oldView->setReadOnly(true);
    newView->setReadOnly(true);


    oldView->setLineWrapMode(
        QPlainTextEdit::NoWrap);

    newView->setLineWrapMode(
        QPlainTextEdit::NoWrap);


    QFont fixedFont =
        QFontDatabase::systemFont(
            QFontDatabase::FixedFont);

    oldView->setFont(fixedFont);
    newView->setFont(fixedFont);


    /*
     * Hide individual horizontal scrollbars.
     *
     * A single scrollbar below both panes
     * will control horizontal movement.
     */
    oldView->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);

    newView->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);


    /*
     * Vertical scrollbars.
     *
     * Only the right-hand scrollbar is shown.
     * Both views move together.
     */
    oldView->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);

    newView->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded);


    contentLayout->addWidget(
        oldView,
        1);

    contentLayout->addWidget(
        separator);

    contentLayout->addWidget(
        newView,
        1);


    mainLayout->addLayout(
        contentLayout,
        1);


    /*
     * Shared horizontal scrollbar.
     */
    horizontalScrollBar =
        new QScrollBar(
            Qt::Horizontal,
            this);


    mainLayout->addWidget(
        horizontalScrollBar);


    /*
     * Initial message.
     */
    oldView->setPlainText(
        "Select a file to view changes.");

    newView->clear();
}

void DiffViewer::connectScrollBars()
{
    /*
     * Synchronize vertical scrolling.
     *
     * Moving either pane moves the other pane
     * to the same vertical position.
     */
    connect(
        oldView->verticalScrollBar(),
        &QScrollBar::valueChanged,
        newView->verticalScrollBar(),
        &QScrollBar::setValue);

    connect(
        newView->verticalScrollBar(),
        &QScrollBar::valueChanged,
        oldView->verticalScrollBar(),
        &QScrollBar::setValue);


    /*
     * One horizontal scrollbar controls both
     * diff panes.
     *
     * Use the maximum range of the two views.
     */
    auto updateHorizontalRange =
        [this]()
        {
            QScrollBar *oldBar =
                oldView->horizontalScrollBar();

            QScrollBar *newBar =
                newView->horizontalScrollBar();

            int maximum =
                qMax(
                    oldBar->maximum(),
                    newBar->maximum());

            int pageStep =
                qMax(
                    oldBar->pageStep(),
                    newBar->pageStep());

            horizontalScrollBar->setRange(
                0,
                maximum);

            horizontalScrollBar->setPageStep(
                pageStep);
        };


    connect(
        oldView->horizontalScrollBar(),
        &QScrollBar::rangeChanged,
        this,
        [updateHorizontalRange](
            int,
            int)
        {
            updateHorizontalRange();
        });


    connect(
        newView->horizontalScrollBar(),
        &QScrollBar::rangeChanged,
        this,
        [updateHorizontalRange](
            int,
            int)
        {
            updateHorizontalRange();
        });


    /*
     * Shared horizontal scrollbar.
     */
    connect(
        horizontalScrollBar,
        &QScrollBar::valueChanged,
        this,
        [this](int value)
        {
            oldView->horizontalScrollBar()
                ->setValue(value);

            newView->horizontalScrollBar()
                ->setValue(value);
        });
}

void DiffViewer::setFile(
    const FileDiff &file)
{
    renderSideBySide(file);
}

void DiffViewer::renderSideBySide(
    const FileDiff &file)
{
    oldView->clear();
    newView->clear();

    QTextCursor oldCursor(
        oldView->document());

    QTextCursor newCursor(
        newView->document());


    /*
     * Context formatting.
     */
    QTextCharFormat contextFormat;

    contextFormat.setForeground(
        QColor("#24292f"));


    /*
     * Removed line formatting.
     */
    QTextCharFormat removedFormat;

    removedFormat.setForeground(
        QColor("#cf222e"));

    removedFormat.setBackground(
        QColor("#ffebe9"));


    /*
     * Added line formatting.
     */
    QTextCharFormat addedFormat;

    addedFormat.setForeground(
        QColor("#1a7f37"));

    addedFormat.setBackground(
        QColor("#dafbe1"));


    /*
     * Hunk header formatting.
     */
    QTextCharFormat hunkFormat;

    hunkFormat.setForeground(
        QColor("#0969da"));

    hunkFormat.setBackground(
        QColor("#ddf4ff"));

    hunkFormat.setFontWeight(
        QFont::Bold);


    /*
     * Line-number formatting.
     *
     * Slightly different from source code so
     * the line numbers are visually identifiable.
     */
    QTextCharFormat lineNumberFormat;

    lineNumberFormat.setForeground(
        QColor("#57606a"));


    for (const DiffHunk &hunk :
         file.hunks)
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
            hunkHeader +=
                hunk.functionName;
        }


        /*
         * Hunk header appears on both sides.
         */
        oldCursor.insertText(
            hunkHeader + "\n",
            hunkFormat);

        newCursor.insertText(
            hunkHeader + "\n",
            hunkFormat);


        for (const DiffLine &line :
             hunk.lines)
        {
            /*
             * Context line.
             */
            if (line.type ==
                DiffLineType::Context)
            {
                QString oldNumber =
                    QString("%1")
                        .arg(line.oldLine, 5);

                QString newNumber =
                    QString("%1")
                        .arg(line.newLine, 5);


                oldCursor.insertText(
                    oldNumber,
                    lineNumberFormat);

                oldCursor.insertText(
                    " | " + line.text + "\n",
                    contextFormat);


                newCursor.insertText(
                    newNumber,
                    lineNumberFormat);

                newCursor.insertText(
                    " | " + line.text + "\n",
                    contextFormat);

                continue;
            }


            /*
             * Removed line.
             */
            if (line.type ==
                DiffLineType::Removed)
            {
                QString oldNumber =
                    QString("%1")
                        .arg(line.oldLine, 5);


                oldCursor.insertText(
                    oldNumber,
                    removedFormat);

                oldCursor.insertText(
                    " | -" + line.text + "\n",
                    removedFormat);


                /*
                 * Empty row on NEW side.
                 */
                newCursor.insertText(
                    "\n");

                continue;
            }


            /*
             * Added line.
             */
            if (line.type ==
                DiffLineType::Added)
            {
                /*
                 * Empty row on OLD side.
                 */
                oldCursor.insertText(
                    "\n");


                QString newNumber =
                    QString("%1")
                        .arg(line.newLine, 5);


                newCursor.insertText(
                    newNumber,
                    addedFormat);

                newCursor.insertText(
                    " | +" + line.text + "\n",
                    addedFormat);
            }
        }


        /*
         * Separate hunks visually.
         */
        oldCursor.insertText("\n");
        newCursor.insertText("\n");
    }


    /*
     * Reset scroll positions.
     */
    oldView->verticalScrollBar()
        ->setValue(0);

    newView->verticalScrollBar()
        ->setValue(0);

    oldView->horizontalScrollBar()
        ->setValue(0);

    newView->horizontalScrollBar()
        ->setValue(0);

    horizontalScrollBar
        ->setValue(0);
}