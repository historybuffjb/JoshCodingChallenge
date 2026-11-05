#include "ConsoleReporter.h"

#include <iostream>

#include "json.hpp"

namespace {

// Round brightness to the nearest integer percentage.
int brightness_percent(int raw_brightness) {
    return (raw_brightness * 100 + 127) / 255;
}

nlohmann::json full_state_json(const Light& light) {
    return {
        {"name", light.name},
        {"id", light.id},
        {"room", light.room},
        {"on", light.on},
        {"brightness", brightness_percent(light.raw_brightness)}
    };
}

} // namespace

void ConsoleReporter::initial_state(const LightStates& lights) const {
    nlohmann::json output = nlohmann::json::array();
    for (const auto& entry : lights) {
        output.push_back(full_state_json(entry.second));
    }
    std::cout << output.dump(2) << '\n';
}

void ConsoleReporter::added(const Light& light) const {
    std::cout << full_state_json(light).dump(2) << '\n';
}
void ConsoleReporter::changed(const Light& previous, const Light& current) const {
    if (previous.name != current.name) {
        std::cout << nlohmann::json{{"id", current.id}, {"name", current.name}}.dump(2)
                  << '\n';
    }
    if (previous.room != current.room) {
        std::cout << nlohmann::json{{"id", current.id}, {"room", current.room}}.dump(2)
                  << '\n';
    }
    if (previous.on != current.on) {
        std::cout << nlohmann::json{{"id", current.id}, {"on", current.on}}.dump(2) << '\n';
    }
    if (previous.raw_brightness != current.raw_brightness) {
        std::cout << nlohmann::json{
            {"id", current.id},
            {"brightness", brightness_percent(current.raw_brightness)}
        }.dump(2) << '\n';
    }
}
void ConsoleReporter::removed(const Light& light) const {
    std::cout << light.name << " (" << light.id << ") has been removed\n";
}

void ConsoleReporter::diagnostic(const std::string& message) const {
    std::cerr << nlohmann::json{{"error", message}}.dump() << '\n';
}
