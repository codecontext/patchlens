#include <QStringList>
#include "patchparser.h"

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

    QStringList lines = text.split('\n');

    for (const QString &line : lines)
    {
        parseMetadata(line, patch);

        if (isDiffStart(line))
        {
            patch.files.emplace_back();

            currentFile = &patch.files.back();

            continue;
        }

        if (!currentFile)
            continue;

        if (line.startsWith("--- "))
        {
            currentFile->oldPath = line.mid(4);
        }
        else if (line.startsWith("+++ "))
        {
            currentFile->newPath = line.mid(4);
        }
    }

    return patch;
}
