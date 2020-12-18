#include "libraryban.h"

#include <iostream>
#include <QProcess>

const QString proc_name = "ldd ";

LDA_BEGIN_NAMESPACE

LibraryBan::LibraryBan(QObject *parent) : QLibrary(parent), proc (new QProcess)
{
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                if (exitStatus == QProcess::ExitStatus::NormalExit) {
                    this->parseOutput();
                } else {
                    std::cout << "Library Ban Check ERROR: [readelf]: " << exitCode << std::endl;
                    Q_EMIT this->checkFailed();
                }
    });
}

LibraryBan::~LibraryBan()
{
    proc->~QProcess();
    this->QLibrary::~QLibrary();
}

void LibraryBan::checkLib()
{
    proc->startDetached(proc_name + this->fileName());
}

void LibraryBan::parseOutput()
{
    QString data = proc->readAll();
    QStringList list = data.split("\n");
    int i = 0;

    m_ban = false;

    while (i<list.length()) {
        QString d = list.at(i).split("=>").at(0);
        d.remove(" ");
        int j = 0;
        while (j<libsToBan.length()) {
            if (d.contains(libsToBan.at(i))) {
                m_ban = true;
                j = libsToBan.length();
            }
        }
        i++;
    }
}

typedef AbstractPlugin* (*requestPlugin)();

AbstractPlugin *LibraryBan::loadPlugin()
{
    if(this->load()) {
        requestPlugin get = (requestPlugin)this->resolve("getPlugin");
        return get();
    }
    return nullptr;
}

LDA_END_NAMESPACE
