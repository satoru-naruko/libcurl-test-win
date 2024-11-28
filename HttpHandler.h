#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <string>
#include <curl/curl.h>

class HttpHandler {
public:
    HttpHandler() : m_curl(nullptr), multi_handle(nullptr) {};
    ~HttpHandler() {};
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out);
    void performGetRequest(const std::string& url);
    void performPostRequest(const std::string& url, const std::string& postData);

private:
    void sendRequest(std::string& response);
    CURL* m_curl;
    CURLM* multi_handle;
};

#endif // HTTPHANDLER_H
