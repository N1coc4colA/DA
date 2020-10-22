#ifndef WINTOOLS_H
#define WINTOOLS_H

#include "libda_global.h"
#include <QList>

LDA_BEGIN_NAMESPACE

bool isFromPID(int winID, int PID);
QList<int> pidToWins(int PID);

LDA_END_NAMESPACE

#endif // WINTOOLS_H
