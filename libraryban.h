#ifndef LIBRARYBAN_H
#define LIBRARYBAN_H

#include <QApplication>
#include <QLibrary>

class QProcess;
class AbstractPlugin;

class AbstractPlugin {
    Q_OBJECT
public:
    explicit AbstractPlugin(QObject *p = nullptr);
    virtual ~AbstractPlugin();

    QString version();
    QString author();
    QString release();
    QString website();
    QString description();
    QString target();

protected:
    const QString m_version = "0.0.0.1";
    const QString m_author = "latin";
    const QString m_release = "Test 1";
    const QString m_website = "github.com/deepin-espagnol";
    const QString m_description = "Base plugin";
    const QString m_target = QApplication::applicationName();
};

class LibraryBan : public QLibrary
{
    Q_OBJECT
public:
    explicit LibraryBan(QObject *parent = nullptr);
    ~LibraryBan();
    inline bool mustBeBan() { return m_ban; }
    QStringList libsToBan;
    AbstractPlugin *loadPlugin();
signals:
    void checkFailed();

public slots:
    void parseOutput();
    void checkLib();

private:
    QProcess *proc;
    bool m_ban = false;
};

#endif // LIBRARYBAN_H
