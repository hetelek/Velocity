#ifndef STFSLIB_GLOBAL_H
#define STFSLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STFSLIB_LIBRARY)
#  define STFSLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define STFSLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // STFSLIB_GLOBAL_H
