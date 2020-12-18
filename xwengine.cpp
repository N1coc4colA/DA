#include "xwengine.h"

#include "basicelements.h"

#include <iostream>
#include <QDomElement>
#include <QMetaProperty>
#include <QLocale>

LDA_BEGIN_NAMESPACE

XWEngine::XWEngine(QObject *parent) : QObject(parent) {}
AbstractElement *XWEngine::rootElement() { return m_root; }

void XWEngine::loadXML(QString data)
{
    QString errMsg;
    int errLine;
    int errCol;

    m_root = nullptr;
    isFirstValidInstanciation = false;

    QDomDocument doc;
    if (!doc.setContent(data, false, &errMsg, &errLine, &errCol)) {
        std::cout << "Error in XML File format:" << std::endl;
        std::cout << " -    line: " << errLine << ", at: " << errCol << ", ERROR:" << std::endl;
        std::cout << " --     " << errMsg.toLocal8Bit().data() << std::endl;
    } else {
        std::cout << "XML File Format OK." << std::endl;

        QDomElement root = doc.documentElement();
        if (root.tagName() != "content")
        {
            std::cout << "Error in root container!" << std::endl;
        } else {
            generateTree(nullptr, root);
        }
    }
}


AbstractElement *XWEngine::generateInstance(QString elementType, QString)
{
    if (elementType == "Widget") {
        return new Widget;
    } else if (elementType == "HBoxLayout") {
        return new HBoxLayout;
    } else if (elementType == "VBoxLayout") {
        return new VBoxLayout;
    } else if (elementType == "LineEdit") {
        return new LineEdit;
    } else if (elementType == "Label") {
        return new Label;
    } else {
        std::cout << "XWE Flaw: unknown type of element: " << elementType.toLocal8Bit().data()
                  << ", it may lead to bugs." << std::endl;
        return nullptr;
    }
}


void XWEngine::generateTree(AbstractElement *p, QDomNode root)
{
    int i = 0;
    bool hasID = false;
    bool mustAddToParent =  false;
    QString ID = "";
    QString parentID = "";
    QString passArgs = "";
    QMap<QString, QVariant> propertiesMatched;

    QDomNamedNodeMap map = root.attributes();
    while (i < map.length()) {
        QString val = map.item(i).toAttr().value();
        QString attr = map.item(i).toAttr().name();

        if (attr == "id") {
            if (elements.contains(val)) {
                std::cout << "XWE Flaw: ID duplication." << std::endl;
            }
            ID = val;
            hasID = true;
        } else if (attr == "vparent") {
            parentID = val;
            mustAddToParent = true;
        } else if (attr == "pass") {
            passArgs = val;
        } else {
            //QObject set props
            propertiesMatched[attr] = val;
        }

        i++;
    }

    AbstractElement *inst = ((root.nodeName() != "content") ? generateInstance(root.nodeName(), passArgs) : nullptr);

    //Loads translator and all that goes with if needed
    if (root.nodeName() == "content") {
        QLocale loc = QLocale::system();
        bool needsTs = false;

        if (propertiesMatched.contains("translation")) {
            QString trp = propertiesMatched["translation"].toString();
            if (trp.contains("§LANG")) {
                trp.replace("§LANG", loc.languageToString(loc.language()));
            }
            if (trp.contains("§REGION")) {
                trp.replace("§REGION", loc.countryToString(loc.country()));
            }
            tsEngine.loadXMLMain(propertiesMatched["translation"].toString());
            needsTs = true;
        }
        if (propertiesMatched.contains("trFallBack")) {
            tsEngine.loadXMLFallback(propertiesMatched["trFallBack"].toString());
            needsTs = true;
        }

        needsTranslations = needsTs;
    }

    //Adds as root
    if ( isFirstValidInstanciation == false && inst != nullptr) {
        isFirstValidInstanciation = true;
        m_root = inst;
    }

    //Properties application
    if (inst != nullptr && inst->self() != nullptr) {
        i = 0;
        //Check and set the properties
        while (i < propertiesMatched.keys().length()) {
            QString currProp = propertiesMatched.keys().at(i);
            if (needsTranslations && propertiesMatched[currProp].toString().startsWith("§")) {
                inst->self()->setProperty(currProp.toLatin1(), tsEngine.translate(propertiesMatched[currProp].toString()));
            } else {
                inst->self()->setProperty(currProp.toLatin1(), propertiesMatched[currProp]);
            }
            i++;
        }
    } else if (root.nodeName() != "content") {
        std::cout << "XWE Flaw: no valid self instance (QObject *) provided (nullptr), undetermined behaviour. Skiped properties setup." << std::endl;
    }

    //Identification
    if ((hasID == false || ID == "") && root.nodeName() != "content") {
        std::cout << "XWE Flaw: no ID provided, possible inaccessibility or inconsistency." << std::endl;
    } else {
        elements[ID] = inst;
        if (inst != nullptr && inst->self() != nullptr) {
            inst->self()->setObjectName(ID);
        }
    }

    //Parent appending
    if (p != nullptr && mustAddToParent == true && parentID != "") {
        elements[parentID]->addElement(inst);
    } else {
        if (mustAddToParent == true) {
            std::cout << "XWE Flaw: trying to add element to unexistent component: " << root.nodeName().toLocal8Bit().data()
                      << ", " << ID.toLocal8Bit().data() << std::endl;
        }
    }

    i = 0;
    QDomNodeList list = root.childNodes();
    while (i < list.length()) {
        generateTree(inst, list.at(i));
        i++;
    }
}

LDA_END_NAMESPACE
