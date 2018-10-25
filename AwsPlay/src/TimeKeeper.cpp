/**
 * @file TimeKeeper.cpp
 *
 * This module contains the implementations of the TimeKeeper class.
 *
 * © 2018 by Richard Walters
 */

#include "TimeKeeper.hpp"

#include <SystemAbstractions/Time.hpp>
#include <time.h>

/**
 * This contains the private properties of a TimeKeeper class instance.
 */
struct TimeKeeper::Impl {
    /**
     * This is used to interface with the operating system's notion of time.
     */
    SystemAbstractions::Time time;
};

TimeKeeper::~TimeKeeper() noexcept = default;

TimeKeeper::TimeKeeper()
    : impl_(new Impl())
{
}

double TimeKeeper::GetCurrentTime() {
    static const auto startTimeHighRes = impl_->time.GetTime();
    static const auto startTimeReal = (double)time(NULL);
    return startTimeReal + (impl_->time.GetTime() - startTimeHighRes);
}
