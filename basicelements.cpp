#include "basicelements.h"

#include <iostream>
#include <QMetaObject>
#include <QMetaProperty>

LDA_BEGIN_NAMESPACE

QObject *VBoxLayout::self() {return this;}
QObject *HBoxLayout::self() {return this;}
QObject *LineEdit::self() {return this;}
QObject *Widget::self() {return this;}
QObject *Label::self() {return this;}

Widget::Widget() : QWidget(), AbstractElement () {}
VBoxLayout::VBoxLayout() : QVBoxLayout(), AbstractElement() {}
HBoxLayout::HBoxLayout() : QHBoxLayout(), AbstractElement() {}
LineEdit::LineEdit() : QLineEdit(), AbstractElement() {}
Label::Label() : QLabel(), AbstractElement() {}

void Widget::addElement(AbstractElement *element)
{
    if (element != nullptr) {
        if (QLayout *lay = qobject_cast<QLayout *>(element->self())) {
            this->setLayout(lay);
        } else {
            element->self()->setParent(this);
        }
    }
}

void VBoxLayout::addElement(AbstractElement *element)
{
    if (element != nullptr) {
        if (QWidget *w = qobject_cast<QWidget *>(element->self())) {
            this->addWidget(w);
        } else if (QLayout *lay = qobject_cast<QLayout *>(element->self())) {
            this->addLayout(lay);
        } else if (QLayoutItem *i = dynamic_cast<QLayoutItem *>(element->self())) {
            this->addItem(i);
        } else {
            std::cout << "Invalid object: " << __func__ << std::endl;
        }
    }
}

void HBoxLayout::addElement(AbstractElement *element)
{
    if (element != nullptr) {
        if (QWidget *w = qobject_cast<QWidget *>(element->self())) {
            this->addWidget(w);
        } else if (QLayout *lay = qobject_cast<QLayout *>(element->self())) {
            this->addLayout(lay);
        } else if (QLayoutItem *i = dynamic_cast<QLayoutItem *>(element->self())) {
            this->addItem(i);
        } else {
            std::cout << "Invalid object: " << __func__ << std::endl;
        }
    }
}

void LineEdit::addElement(AbstractElement *) {}
void Label::addElement(AbstractElement *) {}

LDA_END_NAMESPACE
