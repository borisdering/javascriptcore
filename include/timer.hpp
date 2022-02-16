//
// Created by Boris Dering on 29.11.20.
//

#ifndef TIMER_HPP
#define TIMER_HPP

#include <atomic>

/**
 * Thread save timer class. Using atomic.
 * @note:   Only one operations is permitted each time. Multiple usage is therefore
 *          prohibited and call cause undefined behaviour in your application.
 */
class timer {
public:

    /**
     * Set timeout and dispatch. Passed callback will be executed once the
     * delay has passed.
     * @tparam Function
     * @param function
     * @param delay
     */
    template<typename Function>
    void set_timeout(Function function, int delay);

    /**
     * Set interval and dispatch. Will be executed each time the interval has passed.
     * @tparam Function
     * @param function
     * @param interval
     */
    template<typename Function>
    void set_interval(Function function, int interval);

    /**
     * Stop any running operations.
     */
    void stop();
private:

    /**
     * Atomic boolean value is used to identify if timer is running.
     */
    std::atomic<bool> clear{false};
};


#endif //TIMER_HPP