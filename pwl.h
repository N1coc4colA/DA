#ifndef PWL_H
#define PWL_H

#include "libda_global.h"
#include <QObject>

LDA_BEGIN_NAMESPACE

class LIBDA_SHARED_EXPORT PWL : public QObject
{
    Q_OBJECT
public:
    explicit PWL(QObject *parent = nullptr);
    ~PWL();
    QList<int> WinIDs();

Q_SIGNALS:
    void windowAdded(int id);
    void windowRemoved(int id);

public Q_SLOTS:
    void setPID(int pid = 0);

private:
    int m_pid = 0;
    QList<int> m_ids;
};

LDA_END_NAMESPACE

#endif // PWL_H
