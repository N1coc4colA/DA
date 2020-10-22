#include "pwl.h"
#include "wintools.h"

#include <QDebug>
#include <KF5/KWindowSystem/KWindowSystem>

LDA_BEGIN_NAMESPACE

PWL::PWL(QObject *parent) : QObject(parent)
{
    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, [this](WId id) {
        if (isFromPID((int)id, m_pid)){
            m_ids << (int)id;
            Q_EMIT windowAdded((int)id);
        }
    });
    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, [this](WId id) {
        if (m_ids.contains((int)id)) {
            Q_EMIT windowRemoved((int)id);
            m_ids.removeAll((int)id);
        }
    });
}

PWL::~PWL()
{
    m_ids.~QList();
}

void PWL::setPID(int pid)
{
    m_pid = pid;
    m_ids = pidToWins(pid);
}

QList<int> PWL::WinIDs()
{
    return m_ids;
}

LDA_END_NAMESPACE
