#ifndef WINTOOLS_H
#define WINTOOLS_H

#include "libda_global.h"
#include <QList>

LDA_BEGIN_NAMESPACE
/**
 * @brief Check if a PID is the athor of the winID
 * @param winID
 * @param PID
 * @return
 */
bool isFromPID(int winID, int PID);
/**
 * @brief Get the windows made by a PID
 * @param PID
 * @return
 */
QList<int> pidToWins(int PID);

LDA_END_NAMESPACE

#endif // WINTOOLS_H
