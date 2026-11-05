#pragma once

#include <chrono>

#include "Backoff.h"
#include "ConsoleReporter.h"
#include "LightsClient.h"

class LightMonitor {
public:
    LightMonitor(LightsClient& client, ConsoleReporter& reporter, std::chrono::milliseconds poll_interval);

    int run();

private:
    void report_changes(const LightStates& previous, const LightStates& current) const;
    bool initialize_snapshot();
    bool poll_once();

    LightsClient& client_;
    ConsoleReporter& reporter_;
    std::chrono::milliseconds poll_interval_;
    Backoff backoff_;
    LightStates previous_;
};
