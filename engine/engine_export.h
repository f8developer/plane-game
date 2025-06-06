#pragma once

#ifdef _WIN32
    #ifdef ENGINE_EXPORTS
        #define ENGINE_API __declspec(dllexport)
    #else
        #define ENGINE_API __declspec(dllimport)
    #endif
#else
    #define ENGINE_API __attribute__((visibility("default")))
#endif

// For C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif

// Add your exported functions here with ENGINE_API prefix
// Example:
// ENGINE_API void InitializeEngine();

#ifdef __cplusplus
}
#endif 