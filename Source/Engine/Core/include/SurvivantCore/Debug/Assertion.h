#pragma once
#include <cstdlib>

#include "SurvivantCore/Debug/Logger.h"
#include "SurvivantCore/Debug/DebugBreak.h"

#ifndef ASSERT
#if defined(_DEBUG) || defined(SV_VERBOSE_LOG)

#define ASSERT(condition, ...) if (!(condition))   \
{                                                  \
    SV_LOG_ERROR("Assertion failed: " #condition   \
    __VA_OPT__( "\n%s", SvCore::Utility::FormatString(__VA_ARGS__).c_str()) \
    );                                             \
    DEBUG_BREAK();                                 \
    abort();                                       \
} ((void)0)

#else

#define ASSERT(condition, ...) ((void)(condition))

#endif // _DEBUG || SV_VERBOSE_LOG
#endif // !ASSERT

#ifndef CHECK
#if defined(_DEBUG) || defined(SV_VERBOSE_LOG)

#define CHECK(condition, ...) [&]() -> bool          \
{                                                    \
    if (!(condition))                                \
    {                                                \
        SV_LOG_ERROR("Check failed: " #condition     \
        __VA_OPT__( "\n%s", SvCore::Utility::FormatString(__VA_ARGS__).c_str()) \
        );                                           \
        DEBUG_BREAK();                               \
        return false;                                \
    }                                                \
    return true;                                     \
}()

#else

#define CHECK(condition, ...) [](const bool b) -> bool \
{                                                      \
    return b;                                          \
}(condition)

#endif // _DEBUG || SV_VERBOSE_LOG
#endif // !CHECK

#ifndef ASSUME
#if defined(_DEBUG) || defined(SV_VERBOSE_LOG)

#define ASSUME(condition, ...) CHECK(condition __VA_OPT__(,) __VA_ARGS__)

#else

#define ASSUME(condition, ...) true

#endif // _DEBUG || SV_VERBOSE_LOG
#endif // !ASSUME

#ifndef ASSUME_FALSE
#if defined(_DEBUG) || defined(SV_VERBOSE_LOG)

#define ASSUME_FALSE(condition, ...) !CHECK(!(condition) __VA_OPT__(,) __VA_ARGS__)

#else

#define ASSUME_FALSE(condition, ...) false

#endif // _DEBUG || SV_VERBOSE_LOG
#endif // !ASSUME_FALSE
