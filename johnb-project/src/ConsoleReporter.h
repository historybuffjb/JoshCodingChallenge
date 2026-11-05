#pragma once

#include <string>

#include "Light.h"

class ConsoleReporter {
public:
    void initial_state(const LightStates& lights) const;
    void added(const Light& light) const;
    void changed(const Light& previous, const Light& current) const;
    void removed(const Light& light) const;
    void diagnostic(const std::string& message) const;
};
