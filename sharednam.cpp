#include "sharednam.h"

#include <QEventLoop>
#include <QtNetwork/QNetworkReply>
#include <QTimer>

LDA_BEGIN_NAMESPACE

SharedNAM::SharedNAM(QObject *p) : QNetworkAccessManager(p) {}

SharedNAM *SharedNAM::instance() {
    static SharedNAM *snam = new SharedNAM;
    return snam;
}

QNetworkReply *SharedNAM::pendingGet(const QNetworkRequest &req, int msTimeout)
{
    QEventLoop loop;
    QNetworkReply *rep = this->get(req);
    connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QTimer::singleShot(msTimeout, rep, &QNetworkReply::abort);
    loop.exec();
    return rep;
}

LDA_END_NAMESPACE
