#include "HttpClient.h"

#include <memory>

#include "httplib.h"

class HttplibHttpClient::Impl {
public:
    Impl(const std::string& host, int port, int connect_timeout_seconds, int read_timeout_seconds) : client(host, port) {
        client.set_connection_timeout(connect_timeout_seconds, 0);
        client.set_read_timeout(read_timeout_seconds, 0);
    }

    httplib::Client client;
};

HttplibHttpClient::HttplibHttpClient(const std::string& host, int port, int connect_timeout_seconds, int read_timeout_seconds)
    : impl_(new Impl(host, port, connect_timeout_seconds, read_timeout_seconds)) {}

HttplibHttpClient::~HttplibHttpClient() = default;

HttpResponse HttplibHttpClient::get(const std::string& path) {
    const auto response = impl_->client.Get(path.c_str());
    if (!response) {
        return {false, 0, "", "HTTP request failed"};
    }
    return {true, response->status, response->body, ""};
}
