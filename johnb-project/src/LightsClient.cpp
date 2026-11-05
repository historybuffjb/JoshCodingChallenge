#include "LightsClient.h"

#include <set>
#include <stdexcept>

namespace {

using json = nlohmann::json;
LightSummary parse_summary(const json& value) {
    if (!value.is_object()) {
        throw std::runtime_error("light discovery entry is not a JSON object");
    }

    LightSummary summary;
    try {
        summary.id = value.at("id").get<std::string>();
        summary.name = value.at("name").get<std::string>();
        summary.room = value.at("room").get<std::string>();
    } catch (const json::exception& error) {
        throw std::runtime_error(std::string("missing or invalid discovery field: ") + error.what());
    }

    if (summary.id.empty()) {
        throw std::runtime_error("light discovery returned an empty id");
    }
    return summary;
}

Light parse_light(const json& value) {
    if (!value.is_object()) {
        throw std::runtime_error("light response is not a JSON object");
    }

    Light light;
    try {
        light.id = value.at("id").get<std::string>();
        light.name = value.at("name").get<std::string>();
        light.room = value.at("room").get<std::string>();
        light.on = value.at("on").get<bool>();

        const json& brightness = value.at("brightness");
        if (!brightness.is_number_integer()) {
            throw std::runtime_error("brightness is not an integer");
        }
        // Check before narrowing: very large JSON integers must not wrap into range.
        if (brightness < 0 || brightness > 255) {
            throw std::runtime_error("light brightness is outside 0-255");
        }
        light.raw_brightness = brightness.get<int>();
    } catch (const json::exception& error) {
        throw std::runtime_error(std::string("missing or invalid light field: ") + error.what());
    }

    if (light.id.empty()) {
        throw std::runtime_error("light has an empty id");
    }
    if (light.raw_brightness < 0 || light.raw_brightness > 255) {
        throw std::runtime_error("light brightness is outside 0-255");
    }
    return light;
}

} // namespace

LightsClient::LightsClient(IHttpClient& http_client) : http_client_(http_client) {}

std::vector<LightSummary> LightsClient::discover_lights() {
    const json summaries = get_json("/lights");
    // The Go simulator serializes an empty light collection as null.
    if (summaries.is_null()) {
        return {};
    }
    if (!summaries.is_array()) {
        throw std::runtime_error("GET /lights returned a non-array JSON value");
    }

    std::vector<LightSummary> lights;
    std::set<std::string> ids;
    for (const json& summary : summaries) {
        const LightSummary parsed = parse_summary(summary);
        if (!ids.insert(parsed.id).second) {
            throw std::runtime_error("GET /lights returned duplicate light id: " +
                                     parsed.id);
        }
        lights.push_back(parsed);
    }
    return lights;
}

Light LightsClient::fetch_light(const LightSummary& summary) {
    const Light light = parse_light(get_json("/lights/" + summary.id));
    if (light.id != summary.id) {
        throw std::runtime_error("light detail id does not match its discovery id");
    }
    return light;
}

nlohmann::json LightsClient::get_json(const std::string& path) {
    const HttpResponse response = http_client_.get(path);
    if (!response.transport_ok) {
        throw std::runtime_error("HTTP request failed for " + path +
                                 (response.error.empty() ? "" : ": " + response.error));
    }
    if (response.status != 200) {
        std::string message;
        try {
            const json error_body = json::parse(response.body);
            if (error_body.is_object() && error_body.contains("error") &&
                error_body.at("error").is_string()) {
                message = error_body.at("error").get<std::string>();
            }
        } catch (const json::parse_error&) {
            // Preserve the HTTP status when an error response is not JSON.
        }

        throw std::runtime_error("simulator returned HTTP " +
                                 std::to_string(response.status) +
                                 " for " + path +
                                 (message.empty() ? "" : ": " + message));
    }

    // we need to survive bad data
    try {
        return nlohmann::json::parse(response.body);
    } catch (const nlohmann::json::parse_error& error) {
        throw std::runtime_error("invalid JSON from simulator for " +
                                 path + ": " + error.what());
    }
}
