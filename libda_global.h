#ifndef LIBDA_GLOBAL_H
#define LIBDA_GLOBAL_H

#include <QtCore/qglobal.h>
#include "class_decl.h"

#pragma message("Your are using C++ library based on Qt and Dtk components maintained by @n1coc4cola")

#if defined(LIBDA_GLOBAL_H)
    #define LIBDA_SHARED_EXPORT Q_DECL_EXPORT
    #define LDA_BEGIN_NAMESPACE namespace Dtk { namespace Addons {
    #define LDA_END_NAMESPACE }}
    #define LDA_USE_NAMESPACE using namespace Dtk::Addons;
#else
#  define LIBDADDONS_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBDA_GLOBAL_H
