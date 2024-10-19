#pragma once

// No need to export/import symbols when building a static library
#if defined(XBOXINTERNALS_STATIC)
#define XBOXINTERNALSSHARED_EXPORT
#elif defined(XBOXINTERNALS_LIBRARY)
// Export symbols when building the library
#include <QtCore/qglobal.h>
#define XBOXINTERNALSSHARED_EXPORT Q_DECL_EXPORT
#else
// Import symbols when using the library
#include <QtCore/qglobal.h>
#define XBOXINTERNALSSHARED_EXPORT Q_DECL_IMPORT
#endif
