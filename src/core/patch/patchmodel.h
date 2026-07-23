#pragma once

#include <QString>
#include <vector>


enum class DiffLineType
{
    Context,
    Added,
    Removed
};


struct DiffLine
{
    DiffLineType type;

    int oldLine = -1;
    int newLine = -1;

    QString text;
};


struct DiffHunk
{
    int oldStart = 0;
    int oldCount = 0;

    int newStart = 0;
    int newCount = 0;

    QString functionName;

    std::vector<DiffLine> lines;
};


struct FileDiff
{
    QString oldPath;
    QString newPath;

    std::vector<DiffHunk> hunks;
};


struct PatchFile
{
    QString subject;
    QString author;
    QString date;

    std::vector<FileDiff> files;
};
