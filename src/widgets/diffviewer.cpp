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

    QTextCursor oldCursor(oldView->document());
    QTextCursor newCursor(newView->document());

    QTextCharFormat contextFormat;
    contextFormat.setForeground(QColor("#24292f"));

    QTextCharFormat removedFormat;
    removedFormat.setForeground(QColor("#cf222e"));
    removedFormat.setBackground(QColor("#ffebe9"));

    QTextCharFormat addedFormat;
    addedFormat.setForeground(QColor("#1a7f37"));
    addedFormat.setBackground(QColor("#dafbe1"));

    QTextCharFormat hunkFormat;
    hunkFormat.setForeground(QColor("#0969da"));
    hunkFormat.setBackground(QColor("#ddf4ff"));
    hunkFormat.setFontWeight(QFont::Bold);


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


        /*
         * Hunk header appears in both panes.
         */
        oldCursor.insertText(
            hunkHeader + "\n",
            hunkFormat);

        newCursor.insertText(
            hunkHeader + "\n",
            hunkFormat);


        for (const DiffLine &line : hunk.lines)
        {
            switch (line.type)
            {
            case DiffLineType::Context:
            {
                QString oldLine =
                    QString("%1 | %2")
                        .arg(line.oldLine, 5)
                        .arg(line.text);

                QString newLine =
                    QString("%1 | %2")
                        .arg(line.newLine, 5)
                        .arg(line.text);

                oldCursor.insertText(
                    oldLine + "\n",
                    contextFormat);

                newCursor.insertText(
                    newLine + "\n",
                    contextFormat);

                break;
            }


            case DiffLineType::Removed:
            {
                QString oldLine =
                    QString("%1 | -%2")
                        .arg(line.oldLine, 5)
                        .arg(line.text);

                oldCursor.insertText(
                    oldLine + "\n",
                    removedFormat);

                /*
                 * Keep the NEW side aligned.
                 */
                newCursor.insertText("\n");

                break;
            }


            case DiffLineType::Added:
            {
                /*
                 * Keep the OLD side aligned.
                 */
                oldCursor.insertText("\n");

                QString newLine =
                    QString("%1 | +%2")
                        .arg(line.newLine, 5)
                        .arg(line.text);

                newCursor.insertText(
                    newLine + "\n",
                    addedFormat);

                break;
            }
            }
        }


        /*
         * Blank line between hunks.
         */
        oldCursor.insertText("\n");
        newCursor.insertText("\n");
    }


    /*
     * Put both views at the beginning.
     */
    oldView->moveCursor(
        QTextCursor::Start);

    newView->moveCursor(
        QTextCursor::Start);

    horizontalScrollBar->setValue(0);
}
