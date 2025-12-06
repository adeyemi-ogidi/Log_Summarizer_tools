#include "OllamaClient.h"

#include <stdexcept>
#include <sstream>
#include <iostream>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// libcurl callback to capture response body
static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    const size_t totalSize = size * nmemb;
    auto* response = static_cast<std::string*>(userdata);
    response->append(ptr, totalSize);
    return totalSize;
}

OllamaClient::OllamaClient(std::string baseUrl, std::string modelName)
    : baseUrl_(std::move(baseUrl)), modelName_(std::move(modelName)) {

    if (baseUrl_.empty()) {
        throw std::runtime_error("Ollama base URL cannot be empty.");
    }
    if (modelName_.empty()) {
        throw std::runtime_error("Ollama model name cannot be empty.");
    }
}

std::string OllamaClient::query(const std::string& prompt) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize libcurl.");
    }

    std::string url = baseUrl_ + "/api/chat";
    std::string responseBody;

    try {
        // Build JSON body
        json body;
        body["model"] = modelName_;
        body["messages"] = json::array({
            {
                {"role", "user"},
                {"content", prompt}
            }
        });
        // For non-streaming response
        body["stream"] = false;

        const std::string bodyStr = body.dump();

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, bodyStr.size());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

        // Security: enforce TLS verification if you use HTTPS
        // (for localhost HTTP it's not applicable; left here for future use)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::string err = "curl_easy_perform() failed: ";
            err += curl_easy_strerror(res);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            throw std::runtime_error(err);
        }

        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (httpCode < 200 || httpCode >= 300) {
            throw std::runtime_error("OLLAMA returned HTTP code " + std::to_string(httpCode) +
                                     " with body: " + responseBody);
        }

        // Parse JSON response
        json j = json::parse(responseBody);
        // For /api/chat, OLLAMA typically returns something like:
        // { "message": { "role": "assistant", "content": "..." }, ... }
        if (!j.contains("message") || !j["message"].contains("content")) {
            throw std::runtime_error("Unexpected OLLAMA response format: " + responseBody);
        }

        const std::string content = j["message"]["content"].get<std::string>();
        return content;
    } catch (...) {
        curl_easy_cleanup(curl);
        throw; // rethrow exception
    }
}
