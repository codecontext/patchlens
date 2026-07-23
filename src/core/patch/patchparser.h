#pragma once

#include <QString>

#include "patchmodel.h"

class PatchParser
{
public:
    PatchFile parse(const QString &text);
};
