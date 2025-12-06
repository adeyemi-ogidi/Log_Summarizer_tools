#ifndef OLLAMACLIENT_H
#define OLLAMACLIENT_H

#include <string>

class OllamaClient {
public:
    OllamaClient(std::string baseUrl, std::string modelName);

    // Send a prompt to OLLAMA and return the model's response (full text).
    // Throws std::runtime_error on error.
    std::string query(const std::string& prompt);

private:
    std::string baseUrl_;
    std::string modelName_;
};

#endif // OLLAMACLIENT_H
