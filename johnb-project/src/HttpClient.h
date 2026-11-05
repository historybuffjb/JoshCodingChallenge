#pragma once

#include <string>
#include <memory>

struct HttpResponse {
    bool transport_ok;
    int status;
    std::string body;
    std::string error;
};

class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    virtual HttpResponse get(const std::string& path) = 0;
};

class HttplibHttpClient : public IHttpClient {
public:
    HttplibHttpClient(const std::string& host, int port,
                      int connect_timeout_seconds = 2,
                      int read_timeout_seconds = 5);
    ~HttplibHttpClient() override;

    HttplibHttpClient(const HttplibHttpClient&) = delete;
    HttplibHttpClient& operator=(const HttplibHttpClient&) = delete;

    HttpResponse get(const std::string& path) override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
