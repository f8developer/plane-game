//
// Created by ivanr on 2.6.2025 г..
//

#ifndef DEFINES_H
#define DEFINES_H

#include <string>

#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
    #define GPLATFORM_WINDOWS
#endif

#ifndef DLLEX
    #ifdef GPLATFORM_WINDOWS
        #define DLLEX __declspec(dllexport)
    #else
        #define DLLEX
    #endif
#endif

// Types (Rust-inspired)
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;

using str = std::string;

typedef struct Vector2Int {
    int x;                // Vector x component
    int y;                // Vector y component
} Vector2Int;

#endif //DEFINES_H
