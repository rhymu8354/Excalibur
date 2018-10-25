#ifndef TIME_KEEPER_HPP
#define TIME_KEEPER_HPP

/**
 * @file TimeKeeper.hpp
 *
 * This module declares the TimeKeeper implementation.
 *
 * © 2018 by Richard Walters
 */

#include <Http/TimeKeeper.hpp>
#include <memory>

/**
 * This is the implementation of Http::TimeKeeper used
 * by the actual web server.
 */
class TimeKeeper
    : public Http::TimeKeeper
{
    // Lifecycle Methods
public:
    ~TimeKeeper() noexcept;
    TimeKeeper(const TimeKeeper&) = delete;
    TimeKeeper(TimeKeeper&&) noexcept = delete;
    TimeKeeper& operator=(const TimeKeeper&) = delete;
    TimeKeeper& operator=(TimeKeeper&&) noexcept = delete;

    // Public Methods
public:
    /**
     * This is the constructor of the class.
     */
    TimeKeeper();

    // Http::TimeKeeper
public:
    virtual double GetCurrentTime() override;

    // Private properties
private:
    /**
     * This is the type of structure that contains the private
     * properties of the instance.  It is defined in the implementation
     * and declared here to ensure that it is scoped inside the class.
     */
    struct Impl;

    /**
     * This contains the private properties of the instance.
     */
    std::unique_ptr< Impl > impl_;
};

#endif /* TIME_KEEPER_HPP */
