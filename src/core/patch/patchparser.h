#pragma once

#include <QString>

#include "patchmodel.h"

class PatchParser
{
public:
    PatchFile parse(const QString &text);

private:
    void parseMetadata(const QString &line, PatchFile &patch);

    bool isDiffStart(const QString &line) const;
};
