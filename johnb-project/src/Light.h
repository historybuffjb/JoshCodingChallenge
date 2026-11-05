#pragma once

#include <map>
#include <string>

struct Light {
    std::string id;
    std::string name;
    std::string room;
    bool on;
    int raw_brightness;
};

struct LightSummary {
    std::string id;
    std::string name;
    std::string room;
};

using LightStates = std::map<std::string, Light>;
