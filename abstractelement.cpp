#include "abstractelement.h"

LDA_BEGIN_NAMESPACE

AbstractElement::AbstractElement() {}
AbstractElement::~AbstractElement() {}
AbstractElement *AbstractElement::instance() {return nullptr;}
bool AbstractElement::isSingletonClass() { return m_singletonStatus; }

LDA_END_NAMESPACE
