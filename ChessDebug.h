#ifndef CHESSDEBUG_H
#define CHESSDEBUG_H

#include <cassert>

#if defined(QT_VERSION)
#define CHESS_HAS_QT 1
#else
#define CHESS_HAS_QT 0
#endif

#if defined(NDEBUG)
#define CHESS_DEBUG_BUILD 0
#else
#define CHESS_DEBUG_BUILD 1
#endif

#if CHESS_HAS_QT

#include <QDebug>

namespace ChessDebug
{
template <typename... Args>
inline void debugLog(Args&&... args)
{
    QDebug dbg = qDebug().noquote();
    dbg << "[ChessCore]";
    (dbg << ... << args);
}

template <typename... Args>
inline void infoLog(Args&&... args)
{
    QDebug dbg = qInfo().noquote();
    dbg << "[ChessCore]";
    (dbg << ... << args);
}
}

#else

#include <iostream>

namespace ChessDebug
{
template <typename... Args>
inline void debugLog(Args&&... args)
{
    std::cerr << "[ChessCore] ";
    (std::cerr << ... << args);
    std::cerr << '\n';
}

template <typename... Args>
inline void infoLog(Args&&... args)
{
    std::cout << "[ChessCore] ";
    (std::cout << ... << args);
    std::cout << '\n';
}
}

#endif

#define CHESS_DEBUG_LOG(...) \
do { ChessDebug::debugLog(__VA_ARGS__); } while (false)

#define CHESS_INFO_LOG(...) \
    do { ChessDebug::infoLog(__VA_ARGS__); } while (false)

#if CHESS_DEBUG_BUILD

#define CHESS_DEBUG_ONLY_LOG(...) \
    CHESS_DEBUG_LOG(__VA_ARGS__)

#define CHESS_ASSERT(condition) \
do { assert(condition); } while (false)

#define CHESS_ASSERT_MSG(condition, message)                         \
    do                                                               \
{                                                                \
        if (!(condition))                                            \
    {                                                            \
            CHESS_DEBUG_LOG(                                         \
                                                                     "ASSERT FAILED: ",                                  \
                                                                     message,                                             \
                                                                     " | Condition: ",                                   \
#condition,                                          \
                                                                     " | File: ",                                        \
                                                                     __FILE__,                                            \
                                                                     " | Line: ",                                        \
                                                                     __LINE__                                             \
                );                                                       \
            assert(condition);                                       \
    }                                                            \
} while (false)

#else

#define CHESS_DEBUG_ONLY_LOG(...) \
do { } while (false)

#define CHESS_ASSERT(condition) \
    do { (void)sizeof(condition); } while (false)

#define CHESS_ASSERT_MSG(condition, message) \
    do { (void)sizeof(condition); (void)sizeof(message); } while (false)

#endif

#endif // CHESSDEBUG_H

