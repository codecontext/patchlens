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
    QString text;
};


struct DiffHunk
{
    QString header;
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

    std::vector<FileDiff> files;
};
