#include "diffviewer.h"

#include <QComboBox>
#include <QFrame>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QVBoxLayout>


namespace
{

QFont fixedFont()
{
    return QFontDatabase::systemFont(
        QFontDatabase::FixedFont);
}


QTextCharFormat contextFormat()
{
    QTextCharFormat format;

    format.setForeground(
        QColor("#24292f"));

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


QTextCharFormat addedFormat()
{
    QTextCharFormat format;

    format.setForeground(
        QColor("#1a7f37"));

    format.setBackground(
        QColor("#dafbe1"));

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


QTextCharFormat lineNumberFormat()
{
    QTextCharFormat format;

    format.setForeground(
        QColor("#57606a"));

    return format;
}

} // namespace


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
     * View selector.
     */
    auto *toolbar =
        new QHBoxLayout();

    toolbar->setContentsMargins(
        8, 6, 8, 6);


    auto *viewLabel =
        new QLabel("View:", this);


    viewModeCombo =
        new QComboBox(this);

    viewModeCombo->addItem(
        "Unified");

    viewModeCombo->addItem(
        "Side-by-Side");

    viewModeCombo->setCurrentIndex(1);


    connect(
        viewModeCombo,
        &QComboBox::currentIndexChanged,
        this,
        &DiffViewer::viewModeChanged);


    toolbar->addWidget(
        viewLabel);

    toolbar->addWidget(
        viewModeCombo);

    toolbar->addStretch();


    mainLayout->addLayout(
        toolbar);


    /*
     * Unified view.
     */
    unifiedView =
        new QPlainTextEdit(this);

    unifiedView->setReadOnly(true);

    unifiedView->setLineWrapMode(
        QPlainTextEdit::NoWrap);

    unifiedView->setFont(
        fixedFont());


    /*
     * Side-by-side views.
     */
    auto *sideBySideWidget =
        new QWidget(this);

    auto *sideLayout =
        new QHBoxLayout(
            sideBySideWidget);

    sideLayout->setContentsMargins(
        0, 0, 0, 0);

    sideLayout->setSpacing(0);


    oldView =
        new QPlainTextEdit(
            sideBySideWidget);

    newView =
        new QPlainTextEdit(
            sideBySideWidget);


    oldView->setReadOnly(true);
    newView->setReadOnly(true);


    oldView->setLineWrapMode(
        QPlainTextEdit::NoWrap);

    newView->setLineWrapMode(
        QPlainTextEdit::NoWrap);


    oldView->setFont(
        fixedFont());

    newView->setFont(
        fixedFont());


    oldView->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);

    newView->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);


    oldView->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);

    newView->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded);


    separator =
        new QFrame(sideBySideWidget);

    separator->setFrameShape(
        QFrame::VLine);

    separator->setFrameShadow(
        QFrame::Plain);


    sideLayout->addWidget(
        oldView,
        1);

    sideLayout->addWidget(
        separator);

    sideLayout->addWidget(
        newView,
        1);


    mainLayout->addWidget(
        unifiedView,
        1);

    mainLayout->addWidget(
        sideBySideWidget,
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
     * Initial state.
     */
    unifiedView->hide();

    sideBySideWidget->show();

    oldView->setPlainText(
        "Select a file to view changes.");

    newView->clear();
}


void DiffViewer::connectScrollBars()
{
    /*
     * Vertical synchronization.
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
     * Horizontal ranges.
     */
    connect(
        oldView->horizontalScrollBar(),
        &QScrollBar::rangeChanged,
        this,
        [this](int, int)
        {
            updateHorizontalScrollBar();
        });


    connect(
        newView->horizontalScrollBar(),
        &QScrollBar::rangeChanged,
        this,
        [this](int, int)
        {
            updateHorizontalScrollBar();
        });


    connect(
        unifiedView->horizontalScrollBar(),
        &QScrollBar::rangeChanged,
        this,
        [this](int, int)
        {
            updateHorizontalScrollBar();
        });


    /*
     * Shared horizontal scrolling.
     */
    connect(
        horizontalScrollBar,
        &QScrollBar::valueChanged,
        this,
        [this](int value)
        {
            if (viewMode ==
                ViewMode::Unified)
            {
                unifiedView
                    ->horizontalScrollBar()
                    ->setValue(value);

                return;
            }


            oldView
                ->horizontalScrollBar()
                ->setValue(value);

            newView
                ->horizontalScrollBar()
                ->setValue(value);
        });
}


void DiffViewer::updateHorizontalScrollBar()
{
    if (viewMode ==
        ViewMode::Unified)
    {
        auto *bar =
            unifiedView->horizontalScrollBar();

        horizontalScrollBar->setRange(
            bar->minimum(),
            bar->maximum());

        horizontalScrollBar->setPageStep(
            bar->pageStep());

        return;
    }


    int maximum =
        qMax(
            oldView
                ->horizontalScrollBar()
                ->maximum(),

            newView
                ->horizontalScrollBar()
                ->maximum());


    int pageStep =
        qMax(
            oldView
                ->horizontalScrollBar()
                ->pageStep(),

            newView
                ->horizontalScrollBar()
                ->pageStep());


    horizontalScrollBar->setRange(
        0,
        maximum);

    horizontalScrollBar->setPageStep(
        pageStep);
}


void DiffViewer::setFile(
    const FileDiff &file)
{
    currentFile = file;

    renderFile();
}


void DiffViewer::renderFile()
{
    if (viewMode ==
        ViewMode::Unified)
    {
        renderUnified(
            currentFile);
    }
    else
    {
        renderSideBySide(
            currentFile);
    }

    updateHorizontalScrollBar();
}


void DiffViewer::viewModeChanged(
    int index)
{
    if (index == 0)
    {
        viewMode =
            ViewMode::Unified;
    }
    else
    {
        viewMode =
            ViewMode::SideBySide;
    }


    /*
     * Hide/show the appropriate views.
     */
    QWidget *sideBySideWidget =
        oldView->parentWidget();


    if (viewMode ==
        ViewMode::Unified)
    {
        unifiedView->show();
        sideBySideWidget->hide();
    }
    else
    {
        unifiedView->hide();
        sideBySideWidget->show();
    }


    renderFile();

    horizontalScrollBar->setValue(0);
}


/*
 * Unified diff rendering.
 */
void DiffViewer::renderUnified(
    const FileDiff &file)
{
    unifiedView->clear();

    QTextCursor cursor(
        unifiedView->document());


    for (const DiffHunk &hunk :
         file.hunks)
    {
        QString header =
            QString("@@ -%1,%2 +%3,%4 @@")
                .arg(hunk.oldStart)
                .arg(hunk.oldCount)
                .arg(hunk.newStart)
                .arg(hunk.newCount);


        if (!hunk.functionName.isEmpty())
        {
            header += " ";
            header += hunk.functionName;
        }


        cursor.insertText(
            header + "\n",
            hunkFormat());


        for (const DiffLine &line :
             hunk.lines)
        {
            QString oldNumber;
            QString newNumber;


            if (line.oldLine >= 0)
            {
                oldNumber =
                    QString::number(
                        line.oldLine);
            }


            if (line.newLine >= 0)
            {
                newNumber =
                    QString::number(
                        line.newLine);
            }


            QString prefix;


            QTextCharFormat format;


            switch (line.type)
            {
            case DiffLineType::Context:
                prefix = " ";
                format =
                    contextFormat();
                break;

            case DiffLineType::Added:
                prefix = "+";
                format =
                    addedFormat();
                break;

            case DiffLineType::Removed:
                prefix = "-";
                format =
                    removedFormat();
                break;
            }


            QString text =
                QString("%1 %2 %3 %4\n")
                    .arg(oldNumber, 5)
                    .arg(newNumber, 5)
                    .arg(prefix)
                    .arg(line.text);


            cursor.insertText(
                text,
                format);
        }


        cursor.insertText(
            "\n");
    }


    unifiedView->moveCursor(
        QTextCursor::Start);
}


/*
 * Side-by-side rendering.
 */
void DiffViewer::renderSideBySide(
    const FileDiff &file)
{
    oldView->clear();
    newView->clear();


    QTextCursor oldCursor(
        oldView->document());

    QTextCursor newCursor(
        newView->document());


    for (const DiffHunk &hunk :
         file.hunks)
    {
        QString header =
            QString("@@ -%1,%2 +%3,%4 @@")
                .arg(hunk.oldStart)
                .arg(hunk.oldCount)
                .arg(hunk.newStart)
                .arg(hunk.newCount);


        if (!hunk.functionName.isEmpty())
        {
            header += " ";
            header += hunk.functionName;
        }


        oldCursor.insertText(
            header + "\n",
            hunkFormat());

        newCursor.insertText(
            header + "\n",
            hunkFormat());


        for (const DiffLine &line :
             hunk.lines)
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
                    contextFormat());

                newCursor.insertText(
                    newLine + "\n",
                    contextFormat());

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
                    removedFormat());

                newCursor.insertText(
                    "\n");

                break;
            }


            case DiffLineType::Added:
            {
                oldCursor.insertText(
                    "\n");


                QString newLine =
                    QString("%1 | +%2")
                        .arg(line.newLine, 5)
                        .arg(line.text);


                newCursor.insertText(
                    newLine + "\n",
                    addedFormat());

                break;
            }
            }
        }


        oldCursor.insertText(
            "\n");

        newCursor.insertText(
            "\n");
    }


    oldView->moveCursor(
        QTextCursor::Start);

    newView->moveCursor(
        QTextCursor::Start);
}