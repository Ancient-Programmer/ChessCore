#include "chessclock.h"


namespace ChessCore
{

void ChessClock::setup(MilliCount initialTimeMs, MilliCount incrementMs) noexcept
{
    if(initialTimeMs < 0) initialTimeMs = 0;
    if(incrementMs < 0) incrementMs = 0;

    m_whiteTimeMs = initialTimeMs;
    m_blackTimeMs = initialTimeMs;
    m_incrementMs = incrementMs;

    m_enabled = initialTimeMs > 0;
    m_running = false;

    m_activeColour = PieceColour::White;
    m_lastTick = Clock::now();
}



void ChessClock::clear() noexcept
{
    m_whiteTimeMs = 0;
    m_blackTimeMs = 0;
    m_incrementMs = 0;


    m_enabled = false;
    m_running = false;

    m_activeColour = PieceColour::White;
    m_lastTick = Clock::now();
}


void ChessClock::start(PieceColour colour) noexcept
{
    if(!m_enabled)
    {
        return;
    }

    if(colour != PieceColour::White && colour != PieceColour::Black)
    {
        colour = PieceColour::White;
    }

    m_activeColour = colour;
    m_running = true;
    m_lastTick = Clock::now();
}


void ChessClock::pause() noexcept
{
    if(!m_enabled || !m_running)
        return;

    commitElapsed();
    m_running = false;
}

void ChessClock::resume() noexcept
{
    if (!m_enabled || m_running)
        return;

    m_running = true;
    m_lastTick = Clock::now();
}

void ChessClock::switchTurn(PieceColour nextColour) noexcept
{
    if (nextColour != PieceColour::White && nextColour != PieceColour::Black)
        return;

    if (!m_enabled)
    {
        m_activeColour = nextColour;
        return;
    }

    if (m_running)
    {
        const PieceColour oldColour = m_activeColour;

        commitElapsed();
        addIncrement(oldColour);
    }

    m_activeColour = nextColour;
    m_lastTick = Clock::now();
}

void ChessClock::commitElapsed() noexcept
{
    if (!m_enabled || !m_running)
        return;

    const MilliCount elapsed = getElapsedMsSince(m_lastTick);

    subtractTime(m_activeColour, elapsed);

    m_lastTick = Clock::now();
}

ChessClock::MilliCount ChessClock::getTimeLeft(PieceColour colour) const noexcept
{
    MilliCount baseTime = 0;

    if (colour == PieceColour::White)
        baseTime = m_whiteTimeMs;
    else if (colour == PieceColour::Black)
        baseTime = m_blackTimeMs;
    else
        return 0;

    if (m_enabled && m_running && colour == m_activeColour)
    {
        baseTime -= getElapsedMsSince(m_lastTick);
    }

    return baseTime > 0 ? baseTime : 0;
}

bool ChessClock::isFlagged(PieceColour colour) const noexcept
{
    return m_enabled && getTimeLeft(colour) <= 0;
}

PieceColour ChessClock::getActiveColour() const noexcept
{
    return m_activeColour;
}

bool ChessClock::isEnabled() const noexcept
{
    return m_enabled;
}

bool ChessClock::isRunning() const noexcept
{
    return m_running;
}

void ChessClock::subtractTime(PieceColour colour, MilliCount elapsedMs) noexcept
{
    if (elapsedMs <= 0)
        return;

    if (colour == PieceColour::White)
    {
        m_whiteTimeMs -= elapsedMs;

        if (m_whiteTimeMs < 0)
            m_whiteTimeMs = 0;
    }
    else if (colour == PieceColour::Black)
    {
        m_blackTimeMs -= elapsedMs;

        if (m_blackTimeMs < 0)
            m_blackTimeMs = 0;
    }
}

void ChessClock::addIncrement(PieceColour colour) noexcept
{
    if (m_incrementMs <= 0)
        return;

    if (colour == PieceColour::White)
        m_whiteTimeMs += m_incrementMs;
    else if (colour == PieceColour::Black)
        m_blackTimeMs += m_incrementMs;
}


ChessClock::MilliCount ChessClock::getElapsedMsSince(TimePoint point) const  noexcept
{
    const auto now = Clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(
               now - point
               ).count();
}

}


