#include <HttpClient.hpp>
using namespace CanToCloud::http;

bool HttpClient::initializeCurl()
{
    CURL *curl = curl_easy_init();
    return curl != nullptr ? true : false;
}

bool HttpClient::publish(std::string &url,std::string &data)
{
    if(curl)
    {
        CURLcode result;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        result = curl_easy_perform(curl);
        if(result!=CURLE_OK)
        {
            return false;
        }
        else
        {
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
    if(curl)
    {
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, respCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        return response;
    }
    else
    {
        std::cout<<"curl not initialized"<<std::endl;
        return std::string("");
    }
}