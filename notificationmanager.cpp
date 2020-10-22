#include "notificationmanager.h"

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

LDA_BEGIN_NAMESPACE

NotificationsInterface::NotificationsInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

NotificationsInterface::~NotificationsInterface()
{
}

QDBusPendingReply<> NotificationsInterface::CloseNotification(uint in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return asyncCallWithArgumentList(QStringLiteral("CloseNotification"), argumentList);
}

QDBusPendingReply<uint> NotificationsInterface::Notify(const QString &in0, uint in1, const QString &in2, const QString &in3, const QString &in4, const QStringList &in5, const QVariantMap &in6, int in7)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2) << QVariant::fromValue(in3) << QVariant::fromValue(in4) << QVariant::fromValue(in5) << QVariant::fromValue(in6) << QVariant::fromValue(in7);
    return asyncCallWithArgumentList(QStringLiteral("Notify"), argumentList);
}


NotificationObject::NotificationObject(QObject *parent) : QObject(parent) {}
NotificationObject::~NotificationObject() {}

NotificationObject * NotificationObject::setSummary(QString sum) {m_summary = sum; return this;}
NotificationObject * NotificationObject::setBody(QString body) {m_body = body; return this;}
NotificationObject * NotificationObject::setApp(QString an) {m_app = an; return this;}
NotificationObject * NotificationObject::setAppIcon(QString iconName) {m_appIcon = iconName; return this;}
NotificationObject * NotificationObject::setRawActions(QStringList rawActs) { unMergeRaw(rawActs); return this;}
NotificationObject * NotificationObject::setActions(QStringList actions) {m_actions = actions; return this;};
NotificationObject * NotificationObject::setActionIDs(QStringList actIDs) {m_actionIDs = actIDs; return this;}

NotificationObject * NotificationObject::addAction(QString identifier, QString text) {m_actionIDs << identifier; m_actions << text; return this;}
NotificationObject * NotificationObject::addAction(QString text) {m_actions << text; m_actionIDs << ""; return this;};

NotificationObject *NotificationObject::setTimeout(int t) {m_timeout = t; return this;}

NotificationManager *NotificationObject::manager() const {return m_manager;}
uint NotificationObject::id() {return m_id;}
int NotificationObject::timeout() {return m_timeout;}
QString NotificationObject::summary() {return m_summary;}
QString NotificationObject::body() {return m_body;}
QString NotificationObject::appName() {return m_app;}
QString NotificationObject::appIcon() {return m_appIcon;}
QStringList NotificationObject::rawActions() {return mergeAAI();}
QStringList NotificationObject::actions() {return m_actions;}
QStringList NotificationObject::actionIDs() {return m_actionIDs;}

QStringList NotificationObject::mergeAAI()
{
    QStringList output;
    int i = 0;
    int max = m_actions.length();

    while (i<max) {
        output << m_actionIDs.at(i);
        output << m_actions.at(i);
        i++;
    }
    return output;
}

void NotificationObject::unMergeRaw(QStringList raw)
{
    m_actions.clear();
    m_actionIDs.clear();
    int i = 0;
    int max = raw.length() /2;
    while (i<max) {
        if (i %2 == 0) {
            m_actions << raw.at(i);
        } else {
            m_actionIDs << raw.at(i);
        }
        i++;
    }
}

NotificationManager::NotificationManager(QObject *parent) : QObject(parent) {
    internList = new QList<NotificationObject *>();
    interface = new NotificationsInterface("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus(), this);
    connect(interface, &NotificationsInterface::ActionInvoked, this, &NotificationManager::handleActionTriggered);
    connect(interface, &NotificationsInterface::NotificationClosed, this, &NotificationManager::handleClosed);
}

NotificationObject *NotificationManager::notify(QString appName, QString appIcon, QString summary, QString body, QStringList actions, QStringList actionIDs, int timeout) const
{
    NotificationObject *o = new NotificationObject;
    o->setActions(actions)->setActionIDs(actionIDs);
    o->m_id = interface->Notify(appName, 0, appIcon, summary, body, o->mergeAAI(), QVariantMap(), timeout);
    internList->append(o);
    return o->setApp(appName)->setAppIcon(appIcon)->setSummary(summary)->setBody(body)->setTimeout(timeout);
}

NotificationObject *NotificationManager::notify(QString appName, QString appIcon, QString summary, QString body, QStringList rawActions, int timeout) const
{
    NotificationObject *o = new NotificationObject;
    o->m_id = interface->Notify(appName, 0, appIcon, summary, body, rawActions, QVariantMap(), timeout);
    internList->append(o);
    return o->setApp(appName)->setAppIcon(appIcon)->setSummary(summary)->setBody(body)->setRawActions(rawActions)->setTimeout(timeout);
}

NotificationObject *NotificationManager::notify(NotificationObject *o) const
{
    o->m_id = interface->Notify(o->appName(), 0, o->appIcon(), o->summary(), o->body(), o->mergeAAI(), QVariantMap(), o->timeout());
    internList->append(o);
    return o;
}

void NotificationManager::handleActionTriggered(uint a, QString b)
{
    int i = 0;
    while (i<internList->length()) {
        if (internList->at(i)->id() == a) {
            internList->at(i)->actionTriggered(b);
            Q_EMIT dataTriggered(b);
        }
        i++;
    }
}

void NotificationManager::handleClosed(uint a, uint b) // 1 is hidden (timed out or user clicked on it), 2 is closed, and higher are actions!
{
    int i = 0;
    while (i<internList->length()) {
        if (internList->at(i)->id() == a) {
            internList->at(i)->closed();
            internList->removeAt(i);
        }
        i++;
    }
}

void NotificationManager::handleCloseRequest(uint id)
{
    int i = 0;
    bool accept = false;
    while (i<internList->length()) {
        if (internList->at(i)->id() == id) {
            accept = true;
        }
        i++;
    }
    if (accept == true) {
        interface->CloseNotification(id);
    }
}

LDA_END_NAMESPACE
