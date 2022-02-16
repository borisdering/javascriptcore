#include <thread>
#include <chrono>
#include "timer.hpp"

template <typename Function>
void timer::set_timeout(Function function, int delay) {
    this->clear = false;
    std::thread thread([=]() {
        if(this->clear) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(this->clear) return;
        function();
    });
    thread.detach();
};

template <typename Function>
void timer::set_interval(Function function, int interval) {
    this->clear = false;
    std::thread thread([=]() {
        while (true) {
            if(this->clear) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(this->clear) return;
            function();
        }
    });
    thread.detach();
}

void timer::stop() {
    this->clear = true;
}