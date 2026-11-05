#include "Backoff.h"

#include <algorithm>

Backoff::Backoff(std::chrono::milliseconds initial,
                 std::chrono::milliseconds maximum)
    : initial_(initial), maximum_(maximum), current_(initial) {}

void Backoff::reset() {
    current_ = initial_;
}

std::chrono::milliseconds Backoff::next_delay() {
    const std::chrono::milliseconds delay = current_;
    current_ = std::min(current_ * 2, maximum_);
    return delay;
}
