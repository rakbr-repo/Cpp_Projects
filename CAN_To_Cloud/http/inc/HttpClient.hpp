#pragma once
#include <iostream>
#include <memory>
#include <curl/curl.h>

namespace CanToCloud
{
namespace http
{
    class HttpClient
    {
        private:
        static std::unique_ptr<HttpClient> httpClientInstance;
        static size_t respCallBack(void* contents, size_t size, size_t nmemb, void* userp);
        CURL *curl;

        public:
        HttpClient(const HttpClient&) = delete;
        HttpClient operator=(const HttpClient&) = delete;
        HttpClient() = default;
        ~HttpClient() = default;
        bool initializeCurl();
        void destroyCurl();
        bool publish(std::string &url,std::string &data);
        std::string getRequest(std::string &url);

        static HttpClient& getInstance()
        {
            if(!httpClientInstance)
            {
                httpClientInstance = std::make_unique<HttpClient>();
            }
            return *httpClientInstance;
        }
    };
}
}