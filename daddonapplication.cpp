/*
 * This file is part LDA, Library for Deepin Toolkit Addons.
 * Some components from this project can include sources.
 * If they claim a license, then it is specified as the sources
 * can have been modified. They are shown at beginnig of files
 * or inclusions (when include is replaced by its content).
 *
 * Copyright (C) 2020 Nicolas B. @n1coc4cola or N1coc4colA on Github.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "daddonapplication.h"

#include <execinfo.h> // for backtrace
#include <dlfcn.h>    // for dladdr
#include <cxxabi.h>   // for __cxa_demangle

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>

#include <unistd.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>

#include <QProcess>
#include <DApplication>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QDateTime>
#include <DNotifySender>
#include <QStyle>
#include <QStyleHints>

using Sender = Dtk::Core::DUtil::DNotifySender;

const int MAX_STACK_FRAMES = 128;
const QString strPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)[0] + "/dde-collapse.log";
const QString cfgPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)[0] + "/dde-cfg.ini";
QString appBinPath = "None";

std::string Backtrace(int skip = 1)
{
    void *callstack[128];
    const int nMaxFrames = sizeof(callstack) / sizeof(callstack[0]);
    char buf[1024];
    int nFrames = backtrace(callstack, nMaxFrames);
    char **symbols = backtrace_symbols(callstack, nFrames);

    std::ostringstream trace_buf;
    for (int i = skip; i < nFrames; i++) {
        Dl_info info;
        if (dladdr(callstack[i], &info) && info.dli_sname) {
            char *demangled = nullptr;
            int status = -1;
            if (info.dli_sname[0] == '_')
                demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            snprintf(buf, sizeof(buf), "%-3d %*p %s + %zd\n",
                     i, int(2 + sizeof(void*) * 2), callstack[i],
                     status == 0 ? demangled :
                     info.dli_sname == nullptr ? symbols[i] : info.dli_sname,
                     (char *)callstack[i] - (char *)info.dli_saddr);
            free(demangled);
        } else {
            snprintf(buf, sizeof(buf), "%-3d %*p %s\n",
                     i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
        }
        trace_buf << buf;
    }
    free(symbols);
    if (nFrames == nMaxFrames)
        trace_buf << "[truncated]\n";
    return trace_buf.str();
}

void handleSignals [[ noreturn ]] (int sig)
{
    QFile *file = new QFile(strPath);

        if (!QFile::exists(cfgPath)) {
            QFile file(cfgPath);
            if (!file.open(QIODevice::WriteOnly))
                exit(0);
            file.close();
        }

        QSettings settings(cfgPath, QSettings::IniFormat);
        settings.beginGroup("dde-dock");

        QDateTime lastDate = QDateTime::fromString(settings.value("lastDate").toString(), "yyyy-MM-dd hh:mm:ss:zzz");
        int collapseNum = settings.value("collapse").toInt();

        if (qAbs(lastDate.secsTo(QDateTime::currentDateTime())) < 10) {
            settings.setValue("collapse", collapseNum + 1);
        } else {
            settings.setValue("collapse", 0);
        }
        settings.setValue("lastDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
        settings.endGroup();
        settings.sync();

        if (!file->open(QIODevice::Text | QIODevice::Append)) {
            qDebug() << file->errorString();
            exit(0);
        }

        if (file->size() >= 10 * 1024 * 1024) {
            file->close();
            if (file->open(QIODevice::Text | QIODevice::Truncate)) {
                qDebug() << file->errorString();
                exit(0);
            }
        }

        try {
            DWIDGET_USE_NAMESPACE
            QString head = "\n#####" + qApp->applicationName() + "#####\n"
                    + QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss:zzz]")
                    + "[crash signal number:" + QString::number(sig) + "]\n";
            file->write(head.toUtf8());

    #ifdef Q_OS_LINUX
            void *array[MAX_STACK_FRAMES];
            size_t size = 0;
            char **strings = nullptr;
            size_t i;
            signal(sig, SIG_DFL);
            size = static_cast<size_t>(backtrace(array, MAX_STACK_FRAMES));
            strings = backtrace_symbols(array, int(size));
            for (i = 0; i < size; ++i) {
                QString line = QString::number(i) + " " + QString::fromStdString(strings[i]) + "\n";
                file->write(line.toUtf8());

                std::string symbol(strings[i]);
                QString strSymbol = QString::fromStdString(symbol);
                int pos1 = strSymbol.indexOf("[");
                int pos2 = strSymbol.lastIndexOf("]");
                QString address = strSymbol.mid(pos1 + 1,pos2 - pos1 - 1);

                QString cmd = "addr2line -C -f -e " + qApp->applicationName() + " " + address;
                QProcess *p = new QProcess;
                p->setReadChannel(QProcess::StandardOutput);
                p->start(cmd);
                p->waitForFinished();
                p->waitForReadyRead();
                const char* __data = p->readAllStandardOutput();
                file->write(__data);
                delete p;
                p = nullptr;
            }
            std::cerr << Backtrace() << std::endl;

            free(strings);
    #endif // __linux
        } catch (...) {
            //
        }
        file->close();

        QString sigVal;
        switch (sig) {
        case SIGTERM: sigVal = "SIGTERM"; break;
        case SIGILL: sigVal = "SIGILL"; break;
        case SIGSEGV: sigVal = "SIGSEGV"; break;
        case SIGINT: sigVal = "SIGINT"; break;
        case SIGABRT: sigVal = "SIGABRT"; break;
        case SIGFPE: sigVal = "SIGFPE"; break;
        }

        DWIDGET_USE_NAMESPACE
        Sender send("Application crashed");
        send.timeOut(3000);
        send.appIcon("preferences-system");
        send.appName("Crash reporter");
        send.appBody("Application: \"" + QString("##BEGIN##" + appBinPath).replace("##BEGIN##/home", "~") + "\" crashed: " + sigVal);
        send.call();
        send.~DNotifySender();
        sigVal.~QString();
        appBinPath.~QString();

        exit(sig);
}

LDA_BEGIN_NAMESPACE

DAddonApplication::DAddonApplication(int &argc, char **argv) : DApplication(argc, argv) {
    signal(SIGTERM, handleSignals);
    signal(SIGSEGV, handleSignals);
    signal(SIGILL, handleSignals);
    signal(SIGINT, handleSignals);
    signal(SIGABRT, handleSignals);
    signal(SIGFPE, handleSignals);
    appBinPath = this->applicationFilePath();
}

DAddonApplication::~DAddonApplication()
{
    appBinPath.~QString();
}

void DAddonApplication::handleQuitAction() { this->DApplication::handleQuitAction(); }
void DAddonApplication::handleHelpAction() { this->DApplication::handleHelpAction(); }
void DAddonApplication::handleAboutAction() { this->DApplication::handleAboutAction(); }

void DAddonApplication::setEnabledHoverEffects(bool enable)
{
    this->styleHints()->setUseHoverEffects(enable);
}

void DAddonApplication::toggleHoverEffects()
{
    this->styleHints()->setUseHoverEffects(!this->styleHints()->useHoverEffects());
}

bool DAddonApplication::areHoverEffectsEnabled()
{
    return this->styleHints()->useHoverEffects();
}

LDA_END_NAMESPACE
