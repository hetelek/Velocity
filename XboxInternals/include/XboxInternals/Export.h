#ifndef XBOXINTERNALS_EXPORT_H
#define XBOXINTERNALS_EXPORT_H

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define XBOXINTERNALS_WINDOWS
#endif

// DLL import/export macros
#if defined(XBOXINTERNALS_WINDOWS)
    #if defined(BUILD_XBOXINTERNALS_SHARED)
        // Building shared library - export symbols
        #define XBOXINTERNALS_EXPORT __declspec(dllexport)
    #elif defined(USING_XBOXINTERNALS_SHARED)
        // Using shared library - import symbols
        #define XBOXINTERNALS_EXPORT __declspec(dllimport)
    #else
        // Building or using static library - no decoration
        #define XBOXINTERNALS_EXPORT
    #endif
#else
    // Non-Windows platforms - use GCC visibility if available
    #if defined(__GNUC__) && __GNUC__ >= 4
        #define XBOXINTERNALS_EXPORT __attribute__((visibility("default")))
    #else
        #define XBOXINTERNALS_EXPORT
    #endif
#endif

#endif // XBOXINTERNALS_EXPORT_H
