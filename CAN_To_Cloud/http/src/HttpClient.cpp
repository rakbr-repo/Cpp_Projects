#include <HttpClient.hpp>
using namespace CanToCloud::http;
std::unique_ptr<HttpClient> HttpClient::httpClientInstance = nullptr;

bool HttpClient::initializeCurl()
{
    curl_ = curl_easy_init();
    return curl_ != nullptr ? true : false;
}

bool HttpClient::publish(std::string &url,std::string &data)
{
    if(curl_)
    {
        CURLcode result;
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_HTTPPOST, 1L);
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str());
        result = curl_easy_perform(curl_);
        if(result!=CURLE_OK)
        {
            std::cout<<"Publish failed due to : "<<(int)result<<std::endl;
            return false;
        }
        else
        {
            std::cout<<"Publish successful"<<std::endl;
            return true;
        }
    }
    else
    {
        std::cout<<"curl not initialized"<<std::endl;
        return false;
    }
}

size_t HttpClient::respCallBack(void* contents, size_t size, size_t nmemb, void* userp)
{
    std::string* resp = (std::string*)userp;
    resp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string HttpClient::getRequest(std::string &url)
{
    if(curl_)
    {
        std::string response;
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, HttpClient::respCallBack);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

        return response;
    }
    else
    {
        std::cout<<"curl not initialized"<<std::endl;
        return std::string("");
    }
}

void HttpClient::destroyCurl()
{
    if (curl_) curl_easy_cleanup(curl_);
}