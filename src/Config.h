#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    // Default OLLAMA base URL
    inline const std::string DEFAULT_OLLAMA_URL = "http://localhost:11434";
    // Default model name
    inline const std::string DEFAULT_MODEL_NAME = "llama3";

    // Security: limit how many log entries we send into the prompt
    inline const size_t MAX_LOG_ENTRIES_FOR_PROMPT = 200;
    // Optional: max file size in bytes (e.g., 5 MB)
    inline const size_t MAX_LOG_FILE_BYTES = 5 * 1024 * 1024;
}

#endif // CONFIG_H
