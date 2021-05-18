#ifndef LIBDA_GLOBAL_H
#pragma message("Enjoy using LDA :)")
#endif

#ifndef LIBDA_GLOBAL_H
#define LIBDA_GLOBAL_H

#include <QtCore/qglobal.h>
#include "class_decl.h"
#include "macros.h"

//The versioning: <major>.<sub>.<minor>.<revision>.<status>
//The different status that are possible:
//"a" -> Alpha
//"b" -> Beta
//"p" -> Pre-release
//"r" -> Release
//Usually p and r are exposed on Github, a and b stays local.

#define LDA_MAJOR 1
#define LDA_SUB 0
#define LDA_MINOR 5
#define LDA_REVISION 1
#define LDA_STATUS #r

#if defined(LIBDA_GLOBAL_H)
    #define LIBDA_SHARED_EXPORT Q_DECL_EXPORT
    #define LDA_BEGIN_NAMESPACE namespace Dtk { namespace Addons {
    #define LDA_END_NAMESPACE }}
    #define LDA_USE_NAMESPACE using namespace Dtk::Addons;
#else
    #define LIBDADDONS_SHARED_EXPORT Q_DECL_IMPORT
#endif

static inline int get_lda_major() {return LDA_MAJOR;}
static inline int get_lda_sub() {return LDA_SUB;}
static inline int get_lda_minor() {return LDA_MINOR;}
static inline int get_lda_revision() {return LDA_REVISION;}
static inline const char* get_lda_status() {return LDA_STATUS;}

#endif // LIBDA_GLOBAL_H
