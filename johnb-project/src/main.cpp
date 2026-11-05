#include <chrono>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <iostream>
#include <string>

#include "ConsoleReporter.h"
#include "HttpClient.h"
#include "LightMonitor.h"
#include "LightsClient.h"

namespace {

const char* env_or_default(const char* name, const char* fallback) {
    const char* value = std::getenv(name);
    return value && *value ? value : fallback;
}

int positive_env(const char* name, const char* fallback, int maximum) {
    const std::string value = env_or_default(name, fallback);
    if (value.find_first_not_of("0123456789") != std::string::npos) {
        throw std::runtime_error(std::string(name) + " must be a positive integer");
    }
    const auto number = std::stoll(value);
    if (number < 1 || number > maximum) {
        throw std::runtime_error(std::string(name) + " is outside its supported range");
    }
    return static_cast<int>(number);
}

} // namespace

int main() {
    ConsoleReporter reporter;
    // Flush each event even when stdout is redirected to a file or pipe. This can be a nasty bug. 
    std::cout << std::unitbuf;
    try {
        const std::string host = env_or_default("LIGHTS_HOST", "127.0.0.1");
        const int port = positive_env("LIGHTS_PORT", "8080", 65535);
        const int interval_ms = positive_env("LIGHTS_POLL_MS", "1000", std::numeric_limits<int>::max());
        HttplibHttpClient http_client(host, port);
        LightsClient client(http_client);
        LightMonitor monitor(client, reporter, std::chrono::milliseconds(interval_ms));
        return monitor.run();
    } catch (const std::exception& error) {
        reporter.diagnostic(error.what());
        return 1;
    }
}
