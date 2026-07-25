#include "patchparser.h"

#include <QRegularExpression>
#include <QStringList>

void PatchParser::parseMetadata(
    const QString &line,
    PatchFile &patch)
{
    if (line.startsWith("Subject: "))
    {
        patch.subject = line.mid(9);
    }
    else if (line.startsWith("From: "))
    {
        patch.author = line.mid(6);
    }
    else if (line.startsWith("Date: "))
    {
        patch.date = line.mid(6);
    }
}

bool PatchParser::isDiffStart(const QString &line) const
{
    return line.startsWith("diff --git ");
}

PatchFile PatchParser::parse(const QString &text)
{
    PatchFile patch;

    FileDiff *currentFile = nullptr;
    DiffHunk *currentHunk = nullptr;

    int oldLine = 0;
    int newLine = 0;

    static const QRegularExpression hunkRegex(
        "^@@ -(\\d+)(?:,(\\d+))? \\+(\\d+)(?:,(\\d+))? @@(.*)$");

    const QStringList lines = text.split('\n');

    for (const QString &line : lines)
    {
        parseMetadata(line, patch);

        if (isDiffStart(line))
        {
            patch.files.emplace_back();

            currentFile = &patch.files.back();
            currentHunk = nullptr;

            continue;
        }

        if (!currentFile)
        {
            continue;
        }

        if (line.startsWith("--- "))
        {
            currentFile->oldPath = line.mid(4);
            continue;
        }

        if (line.startsWith("+++ "))
        {
            currentFile->newPath = line.mid(4);
            continue;
        }

        QRegularExpressionMatch match =
            hunkRegex.match(line);

        if (match.hasMatch())
        {
            currentFile->hunks.emplace_back();

            currentHunk = &currentFile->hunks.back();

            currentHunk->oldStart =
                match.captured(1).toInt();

            currentHunk->oldCount =
                match.captured(2).isEmpty() ?
                1 :
                match.captured(2).toInt();

            currentHunk->newStart =
                match.captured(3).toInt();

            currentHunk->newCount =
                match.captured(4).isEmpty() ?
                1 :
                match.captured(4).toInt();

            currentHunk->functionName =
                match.captured(5).trimmed();

            oldLine = currentHunk->oldStart;
            newLine = currentHunk->newStart;

            continue;
        }

        if (!currentHunk)
        {
            continue;
        }

        if (line.startsWith("\\ No newline"))
        {
            continue;
        }

        DiffLine diffLine;

        if (line.startsWith("+"))
        {
            diffLine.type = DiffLineType::Added;
            diffLine.oldLine = -1;
            diffLine.newLine = newLine++;
            diffLine.text = line.mid(1);
        }
        else if (line.startsWith("-"))
        {
            diffLine.type = DiffLineType::Removed;
            diffLine.oldLine = oldLine++;
            diffLine.newLine = -1;
            diffLine.text = line.mid(1);
        }
        else
        {
            diffLine.type = DiffLineType::Context;
            diffLine.oldLine = oldLine++;
            diffLine.newLine = newLine++;

            if (line.startsWith(' '))
            {
                diffLine.text = line.mid(1);
            }
            else
            {
                diffLine.text = line;
            }
        }

        currentHunk->lines.emplace_back(std::move(diffLine));
    }

    // Debug output for verification
    qDebug() << "Files:" << patch.files.size();

    for (const auto &file : patch.files)
    {
        qDebug()
            << file.newPath
            << "Hunks:" << file.hunks.size();
    }

    for (const auto &file : patch.files)
    {
        qDebug() << file.newPath;

        for (const auto &hunk : file.hunks)
        {
            qDebug()
                << "Hunk"
                << hunk.oldStart
                << hunk.oldCount
                << hunk.newStart
                << hunk.newCount
                << hunk.lines.size()
                << "lines";
        }
    }

    return patch;
}
