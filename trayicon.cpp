#include "trayicon.h"

#include <DApplication>

DWIDGET_USE_NAMESPACE

LDA_BEGIN_NAMESPACE

TrayIcon::TrayIcon(QObject *parent) : QObject(parent)
{
    m_trayIcon = new QSystemTrayIcon;
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

        m_trayIcon->show();
        m_trayIcon->setContextMenu(new QMenu());
        m_trayIcon->setIcon(((DApplication *)qApp)->productIcon());
        m_trayIcon->setToolTip(qApp->applicationDisplayName());
    }
}

TrayIcon::~TrayIcon()
{
    m_trayIcon->~QSystemTrayIcon();
}

void TrayIcon::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        Q_EMIT clicked(); break;
    default: break;
    }
}

void TrayIcon::setTrayMenu(QMenu *m)
{
    if (enabled) {
        m_trayIcon->setContextMenu(m);
    } else {
        m_menu = m;
    }
}

void TrayIcon::setTrayIcon(QIcon icon)
{
    m_trayIcon->setIcon(icon);
}

void TrayIcon::setTrayTitle(QString str)
{
    m_trayIcon->setToolTip(str);
}

void TrayIcon::enableContextMenu(bool enabl)
{
    enabled = enabl;
    if (enabl == true) {
        if(!m_trayIcon->contextMenu()) {
            m_trayIcon->setContextMenu(m_menu);
        }
    } else {
        if (m_trayIcon->contextMenu()) {
            m_menu = m_trayIcon->contextMenu();
        }
        m_trayIcon->setContextMenu(nullptr);
    }
}

void TrayIcon::setTrayVisible(bool visible)
{
    m_trayIcon->setVisible(visible);
}

QString TrayIcon::trayTitle()
{
    return m_trayIcon->toolTip();
}

QMenu *TrayIcon::trayMenu()
{
    return m_trayIcon->contextMenu();
}

QIcon TrayIcon::trayIcon()
{
    return m_trayIcon->icon();
}

bool TrayIcon::isTrayVisible()
{
    return m_trayIcon->isVisible();
}

LDA_END_NAMESPACE
