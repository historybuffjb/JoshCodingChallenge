#pragma once

#include <chrono>

class Backoff {
public:
    Backoff(std::chrono::milliseconds initial,
            std::chrono::milliseconds maximum);

    void reset();
    std::chrono::milliseconds next_delay();

private:
    std::chrono::milliseconds initial_;
    std::chrono::milliseconds maximum_;
    std::chrono::milliseconds current_;
};
