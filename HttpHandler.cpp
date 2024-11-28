#include "HttpHandler.h"
#include <iostream>
#include <json/json.h>

size_t HttpHandler::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    size_t totalSize = size * nmemb;
    if (out) {
        out->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }
    return 0;
}

void HttpHandler::performGetRequest(const std::string& url) {
    std::string response;

    m_curl = curl_easy_init();
    if (m_curl) {
        curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response);

        sendRequest(response);
    }
}

void HttpHandler::performPostRequest(const std::string& url, const std::string& postData) {
    std::string response;

    m_curl = curl_easy_init();
    if (m_curl) {
        curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, postData.c_str());

        sendRequest(response);
    }
}

void HttpHandler::sendRequest(std::string& response) {
    multi_handle = curl_multi_init();
    curl_multi_add_handle(multi_handle, m_curl);

    int still_running = 0;
    curl_multi_perform(multi_handle, &still_running);

    while (still_running) {
        CURLMcode mc = curl_multi_perform(multi_handle, &still_running);

        if (still_running) {
            mc = curl_multi_poll(multi_handle, NULL, 0, 1000, NULL);
        }

        if (mc) {
            break;
        }
    }

    CURLMsg* msg;
    int msgs_left;
    while ((msg = curl_multi_info_read(multi_handle, &msgs_left)) != NULL) {
        if (msg->msg == CURLMSG_DONE) {
            CURL* completed_handle = msg->easy_handle;
            CURLcode result = msg->data.result;

            if (result != CURLE_OK) {
                fprintf(stderr, "Transfer failed: %s\n", curl_easy_strerror(result));
            }
            else {
                long http_code = 0;
                curl_easy_getinfo(completed_handle, CURLINFO_RESPONSE_CODE, &http_code);
                printf("Transfer completed successfully. status = %d\n", http_code);

                Json::CharReaderBuilder readerBuilder;
                Json::Value jsonData;
                std::string errors;

                std::istringstream responseStream(response);
                if (!Json::parseFromStream(readerBuilder, responseStream, &jsonData, &errors)) {
                    std::cerr << "JSON parsing error: " << errors << std::endl;
                }
                else {
                    std::cout << "Received JSON data: " << jsonData.toStyledString() << std::endl;
                }
            }

            curl_multi_remove_handle(multi_handle, completed_handle);
            curl_easy_cleanup(completed_handle);
        }
    }
    curl_multi_cleanup(multi_handle);
    m_curl = nullptr;
}
