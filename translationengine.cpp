#include "translationengine.h"

#include "sharednam.h"

#include <iostream>
#include <QNetworkReply>
#include <QDomElement>
#include <QDomDocument>

LDA_BEGIN_NAMESPACE

TranslationEngine::TranslationEngine(QObject *parent) : QObject(parent) {}

bool TranslationEngine::loadXMLMain(QString main)
{
    QNetworkReply *ans = SharedNAM::instance()->pendingGet(QNetworkRequest(main));
    if (ans->error()) {
        std::cout << "XWE, Translation Engine: failed to load, error: " << ans->error()
                  << ", " << ans->errorString().toLocal8Bit().data()
                  << "\n--- Url: " << ans->url().toString().toLocal8Bit().data() << std::endl;
    } else {

        QString errMsg;
        int errLine;
        int errCol;

        QDomDocument doc;
        if (!doc.setContent(ans->readAll(), false, &errMsg, &errLine, &errCol)) {
            std::cout << "XWE, Translation Engine: Error in XML Translation File format:" << std::endl;
            std::cout << " -    line: " << errLine << ", at: " << errCol << ", ERROR:" << std::endl;
            std::cout << " --     " << errMsg.toLocal8Bit().data() << std::endl;
        } else {
            std::cout << "XML File Format OK." << std::endl;

            QDomElement root = doc.documentElement();
            if (root.tagName() != "translationsDocument")
            {
                std::cout << "XWE, Translation Engine: Error in root container!" << std::endl;
            } else {
                m_main.clear();
                int i = 0;
                QDomNodeList list = root.childNodes();
                while (i < list.length()) {
                    if (list.at(i).nodeName() == "ts") {
                        int j = 0;
                        QDomNamedNodeMap map = list.at(i).attributes();
                        bool hasID = false;
                        bool hasTs = false;
                        QString ID = "";
                        QString ts = "";
                        while (j < map.length()) {
                            if (map.item(j).toAttr().name() == "val") {
                                hasTs = true;
                                ts = map.item(j).toAttr().value();
                            } else if (map.item(j).toAttr().name() == "id") {
                                hasID = true;
                                ID = map.item(j).toAttr().value();
                            }
                            j++;
                        }
                        if (hasID && hasTs) {
                            m_main[ID] = ts;
                        } else {
                            std::cout << "XWE, Translation Engine: No valid translation nor identifier given, translation skiped." << std::endl;
                        }
                        return true;
                    } else {
                        std::cout << "XWE, Translation Engine: Skipping non handled element of the XML file. This might lead to inconsistencies." << std::endl;
                    }
                    i++;
                }
            }
        }
    }
    return false;
}


bool TranslationEngine::loadXMLFallback(QString main)
{
    QNetworkReply *ans = SharedNAM::instance()->pendingGet(QNetworkRequest(main));
    if (ans->error()) {
        std::cout << "XWE, Translation Engine: failed to load, error: " << ans->error()
                  << ", " << ans->errorString().toLocal8Bit().data()
                  << "\n--- Url: " << ans->url().toString().toLocal8Bit().data() << std::endl;
    } else {

        QString errMsg;
        int errLine;
        int errCol;

        QDomDocument doc;
        if (!doc.setContent(ans->readAll(), false, &errMsg, &errLine, &errCol)) {
            std::cout << "XWE, Translation Engine: Error in XML Translation File format:" << std::endl;
            std::cout << " -    line: " << errLine << ", at: " << errCol << ", ERROR:" << std::endl;
            std::cout << " --     " << errMsg.toLocal8Bit().data() << std::endl;
        } else {
            std::cout << "XML File Format OK." << std::endl;

            QDomElement root = doc.documentElement();
            if (root.tagName() != "translationsDocument")
            {
                std::cout << "XWE, Translation Engine: Error in root container!" << std::endl;
            } else {
                m_main.clear();
                int i = 0;
                QDomNodeList list = root.childNodes();
                while (i < list.length()) {
                    if (list.at(i).nodeName() == "ts") {
                        int j = 0;
                        QDomNamedNodeMap map = list.at(i).attributes();
                        bool hasID = false;
                        bool hasTs = false;
                        QString ID = "";
                        QString ts = "";
                        while (j < map.length()) {
                            if (map.item(j).toAttr().name() == "val") {
                                hasTs = true;
                                ts = map.item(j).toAttr().value();
                            } else if (map.item(j).toAttr().name() == "id") {
                                hasID = true;
                                ID = map.item(j).toAttr().value();
                            }
                            j++;
                        }
                        if (hasID && hasTs) {
                            m_fbk[ID] = ts;
                        } else {
                            std::cout << "XWE, Translation Engine: No valid translation nor identifier given, translation skiped." << std::endl;
                        }
                        return true;
                    } else {
                        std::cout << "XWE, Translation Engine: Skipping non handled element of the XML file. This might lead to inconsistencies." << std::endl;
                    }
                    i++;
                }
            }
        }
    }
    return false;
}

QString TranslationEngine::translate(QString entry)
{
    QString source = entry.remove(0, 1); //Remove the first character tha is "§" and that isn't included in the translation system itself.
    if (m_main.contains(source)) {
        return m_main[source];
    } else if (m_fbk.contains(source)) {
        return m_fbk[source];
    } else {
        return source;
    }
}

LDA_END_NAMESPACE
