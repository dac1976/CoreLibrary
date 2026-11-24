// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <15799155+dac1976@users.noreply.github.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License and GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// and GNU Lesser General Public License along with this program. If
// not, see <http://www.gnu.org/licenses/>.

/*!
 * \file PeriodicThread.h
 * \brief File containing declaration of PeriodicThread class.
 */
#ifndef PERIODICTHREAD_H
#define PERIODICTHREAD_H

#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
#include "features/PlatformFeatures.h" // $(HGLPRJ)/Common/Utils/Platform
#include "ThreadBase.h"
#include "SyncEvent.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

enum class eOverrunPolicy
{
    catchUp,
    skipMissedTicks,
    resynchronise
};

/*! \brief Periodic tick information supplied to the extended callback. */
struct PeriodicTickInfo
{
    using clock_t      = std::chrono::steady_clock;
    using time_point_t = clock_t::time_point;
    using duration_t   = clock_t::duration;

    /*! \brief Zero-based logical tick index, including skipped ticks. */
    uint64_t tickIndex{0};

    /*! \brief Number of ticks skipped since the previous callback. */
    uint64_t skippedTicks{0};

    /*! \brief Scheduled absolute time of the current tick. */
    time_point_t scheduledTime{};

    /*! \brief Actual time immediately before callback execution. */
    time_point_t actualTime{};

    /*! \brief Configured period used for the current scheduling decision. */
    duration_t period{};

    /*! \brief Difference between actual and scheduled tick times. */
    duration_t wakeupLatency{};

    /*! \brief Execution duration of the previous callback. */
    duration_t previousCallbackDuration{};

    /*! \brief Whether the current tick is a deliberate catch-up tick. */
    bool catchUpTick{false};

    /*! \brief Whether the timing reference has been resynchronised. */
    bool resynchronised{false};
};

/*!
 * \brief PeriodicThread constructor.
 *
 * eventCallback - Function object to be called when event ticks.
 * eventCallbackEx - Function object to be called when event ticks with extended info.
 * period - Period between event being triggered, must be > 0.
 * delayedStart - When true user must called Start() manually.
 * timeUnit - The unit of time associated with the period.
 * overrunPolicy - Policy to handle timer overruns.
 * maxCatchUpTicks - Maximum consecutive catch-up ticks, 0 implies unlimited catchup ticks.
 *
 * Only define one of the callbacks not both.
 */
struct PeriodicThreadSettings
{
    using event_callback_t    = std::function<void()>;
    using event_callback_ex_t = std::function<void(PeriodicTickInfo const&)>;

    event_callback_t    eventCallback{};
    event_callback_ex_t eventCallbackEx{};
    int64_t             period{0};
    bool                delayedStart{false};
    eWaitTimeUnit       timeUnit{eWaitTimeUnit::milliseconds};
    eOverrunPolicy      overrunPolicy{eOverrunPolicy::skipMissedTicks};
    uint32_t            maxCatchUpTicks{4};
};

/*!
 * \brief Class defining a periodic thread that executes a registered
 * callback at a specified rate using absolute monotonic clock deadlines.
 *
 * Unlike EventThread, callback execution time does not accumulate into
 * the scheduled tick period. Configurable overrun policies determine
 * how missed deadlines are handled.
 *
 * Actual callback timing remains subject to operating system scheduling
 * latency and clock resolution.
 */
class PeriodicThread final : public ThreadBase
{
    using clock_t      = std::chrono::steady_clock;
    using time_point_t = clock_t::time_point;
    using duration_t   = clock_t::duration;

public:
    /*!
     * \brief PeriodicThread constructor.
     * \param[in] settings - struct containing settings.
     *
     * Period must be greater than 0.
     */
    explicit PeriodicThread(PeriodicThreadSettings const& settings);

    /*! \brief PeriodicThread destructor. */
    ~PeriodicThread() OVERRIDE_;

    /*! \brief Copy constructor deleted.*/
    PeriodicThread(const PeriodicThread&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    PeriodicThread& operator=(const PeriodicThread&) = delete;
    /*! \brief Move constructor deleted.*/
    PeriodicThread(PeriodicThread&&) = delete;
    /*! \brief Move assignment operator deleted.*/
    PeriodicThread& operator=(PeriodicThread&&) = delete;

    /*!
     * \brief Set the time period between ticks of the event.
     * \param[in] period - Period between event being triggered.
     * \param[in] timeUnit - The unit of time associated with the period.
     *
     * Period must be greater than 0.
     */
    void SetPeriod(int64_t period, eWaitTimeUnit timeUnit = eWaitTimeUnit::milliseconds);
    /*!
     * \brief Get the time period between ticks of the event.
     * \return Period between event being triggered, in milliseconds.
     */
    int64_t Period(eWaitTimeUnit* timeUnit = nullptr) const;
    /*!
     * \brief Set the timer overrun policy.
     * \param[in] overrunPolicy - Policy to handle timer overruns.
     */
    void SetOverrunPolicy(eOverrunPolicy overrunPolicy);
    /*!
     * \brief Get the timer overrun policy.
     * \return Policy value.
     */
    eOverrunPolicy OverrunPolicy() const;
    /*!
     * \brief Set the max number of catch-up ticks.
     * \param[in] maxCatchUpTicks - Max catchup ticks value.
     */
    void SetMaxCatchUpTicks(uint32_t maxCatchUpTicks);
    /*!
     * \brief Get the max number of catch-up ticks.
     * \return Max catchup ticks value.
     */
    uint32_t MaxCatchUpTicks() const;

private:
    /*! \brief Thread function.*/
    void ThreadFunction() NO_EXCEPT_ OVERRIDE_;
    /*! \brief On termination function.*/
    void OnThreadTermination() NO_EXCEPT_ OVERRIDE_;
    /*! \brief Function to process termination conditions.*/
    void ProcessTerminationConditions() NO_EXCEPT_ OVERRIDE_;
    /*! \brief Function to convert time periods to <chrono> native format. .*/
    static duration_t ConvertPeriod(int64_t period, eWaitTimeUnit timeUnit);

private:
    /*! \brief Event period mutex.*/
    mutable std::mutex m_settingsMutex{};
    /*! \brief Update event.*/
    hgl::SyncEvent m_updateEvent{};
    /*! \brief Settings.*/
    PeriodicThreadSettings m_settings;
    /*! \brief Event tick period in native clock duration. */
    duration_t m_periodDuration{};
    /*! \brief Whether the timing reference has been initialised. */
    bool m_timingInitialised{false};
    /*! \brief Current consecutive catch-up tick count. */
    uint32_t m_consecutiveCatchUpTicks{0};
    /*! \brief Periodic tick info to pass to extended callback. */
    PeriodicTickInfo m_tickInfo{};
};

} // namespace threads
} // namespace core_lib

#endif // PERIODICTHREAD_H
