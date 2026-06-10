#ifndef CHESSCLOCK_H
#define CHESSCLOCK_H


#include <chrono>
#include "chesstypes.h"


namespace ChessCore
{



/**
 * @class ChessClock
 * @brief Generic chess clock for timed games.
 *
 * This class manages the remaining time for both players in milliseconds.
 * It supports starting, pausing, resuming, switching turns, adding increment,
 * and checking whether a player has run out of time.
 *
 * The clock uses std::chrono::steady_clock, making it suitable for measuring
 * elapsed time because it is not affected by system clock changes.
 *
 * This class has no GUI or Qt dependency and is suitable for a pure chess core.
 */
class ChessClock
{
public:
    /**
     * @brief Clock type used for measuring elapsed time.
     *
     * std::chrono::steady_clock is monotonic and should not jump forward
     * or backward due to system time changes.
     */
    using Clock = std::chrono::steady_clock;

    /**
     * @brief Time point type used by the internal clock.
     */
    using TimePoint = Clock::time_point;

    /**
     * @brief Millisecond count type.
     *
     * Used for storing remaining time, increment, and elapsed time.
     */
    using MilliCount = int64_t;

public:
    /**
     * @brief Default constructor.
     *
     * Creates a disabled and stopped chess clock.
     */
    ChessClock() = default;

    /**
     * @brief Sets up the chess clock.
     *
     * Initializes both players with the same starting time and sets the
     * increment added after each completed move.
     *
     * If initialTimeMs is less than or equal to zero, the clock is disabled.
     * Negative values should be clamped to zero by the implementation.
     *
     * @param initialTimeMs Initial time for each player in milliseconds.
     * @param incrementsMs Increment added after each move in milliseconds.
     */
    void setup(MilliCount initialTimeMs, MilliCount incrementsMs = 0) noexcept;

    /**
     * @brief Clears the clock state.
     *
     * Resets both players' times, increment, running state, enabled state,
     * active colour, and internal timestamp.
     */
    void clear() noexcept;

    /**
     * @brief Starts the clock for a given colour.
     *
     * Sets the active colour and begins measuring elapsed time from now.
     * If the clock is disabled, this function does nothing.
     *
     * @param colour Colour whose clock should start running.
     */
    void start(PieceColour colour) noexcept;

    /**
     * @brief Pauses the currently running clock.
     *
     * Commits elapsed time before stopping so the active player's remaining
     * time is updated correctly.
     */
    void pause() noexcept;

    /**
     * @brief Resumes the clock.
     *
     * Restarts timing for the current active colour without counting the time
     * spent while paused.
     */
    void resume() noexcept;

    /**
     * @brief Switches the active clock to the next player.
     *
     * Usually called after a legal move.
     *
     * The implementation should:
     * - commit elapsed time for the old active player
     * - add increment to the old active player
     * - switch the active colour
     * - reset the internal timestamp
     *
     * @param nextColour Colour whose clock should run next.
     */
    void switchTurn(PieceColour nextColour) noexcept;

    /**
     * @brief Commits elapsed time to the active player.
     *
     * Calculates how much time has passed since the last tick and subtracts
     * it from the active player's remaining time.
     *
     * This function also resets the internal timestamp to the current time.
     */
    void commitElapsed() noexcept;

    /**
     * @brief Gets the remaining time for a player.
     *
     * If the requested colour is currently active and the clock is running,
     * this returns the stored time minus the elapsed time since the last tick.
     *
     * This function does not necessarily modify the stored time.
     *
     * @param colour Colour whose remaining time should be returned.
     * @return Remaining time in milliseconds. Returns 0 for invalid colours
     *         or when the calculated time is below zero.
     */
    MilliCount getTimeLeft(PieceColour colour) const noexcept;

    /**
     * @brief Checks whether a player has run out of time.
     *
     * @param colour Colour to check.
     * @return true if the clock is enabled and the player's remaining time
     *         is zero or less, otherwise false.
     */
    bool isFlagged(PieceColour colour) const noexcept;

    /**
     * @brief Gets the currently active colour.
     *
     * @return Colour whose clock is currently selected as active.
     */
    PieceColour getActiveColour() const noexcept;

    /**
     * @brief Checks whether the clock is enabled.
     *
     * A clock is usually enabled when it was set up with a positive initial
     * time value.
     *
     * @return true if the clock is enabled, otherwise false.
     */
    bool isEnabled() const noexcept;

    /**
     * @brief Checks whether the clock is currently running.
     *
     * @return true if the clock is actively measuring time, otherwise false.
     */
    bool isRunning() const noexcept;

    /**
     * @brief Gets elapsed milliseconds since a time point.
     *
     * Calculates the time difference between the current steady clock time
     * and the provided time point.
     *
     * @param point Previous time point.
     * @return Elapsed time in milliseconds.
     */
    MilliCount getElapsedMsSince(TimePoint point) const noexcept;

private:
    /**
     * @brief Subtracts elapsed time from a player's clock.
     *
     * The implementation should clamp the result to zero so the stored time
     * does not become negative.
     *
     * @param colour Colour whose time should be reduced.
     * @param elapsedMs Elapsed time in milliseconds.
     */
    void subtractTime(PieceColour colour, MilliCount elapsedMs) noexcept;

    /**
     * @brief Adds increment to a player's clock.
     *
     * Usually called after that player successfully completes a move.
     *
     * @param colour Colour whose time should receive the increment.
     */
    void addIncrement(PieceColour colour) noexcept;

private:
    /**
     * @brief Remaining time for white in milliseconds.
     */
    MilliCount m_whiteTimeMs = 0;

    /**
     * @brief Remaining time for black in milliseconds.
     */
    MilliCount m_blackTimeMs = 0;

    /**
     * @brief Increment added after each completed move in milliseconds.
     */
    MilliCount m_incrementMs = 0;

    /**
     * @brief Whether this clock is enabled.
     *
     * If false, timing functions should not affect player times.
     */
    bool m_enabled = false;

    /**
     * @brief Whether this clock is currently running.
     */
    bool m_running = false;

    /**
     * @brief Colour whose clock is currently active.
     */
    PieceColour m_activeColour = PieceColour::White;

    /**
     * @brief Last timestamp used to calculate elapsed time.
     *
     * Updated when the clock starts, resumes, commits elapsed time,
     * or switches turns.
     */
    TimePoint m_lastTick;
};

} // namespace ChessCore

#endif // CHESSCLOCK_H
