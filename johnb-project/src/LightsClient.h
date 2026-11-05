#pragma once

#include <string>
#include <vector>

#include "HttpClient.h"
#include "Light.h"
#include "json.hpp"

class LightsClient {
public:
    explicit LightsClient(IHttpClient& http_client);

    std::vector<LightSummary> discover_lights();
    Light fetch_light(const LightSummary& summary);

private:
    nlohmann::json get_json(const std::string& path);

    IHttpClient& http_client_;
};
