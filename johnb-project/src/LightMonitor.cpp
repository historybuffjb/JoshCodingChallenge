#include "LightMonitor.h"

#include <exception>
#include <thread>

namespace {

const std::chrono::milliseconds kInitialBackoff(1000);
const std::chrono::milliseconds kMaximumBackoff(30000);

} // namespace

LightMonitor::LightMonitor(LightsClient& client, ConsoleReporter& reporter,
                           std::chrono::milliseconds poll_interval)
    : client_(client),
      reporter_(reporter),
      poll_interval_(poll_interval),
      backoff_(kInitialBackoff, kMaximumBackoff) {}

int LightMonitor::run() {
    while (!initialize_snapshot()) {
        const std::chrono::milliseconds delay = backoff_.next_delay();
        reporter_.diagnostic("initial snapshot incomplete; retrying after " +
                             std::to_string(delay.count()) + " ms");
        std::this_thread::sleep_for(delay);
    }

    backoff_.reset();
    std::chrono::milliseconds delay(0);
    // this is our main loop
    while (true) {
        // don't spam the server
        std::this_thread::sleep_for(delay);

        if (poll_once()) {
            backoff_.reset();
            delay = poll_interval_;
        } else {
            delay = backoff_.next_delay();
        }
    }

}

bool LightMonitor::initialize_snapshot() {
    std::vector<LightSummary> summaries;
    try {
        summaries = client_.discover_lights();
    } catch (const std::exception& error) {
        reporter_.diagnostic(std::string("initial discovery request failed: ") + error.what());
        return false;
    }

    LightStates initial;
    bool complete = true;
    // store all the lights
    for (const LightSummary& summary : summaries) {
        try {
            const Light light = client_.fetch_light(summary);
            initial.emplace(light.id, light);
        } catch (const std::exception& error) {
            complete = false;
            reporter_.diagnostic("initial detail request failed for " +
                                 summary.id + ": " + error.what());
        }
    }

    if (!complete) {
        return false;
    }
    // we set to previous to compare against
    previous_ = initial;
    reporter_.initial_state(previous_);
    return true;
}

bool LightMonitor::poll_once() {
    std::vector<LightSummary> summaries;
    try {
        // A failed discovery is not evidence of removal. Keep everything and
        // retry the complete discovery later.
        summaries = client_.discover_lights();
    } catch (const std::exception& error) {
        reporter_.diagnostic(std::string("discovery request failed; retaining previous state: ") +
                             error.what());
        return false;
    }

    // Discovery succeeded and was validated, so IDs absent from this set may
    // now be confirmed as removed by report_changes.
    LightStates current;
    bool detail_failure = false;
    for (const LightSummary& summary : summaries) {
        try {
            const Light light = client_.fetch_light(summary);
            current.emplace(light.id, light);
        } catch (const std::exception& error) {
            detail_failure = true;
            const auto old = previous_.find(summary.id);
            // we need to rollback
            if (old != previous_.end()) {
                current.emplace(old->first, old->second);
                reporter_.diagnostic("detail request failed for " + summary.id +
                                     "; retaining previous state: " + error.what());
            } else {
                reporter_.diagnostic("detail request failed for new light " + summary.id +
                                     "; waiting for a successful detail response: " +
                                     error.what());
            }
        }
    }

    report_changes(previous_, current);
    previous_ = current;
    return !detail_failure;
}
/*
either update or add our current states
*/
void LightMonitor::report_changes(const LightStates& previous,
                                  const LightStates& current) const {
    for (const auto& entry : current) {
        const auto old = previous.find(entry.first);
        if (old == previous.end()) {
            reporter_.added(entry.second);
        } else {
            reporter_.changed(old->second, entry.second);
        }
    }
    // remove lights that are no longer present
    for (const auto& entry : previous) {
        if (current.find(entry.first) == current.end()) {
            reporter_.removed(entry.second);
        }
    }
}
