#include <iostream>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

// Callback function for libcurl to write response data into a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

int main() {
    CURL* curl;
    CURLcode res;
    std::string url = "http://localhost:8000"; // Replace with your server's URL
    std::string operation = "add"; // Replace with the desired operation
    std::string request_data;

    // Create a JSON request
    rapidjson::Document request_json;
    request_json.SetObject();
    request_json.AddMember("operation", rapidjson::Value(operation.c_str(), request_json.GetAllocator()), request_json.GetAllocator());
    rapidjson::Value operands(rapidjson::kArrayType);
    operands.PushBack(5, request_json.GetAllocator());
    operands.PushBack(3, request_json.GetAllocator());
    request_json.AddMember("operands", operands, request_json.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    request_json.Accept(writer);
    request_data = buffer.GetString();

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL and HTTP method
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data.c_str());

        // Specify the response callback function and container
        std::string response_data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        // Perform the HTTP POST request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Parse the JSON response
            rapidjson::Document response_json;
            response_json.Parse(response_data.c_str());

            if (response_json.HasMember("result")) {
                int result = response_json["result"].GetInt();
                std::cout << "Result from RPC server: " << result << std::endl;
            } else if (response_json.HasMember("error")) {
                std::cerr << "RPC error: " << response_json["error"].GetString() << std::endl;
            } else {
                std::cerr << "Invalid response from server." << std::endl;
            }
        }

        // Clean up libcurl resources
        curl_easy_cleanup(curl);
    }

    // Clean up libcurl global resources
    curl_global_cleanup();

    return 0;
}
