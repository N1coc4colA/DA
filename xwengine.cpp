#include "xwengine.h"

#include "basicelements.h"
#include "daddonapplication.h"

#include <iostream>
#include <QDomElement>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QLocale>
#include <QApplication>

/**
 * @brief Generate a QString for indentation, with the right level.
 * @param indent
 * @return
 */
const char* generateIndent(int indent)
{
    QString out = "";
    int i = 0;
    while (i<indent) {
        out.append("--");
        i++;
    }
    out.append(" ");
    return out.toLocal8Bit().data();
}

/**
 * @brief Used to cast QObject::connect without warning or boolean casting error.
 * @param src
 * @param tar
 * @param sig
 * @param trig
 * @return
 */
bool connector(QObject *src, QObject *tar, const char* sig, const char* trig)
{
    return QObject::connect(src, sig, tar, trig);
}

LDA_BEGIN_NAMESPACE

XWEngine::XWEngine(QObject *parent) : QObject(parent)
{
    if (DAddonApplication *a = qobject_cast<DAddonApplication *>(QApplication::instance())) {
        enableDebug = a->isConsoleDebug();
    }
}

AbstractElement *XWEngine::rootElement() { return m_root; }

QString XWEngine::getSourceContent()
{
    return doc.toString();
}

int XWEngine::objectCount()
{
    return objC;
}

bool XWEngine::loadXML(QString data)
{
    if (enableAppEvents) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Proccessing app events. 0"
                      << std::endl;
        }
        QApplication::instance()->processEvents();
    }

    QString errMsg;
    int errLine;
    int errCol;
    m_root = nullptr;
    isFirstValidInstanciation = false;
    needsTranslations = false;
    currentIndent = 0;
    objC = 0;

    if (!doc.setContent(data, false, &errMsg, &errLine, &errCol)) {
        std::cout << "XWE Flaw > Error in XML File format:" << std::endl;
        std::cout << " -    line: " << errLine << ", at: " << errCol << ", ERROR:" << std::endl;
        std::cout << " --     " << errMsg.toLocal8Bit().data() << std::endl;
        return false;
    } else {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XML File Format OK." << std::endl;
        }

        QDomElement root = doc.documentElement();
        if (root.tagName() != "content" && enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Flaw > Error in root container!"
                      << std::endl;
        } else {
            generateTree(nullptr, root);
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > Ended XML laoding and processing."
                          << std::endl;
            }
        }
    }
    return true;
}

AbstractElement *XWEngine::generateInstance(QString elementType, QString)
{
    if (enableAppEvents) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Proccessing app events. 1"
                      << std::endl;
        }
        QApplication::instance()->processEvents();
    }
    if (enableDebug) {
        std::cout << generateIndent(currentIndent)
                  << "XWE Debug > Generating instance with internal types."
                  << std::endl;
    }
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
    } else if (elementType == "script") {
    } else if (enableDebug) {
        std::cout << generateIndent(currentIndent)
                  << "XWE Flaw: unknown type of element: " << elementType.toLocal8Bit().data()
                  << ", it may lead to bugs."
                  << std::endl;
    }
    return nullptr;
}

bool XWEngine::generateConnection(QMap<QString, QVariant> &propertiesMatched)
{
    if (enableAppEvents) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Proccessing app events. 2"
                      << std::endl;
        }
        QApplication::instance()->processEvents();
    }
    if (propertiesMatched.contains("source")) {
        if (propertiesMatched.contains("target")) {
            if (propertiesMatched.contains("signal")) {
                if (propertiesMatched.contains("triggers")) {
                    if (elements[propertiesMatched["source"].toString()]->self()) {
                        if (elements[propertiesMatched["target"].toString()]->self()) {

                            if (enableDebug) {
                                std::cout << generateIndent(currentIndent)
                                          << "XWE Debug > Found the needs to establish the connection."
                                          << std::endl;
                                std::cout << generateIndent(currentIndent)
                                          << "XWE Debug > Accessing the QMetaObject of the target and source."
                                          << std::endl;
                            }

                            const QMetaObject *metaobj = elements[propertiesMatched["target"].toString()]->self()->metaObject();
                            bool targetFuncAuth = false;
                            for(int i = metaobj->methodOffset();i < metaobj->methodCount();i++){
                                QMetaMethod metamethod = metaobj->method(i);
                                if (metamethod.methodSignature() == propertiesMatched["triggers"]) {
                                    targetFuncAuth = true;
                                    if (enableDebug) {
                                        std::cout << generateIndent(currentIndent)
                                                  << "XWE Debug > Found right trigger function signature."
                                                  << std::endl;
                                    }
                                }
                            }

                            metaobj = elements[propertiesMatched["source"].toString()]->self()->metaObject();
                            bool sourceFuncAuth = false;
                            for(int i = metaobj->methodOffset();i < metaobj->methodCount();i++){
                                QMetaMethod metamethod = metaobj->method(i);
                                if (metamethod.methodSignature() == propertiesMatched["signal"]) {
                                    sourceFuncAuth = true;
                                    if (enableDebug) {
                                        std::cout << generateIndent(currentIndent)
                                                  << "XWE Debug > Found the proper signal function signature."
                                                  << std::endl;
                                    }
                                }
                            }

                            if (sourceFuncAuth) {
                                if (targetFuncAuth) {
                                    if (enableDebug) {
                                        std::cout << generateIndent(currentIndent)
                                                  << "XWE Debug > Running connection method."
                                                  << std::endl;
                                    }

                                    return connect(elements[propertiesMatched["source"].toString()]->self(),
                                            propertiesMatched["signal"].toString().toLocal8Bit().data(),
                                            elements[propertiesMatched["target"].toString()]->self(),
                                            propertiesMatched["triggers"].toString().toLocal8Bit().data());
                                } else if (enableDebug) {
                                    std::cout << generateIndent(currentIndent)
                                              << "XWE Flaw > Connections: unable to find connect's trigger method \""
                                              << propertiesMatched["triggers"].toString().toLocal8Bit().data() << "\"."
                                              << std::endl;
                                }
                            } else if (enableDebug) {
                                std::cout << generateIndent(currentIndent)
                                          << "XWE Flaw > Connections: unable to find connect's signal method \""
                                          << propertiesMatched["signal"].toString().toLocal8Bit().data() << "\"."
                                          << std::endl;
                            }
                        } else if (enableDebug) {
                            std::cout << generateIndent(currentIndent)
                                      << "XWE Flaw > Connections: impossible to reach connect's target \""
                                      << propertiesMatched["target"].toString().toLocal8Bit().data() << "\"."
                                      << std::endl;
                        }
                    } else if (enableDebug) {
                        std::cout << generateIndent(currentIndent)
                                  << "XWE Flaw > Connections: impossible to reach connect's source \""
                                  << propertiesMatched["source"].toString().toLocal8Bit().data() << "\"."
                                  << std::endl;
                    }
                } else if (enableDebug) {
                    std::cout << generateIndent(currentIndent)
                              << "XWE Flaw > Connections: no triggers (target function for the target object) provided."
                              << std::endl;
                }
            } else if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Flaw > Connections: no signal provided."
                          << std::endl;
            }
        } else if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Flaw > Connections: no target provided."
                      << std::endl;
        }
    } else if (enableDebug) {
        std::cout << generateIndent(currentIndent)
                  << "XWE Flaw > Connections: no source provided."
                  << std::endl;
    }
    return false;
}

AbstractElement *XWEngine::instanceSetup(AbstractElement *p, QDomNode root)
{
    if (enableAppEvents) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Proccessing app events. 3"
                      << std::endl;
        }
        QApplication::instance()->processEvents();
    }

    if (enableDebug) {
        std::cout << generateIndent(currentIndent)
                  << "XWE Debug > Starting to generate new object \""
                  << root.nodeName().toLocal8Bit().data() << "\"."
                  << std::endl;
    }

    int i = 0;
    bool hasID = false;
    bool mustAddToParent =  false;
    bool parentNotFound = true;
    bool wasFirstInst = isFirstValidInstanciation;

    //To free
    QString ID;
    QString parentID;
    QString passArgs;
    QMap<QString, QVariant> propertiesMatched;

    QDomNamedNodeMap map = root.attributes();

    //Pre-process data
    while (i < map.length()) {
        QString val = map.item(i).toAttr().value();
        QString attr = map.item(i).toAttr().name();

        if (attr == "id") {
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > Storing the object's ID."
                          << std::endl;
            }
            if (elements.contains(val)) {
                if (enableDebug) {
                    std::cout << generateIndent(currentIndent)
                              << "XWE Flaw > Identifiers: duplication of identifier \""
                              << val.toLocal8Bit().data() << "\""
                              << std::endl;
                }
            }
            ID = val;
            hasID = true;
        } else if (attr == "vparent") {
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > Object request parent by it's ID."
                          << std::endl;
            }
            parentID = val;
            mustAddToParent = true;
            if (enableAutoParenting && !mustAddToParent) {
                parentNotFound = false;
            }
        } else if (attr == "pass") {
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > Object reqest arguments to be passed: \""
                          << val.toLocal8Bit().data() << "\"."
                          << std::endl;
            }
            passArgs = val;
        } else {
            //QObject set props
            propertiesMatched[attr] = val;
        }
        i++;
    }

    if (enableAppEvents) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Proccessing app events. 4"
                      << std::endl;
        }
        QApplication::instance()->processEvents();
    }

    //Get instance, and change the object if is singleton

    AbstractElement *inst = ((root.nodeName() != "content" && root.nodeName() != "connect") ? generateInstance(root.nodeName(), passArgs) : nullptr);

    if (inst) {
        objC++;
    } else if (enableDebug) {
        std::cout << generateIndent(currentIndent)
                  << "XWE Debug > Invalid Element instance."
                  << std::endl;
    }

    if (inst && inst->isSingletonClass()) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Using singleton for type \""
                      << root.nodeName().toLocal8Bit().data() << "\""
                      << std::endl;
        }
        AbstractElement *target = inst->instance();
        inst->~AbstractElement();
        inst = target;
    }

    if (hasID && inst && inst->self()) {
        inst->self()->setObjectName(ID);
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Has set the object name to \""
                      << ID.toLocal8Bit().data() << "\"."
                      << std::endl;
        }
    }

    //Loads translator and all that goes with if needed
    if (root.nodeName() == "content") {
        QLocale loc = QLocale::system();
        bool needsTs = false;

        if (propertiesMatched.contains("translation")) {
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > Found translation property."
                          << std::endl;
            }

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
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > Found translation fallback property."
                          << std::endl;
            }
            tsEngine.loadXMLFallback(propertiesMatched["trFallBack"].toString());
            needsTs = true;
        }
        if (needsTs && enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > No translations found."
                      << std::endl;
        }

        needsTranslations = needsTs;
    } else if (root.nodeName() == "connect") { //Create connections from QMetaObject
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Creating a connection."
                      << std::endl;
        }
        generateConnection(propertiesMatched);
    }

    //Adds as root
    if (isFirstValidInstanciation == false && inst) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Found the first valid object instance of type \"" << root.nodeName().toLocal8Bit().data()
                      << "\" and ID \"" << ID.toLocal8Bit().data()
                      << "\"."
                      << std::endl;
        }
        isFirstValidInstanciation = true;
        m_root = inst;
    }

    if (enableAppEvents) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Proccessing app events. 5"
                      << std::endl;
        }
        QApplication::instance()->processEvents();
    }

    //Properties application
    if (inst && inst->self()) {
        i = 0;
        //Check and set the properties
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Setting up properties."
                      << std::endl;
        }
        while (i < propertiesMatched.keys().length()) {
            QString currProp = propertiesMatched.keys().at(i);

            //If the property is a lookup, we get the value now.
            if (propertiesMatched[currProp].toString().startsWith("@")) {
                QString val = propertiesMatched[currProp].toString().remove(0, 1);

                if (enableDebug) {
                    std::cout << generateIndent(currentIndent)
                              << "XWE Debug > Processing property value "
                              << val.toLocal8Bit().data() << "."
                              << std::endl;
                }
                if (val.startsWith("query(") && val.endsWith(")")) {
                    if (enableDebug) {
                        std::cout << generateIndent(currentIndent)
                                  << "XWE Debug > Setting property by using query."
                                  << std::endl;
                    }
                    val = val.remove(0, 6).remove(val.length() - 1, 1);
                    QStringList data = val.split(", ");

                } else if (val.startsWith("dynprop(") && val.endsWith(")")) {
                    if (enableDebug) {
                        std::cout << generateIndent(currentIndent)
                                  << "XWE Debug > Setting the dynamic property."
                                  << std::endl;
                    }

                    //If the property is dynamic, we connect signals now.
                    val = val.remove(0, 8).remove(val.length() - 1, 1);
                    QStringList data = val.split(", ");
                    inst->self()->setProperty(currProp.toLocal8Bit().data(), elements[data[0]]->self()->property(data[1].toLocal8Bit().data()));

                    //Check if a notifier of values changes exists first.
                    if (elements[data[0]]->self()) {
                        const QMetaObject *senderMeta = elements[data[0]]->self()->metaObject();
                        int index = senderMeta->indexOfProperty(data[1].toLocal8Bit().data());

                        bool hasNotifier = false;
                        QString notifier = "";
                        QString writter = "";

                        if (index != -1) {
                            if (enableDebug) {
                                std::cout << generateIndent(currentIndent)
                                          << "XWE Debug > Looking for a property notifier."
                                          << std::endl;
                            }

                            const QMetaProperty property = senderMeta->property(index);
                            if (property.hasNotifySignal()) {
                                const QMetaMethod notifySignal = property.notifySignal();
                                hasNotifier = true;
                                notifier = notifySignal.methodSignature();
                                if (enableDebug) {
                                    std::cout << generateIndent(currentIndent)
                                              << "XWE Debug > Found property notifier \""
                                              << notifier.simplified().toLocal8Bit().data() << "\"."
                                              << std::endl;
                                }
                            }

                            if (hasNotifier) {
                                //Then connect 'em
                                //We separate it of the generateConnect as it's a "hidden" way,
                                //we connect but that's not so obvious, so in that case, we generate special warnings.

                                //We suppose every one's using the Qt's advisories to declare props and how to write 'em.
                                writter = QString(property.name() + QString("Changed(") + property.typeName() + ")");

                                if (enableDebug) {
                                    std::cout << generateIndent(currentIndent)
                                              << "XWE Debug > Trying to use property writter."
                                              << std::endl;
                                }

                                if ((connector(elements[data[0]]->self(), inst->self(), notifier.toLocal8Bit().data(), writter.toLocal8Bit().data()) == false)
                                    && enableDebug) {
                                    std::cout << generateIndent(currentIndent)
                                              << "XWE Flaw > AutoConnections (From functional \"dynprop("
                                              << data[0].toLocal8Bit().data() << ", "
                                              << data[1].toLocal8Bit().data() << ")\"): unable to connect with slot \""
                                              << writter.toLocal8Bit().data() << "\", Type: \""
                                              << root.nodeName().toLocal8Bit().data() << "\", ID: \""
                                              << ID.toLocal8Bit().data() << "\". Try to make connection manually to fix it."
                                              << std::endl;
                                }
                            } else if (enableDebug) {
                                std::cout << generateIndent(currentIndent)
                                          << "XWE Flaw > Properties: unable to find a valid signal to update the property. ID: \""
                                          << data[0].toLocal8Bit().data() << "\", Property: \""
                                          << data[1].toLocal8Bit().data() << "\", using actual value instead."
                                          << std::endl;
                            }
                            if (!inst->self()->setProperty(root.nodeName().toLocal8Bit().data(), elements[data[0]]->self()->property(data[1].toLocal8Bit().data())) && enableDebug) {
                                std::cout << generateIndent(currentIndent)
                                          << "XWE Debug > Failed to set the property "
                                          << property.name() << "."
                                          << std::endl;
                            }
                        } else if (enableDebug) {
                            std::cout << generateIndent(currentIndent)
                                      << "XWEngine Flaw > Properties: unable to find Property: \""
                                      << data[1].toLocal8Bit().data() << "\", of Object of ID: \""
                                      << data[0].toLocal8Bit().data() << "."
                                      << std::endl;
                        }
                    } else if (enableDebug) {
                        std::cout << generateIndent(currentIndent)
                                  << "XWE Flaw > Identifiers: unable to find object of ID: \""
                                  << data[0].toLocal8Bit().data() << "\"."
                                  << std::endl;
                    }
                } else if (enableDebug) {
                    std::cout << generateIndent(currentIndent)
                              << "XWE Flaw > Functionals: unable to find or run functional: \""
                              << propertiesMatched[currProp].toString().toLocal8Bit().data() << "\"."
                              << std::endl;
                }
            } else {
                //Means it may need Tr.
                if (needsTranslations && propertiesMatched[currProp].toString().startsWith("§")) {
                    if (enableDebug) {
                        std::cout << generateIndent(currentIndent)
                                  << "XWE Debug > Translating property"
                                  << std::endl;
                    }
                    inst->self()->setProperty(currProp.toLatin1(), tsEngine.translate(propertiesMatched[currProp].toString()));
                } else {
                    if (enableDebug) {
                        std::cout << generateIndent(currentIndent)
                                  << "XWE Debug > Setting property "
                                  << currProp.toLocal8Bit().data() << "."
                                  << std::endl;
                    }
                    QVariant var = propertiesMatched[currProp];
                    if (var.toString().startsWith("\\§") && !var.toString().startsWith("\\@")) {//If we want to use "@" or "§" but not in the way we've defined them.
                        var = var.toString().remove(0, 1);
                    }
                    if (!inst->self()->setProperty(currProp.toLatin1(), var) && enableDebug) {
                        std::cout << generateIndent(currentIndent)
                                  << "XWE Debug > Failed to set property "
                                  << currProp.toLocal8Bit().data() << "."
                                  << std::endl;
                    }
                }
            }
            i++;
        }
    } else if (root.nodeName() != "content" && root.nodeName() != "connect" && enableDebug) {
        std::cout << generateIndent(currentIndent)
                  << "XWE Flaw > Properties: no valid self instance provided, undetermined behaviour, properties setup skipped. Type: \""
                  << root.nodeName().toLocal8Bit().data() << "\", ID: \""
                  << ID.toLocal8Bit().data() << "\"."
                  << std::endl;
    }


    //Parent appending
    if (mustAddToParent && inst && !inst->isSingletonClass() && !parentID.isEmpty()) {
        AbstractElement *top = elements[parentID];
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Must add to parent " << parentID.toLocal8Bit().data()
                      << "."
                      << std::endl;
        }
        if (top) {
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > The parent instance is valid."
                          << std::endl;
            }
            top->addElement(inst);
            if (enableObjectParenting && inst->self() && top->self()) {
                inst->self()->setParent(top->self());
                if (enableDebug) {
                    std::cout << generateIndent(currentIndent)
                              << "XWE Debug > The QObject parent have been set."
                              << std::endl;
                }
            }
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > The Element parent have been set."
                          << std::endl;
            }
        } else if (enableAutoParenting) {
            if (enableDebug) {

                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > The parent have not been found, using autoparenting."
                          << std::endl;
            }
            if (p) {
                p->addElement(inst);
                if (enableObjectParenting && inst->self() && top->self()) {
                    inst->self()->setParent(p->self());
                    if (enableDebug) {
                        std::cout << generateIndent(currentIndent)
                                  << "XWE Debug > The QObject parent have been set."
                                  << std::endl;
                    }
                }
                if (enableDebug) {
                    std::cout << generateIndent(currentIndent)
                              << "XWE Debug > The Element parent have been set." << std::endl;
                }
            }
        } else if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > No valid parent found."
                      << std::endl;
        }
    } else if (inst && !inst->isSingletonClass() && enableAutoParenting && parentNotFound) {
        if (p) {
            p->addElement(inst);
            if (enableObjectParenting && p->self() && inst) {
                inst->self()->setParent(p->self());
                if (enableDebug) {
                    std::cout << generateIndent(currentIndent)
                              << "XWE Debug > The QObject parent have been set."
                              << std::endl;
                }
            }
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > The Element parent have been set."
                          << std::endl;
            }
        }
    } else if (enableDebug && root.nodeName() != "content" && (isFirstValidInstanciation == wasFirstInst)) {
        if (inst && inst->isSingletonClass()) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > A Singleton class (\""
                      << root.nodeName().toLocal8Bit().data()
                      << "\") cannot be parented."
                      << std::endl;
        } else {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > No valid parent name have been found: \"" << parentID.toLocal8Bit().data()
                      << "\".\n"
                      << std::endl;
            std::cout << generateIndent(currentIndent)
                      << "  Generated trace:\n"
                      << generateIndent(currentIndent)
                      << "  |\n"
                      << generateIndent(currentIndent)
                      << "  +-+- Autoparent ID:         " << ((p) ? ((p->self()) ? p->self()->objectName().toLocal8Bit().data() : "<unknown>") : "<unknown>") << "\n"
                      << generateIndent(currentIndent)
                      << "    |- Autoparent C++ class:  " << ((p) ? typeid(p).name() : "<unknown>") << "\n"
                      << generateIndent(currentIndent)
                      << "    |- Autoparent validity:   " << ((p) ? "OK" : "No object supplied") << "\n"
                      << generateIndent(currentIndent)
                      << "    |- Parent specified:      " << ((mustAddToParent) ? "true" : "false") << "\n"
                      << generateIndent(currentIndent)
                      << "    |- parent ID:             " << parentID.toLocal8Bit().data() << "\n"
                      << generateIndent(currentIndent)
                      << "    |- parent ID validity:    " << ((!parentID.isEmpty()) ? ((elements.keys().contains(parentID)) ? "OK" : "Not in list") : "Unauthorized name") << "\n"
                      << generateIndent(currentIndent)
                      << "    |- target ID:             " << ID.toLocal8Bit().data() << "\n"
                      << generateIndent(currentIndent)
                      << "    |- target class:          " << root.nodeName().toLocal8Bit().data() << "\n"
                      << generateIndent(currentIndent)
                      << "    |- object validity:       " << ((inst) ? "OK" : "No object supplied") << "\n"
                      << generateIndent(currentIndent)
                      << "    |- target singleton:      " << ((inst) ? ((inst->isSingletonClass()) ? "true" : "false") : "<unknown>") << "\n"
                      << generateIndent(currentIndent)
                      << "    |- child level:           " << currentIndent << "\n"
                      << generateIndent(currentIndent)
                      << "    |- Object creation count: " << objC << "\n"
                      << generateIndent(currentIndent)
                      << "    |- First step check:      " << ((mustAddToParent && inst && !inst->isSingletonClass() && !parentID.isEmpty()) ? "true" : "false") << "\n"
                      << generateIndent(currentIndent)
                      << "    +- Second check:          " << ((inst && !inst->isSingletonClass() && enableAutoParenting && parentNotFound) ? "true" : "false") << "\n"
                      << std::endl;
        }
    }

    //Identification
    if ((hasID == false || ID.isEmpty()) && root.nodeName() != "content" && root.nodeName() != "connect" && enableDebug) {
        std::cout << generateIndent(currentIndent)
                  << "XWE Flaw > Identifiers: no ID provided, possible inaccessibility or inconsistency, type: \""
                  << root.nodeName().toLocal8Bit().data()
                  << "\"."
                  << std::endl;
    } else {
        if (inst) {
            elements[ID] = inst;
            if (inst->self()) {
                inst->self()->setObjectName(ID);
                if (enableDebug) {
                    std::cout << generateIndent(currentIndent)
                              << "XWE Debug > Has set object name to \"" << parentID.toLocal8Bit().data()
                              << "\"."
                              << std::endl;
                }
            }
        }
    }

    if (enableAppEvents) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Proccessing app events. 6"
                      << std::endl;
        }
        QApplication::instance()->processEvents();
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Processing ended."
                      << std::endl;
        }
    }
}

void XWEngine::generateTree(AbstractElement *p, QDomNode root)
{
    currentIndent++;

    AbstractElement *inst = instanceSetup(p, root);

    if (enableDebug) {
        std::cout << generateIndent(currentIndent)
                  << "XWE Debug > Generating children objects"
                  << std::endl;
    }
    int i = 0;
    QDomNodeList list = root.childNodes();
    if (!list.isEmpty()) {
        int len = list.length();
        while (i < len) {
            if (enableDebug) {
                std::cout << generateIndent(currentIndent)
                          << "XWE Debug > Creating child "
                          << i << "."
                          << std::endl;
            }
            generateTree(inst, list.at(i));
            i++;
        }
    }
    if (enableDebug) {
        std::cout << generateIndent(currentIndent)
                  << "XWE Debug > Ended children creation."
                  << std::endl;
    }

    if (enableAppEvents) {
        if (enableDebug) {
            std::cout << generateIndent(currentIndent)
                      << "XWE Debug > Proccessing app events. 7"
                      << std::endl;
        }
        QApplication::instance()->processEvents();
    }

    currentIndent--;
}

LDA_END_NAMESPACE
