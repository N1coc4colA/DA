#include "xwengine.h"

#include "basicelements.h"

#include <iostream>
#include <QDomElement>
#include <QMetaProperty>
#include <QMetaMethod>
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

bool XWEngine::generateConnection(QMap<QString, QVariant> propertiesMatched)
{
    if (propertiesMatched.contains("source")) {
        if (propertiesMatched.contains("target")) {
            if (propertiesMatched.contains("signal")) {
                if (propertiesMatched.contains("triggers")) {
                    if (elements[propertiesMatched["source"].toString()]->self() != nullptr) {
                        if (elements[propertiesMatched["target"].toString()]->self() != nullptr) {

                            const QMetaObject *metaobj = elements[propertiesMatched["target"].toString()]->self()->metaObject();
                            bool targetFuncAuth = false;
                            for(int i = metaobj->methodOffset();i < metaobj->methodCount();i++){
                                QMetaMethod metamethod = metaobj->method(i);
                                if (metamethod.methodSignature() == propertiesMatched["triggers"]) {
                                    targetFuncAuth = true;
                                }
                            }

                            metaobj = elements[propertiesMatched["source"].toString()]->self()->metaObject();
                            bool sourceFuncAuth = false;
                            for(int i = metaobj->methodOffset();i < metaobj->methodCount();i++){
                                QMetaMethod metamethod = metaobj->method(i);
                                if (metamethod.methodSignature() == propertiesMatched["signal"]) {
                                    sourceFuncAuth = true;
                                }
                            }

                            if (sourceFuncAuth) {
                                if (targetFuncAuth) {
                                    connect(elements[propertiesMatched["source"].toString()]->self(), propertiesMatched["signal"].toString().toLocal8Bit().data(),
                                            elements[propertiesMatched["target"].toString()]->self(), propertiesMatched["triggers"].toString().toLocal8Bit().data());
                                    return true;
                                } else {
                                    std::cout << "XWE Flaw > Connections: unable to find connect's trigger method \"" <<
                                                 propertiesMatched["triggers"].toString().toLocal8Bit().data() << "\"."  << std::endl;
                                }
                            } else {
                                std::cout << "XWE Flaw > Connections: unable to find connect's signal method \"" <<
                                             propertiesMatched["triggers"].toString().toLocal8Bit().data() << "\"."  << std::endl;
                            }
                        } else {
                            std::cout << "XWE Flaw > Connections: impossible to reach connect's target \"" <<
                                         propertiesMatched["target"].toString().toLocal8Bit().data() << "\"."  << std::endl;
                        }
                    } else {
                        std::cout << "XWE Flaw > Connections: impossible to reach connect's source \"" <<
                                     propertiesMatched["target"].toString().toLocal8Bit().data() << "\"."  << std::endl;
                    }
                } else {
                    std::cout << "XWE Flaw > Connections: no triggers (target function for the target) provided." << std::endl;
                }
            } else {
                std::cout << "XWE Flaw > Connections: no signal provided." << std::endl;
            }
        } else {
            std::cout << "XWE Flaw > Connections: no source provided." << std::endl;
        }
    } else {
        std::cout << "XWE Flaw > Connections: no target provided." << std::endl;
    }
    return false;
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
                std::cout << "XWE Flaw > Identifiers: duplication of identifier \"" << val.toLocal8Bit().data() << "\"" << std::endl;
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
    } else if (root.nodeName() == "connect") { //Create connections from QMetaObject
        generateConnection(propertiesMatched);
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

            //If the property is a lookup, we get the value now.
            if (propertiesMatched[currProp].toString().startsWith("@")) {
                QString val = propertiesMatched[currProp].toString().remove(0, 1);
                if (val.startsWith("query(") && val.endsWith(")")) {
                    val = val.remove(0, 6).remove(val.length() - 1, 1);
                    QStringList data = val.split(", ");


                } else if (val.startsWith("dynprop(") && val.endsWith(")")) {
                    //If the property is dynamic, we connect signals now.
                    val = val.remove(0, 8).remove(val.length() - 1, 1);
                    QStringList data = val.split(", ");

                    //Check if a notifier of values changes exists first.
                    if (elements[data[0]]->self()) {
                        const QMetaObject *senderMeta = elements[data[0]]->self()->metaObject();
                        int index = senderMeta->indexOfProperty(data[1].toLocal8Bit().data());

                        bool hasNotifier = false;
                        QString notifier = "";
                        QString writter = "";

                        if (index != -1) {
                            const QMetaProperty property = senderMeta->property(index);
                            if (property.hasNotifySignal()) {
                                const QMetaMethod notifySignal = property.notifySignal();
                                hasNotifier = true;
                                notifier = notifySignal.methodSignature();
                            }

                            if (hasNotifier) {
                                //Then connect 'em
                                //We separate it of the generateConnect as it's a "hidden" way, we connect but that's not so obvious, so in that case, we generate special warnings.
                                //We suppose every one's using the Qt's advisories to declare props and how to write 'em.
                                writter = QString(property.name() + QString("Changed(") + property.typeName() + ")");

                                if (((bool)(connect(elements[data[0]]->self(), notifier.toLocal8Bit().data(), inst->self(), writter.toLocal8Bit().data()))) == false) {
                                    std::cout << "XWE Flaw > AutoConnections (From functional \"dynprop("
                                              << data[0].toLocal8Bit().data() << ", "
                                              << data[1].toLocal8Bit().data() << ")\"): unable to connect with slot \""
                                              << writter.toLocal8Bit().data() << "\", Type: \""
                                              << root.nodeName().toLocal8Bit().data() << "\", ID: \""
                                              << ID.toLocal8Bit().data() << "\". Try to make connection manually to fix it."
                                              << std::endl;
                                }
                            } else {
                                std::cout << "XWE Flaw > Properties: unable to find a valid signal to update the property. ID: \""
                                          << data[0].toLocal8Bit().data() << "\", Property: \""
                                          << data[1].toLocal8Bit().data() << "\", using actual value instead."
                                          << std::endl;
                                inst->self()->setProperty(root.nodeName().toLocal8Bit().data(), elements[data[0]]->self()->property(data[1].toLocal8Bit().data()));
                            }
                        } else {
                            std::cout << "XWEngine Flaw > Properties: unable to find Property: \""
                                      << data[1].toLocal8Bit().data() << "\", of Object of ID: \""
                                      << data[0].toLocal8Bit().data()
                                      << std::endl;
                        }
                    } else {
                        std::cout << "XWE Flaw > Identifiers: unable to find object of ID: \""
                                  << data[0].toLocal8Bit().data() << "\"."
                                  << std::endl;
                    }
                } else {
                    std::cout << "XWE Flaw > Functionals: unable to find or run functional: \""
                              << propertiesMatched[currProp].toString().toLocal8Bit().data() << "\"."
                              << std::endl;
                }
            } else {
                //Means it may need Tr.
                if (needsTranslations && propertiesMatched[currProp].toString().startsWith("§")) {
                    inst->self()->setProperty(currProp.toLatin1(), tsEngine.translate(propertiesMatched[currProp].toString()));
                } else {
                    QVariant var = propertiesMatched[currProp];
                    if (var.toString().startsWith("\\§") && !var.toString().startsWith("\\@")) {//If we want to use "@" or "§" but not in the way we've defined them.
                        var = var.toString().remove(0, 1);
                    }
                    inst->self()->setProperty(currProp.toLatin1(), var);
                }
            }
            i++;
        }
    } else if (root.nodeName() != "content" && root.nodeName() != "connect") {
        std::cout << "XWE Flaw > Properties: no valid self instance provided, undetermined behaviour, properties setup skipped. Type: \""
                  << root.nodeName().toLocal8Bit().data() << "\", ID: \""
                  << ID.toLocal8Bit().data() << "\"."
                  << std::endl;
    }

    //Identification
    if ((hasID == false || ID == "") && root.nodeName() != "content" && root.nodeName() != "connect") {
        std::cout << "XWE Flaw > Identifiers: no ID provided, possible inaccessibility or inconsistency." << std::endl;
    } else {
        elements[ID] = inst;
        if (inst != nullptr && inst->self() != nullptr) {
            inst->self()->setObjectName(ID);
        }
    }

    //Parent appending
    if (p != nullptr && mustAddToParent == true && parentID != "") {
        elements[parentID]->addElement(inst);
    } else if (mustAddToParent == true) {
            std::cout << "XWE Flaw > Containering and Parenting: trying to add element to unexistent component of Type: \""
                      << root.nodeName().toLocal8Bit().data() << "\", ID: \""
                      << ID.toLocal8Bit().data() << "\"."
                      << std::endl;
    }

    i = 0;
    QDomNodeList list = root.childNodes();
    while (i < list.length()) {
        generateTree(inst, list.at(i));
        i++;
    }
}

LDA_END_NAMESPACE
