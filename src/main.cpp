#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "Types.h"
#include "Config.h"
#include "LogParser.h"
#include "ReportGenerator.h"
#include "OllamaClient.h"

static void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <log.csv> [--model MODEL] [--url BASE_URL]\n"
              << "\n"
              << "Example:\n"
              << "  " << progName << " sample_logs.csv --model llama3 --url http://localhost:11434\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string logPath;
    std::string modelName = Config::DEFAULT_MODEL_NAME;
    std::string baseUrl = Config::DEFAULT_OLLAMA_URL;

    logPath = argv[1];

    // Parse optional args
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--model" && i + 1 < argc) {
            modelName = argv[++i];
        } else if (arg == "--url" && i + 1 < argc) {
            baseUrl = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    try {
        std::cout << "[*] Parsing log file: " << logPath << "\n";
        std::vector<LogEntry> entries = LogParser::parseCsv(logPath);
        if (entries.empty()) {
            std::cerr << "[!] No valid log entries found. Exiting.\n";
            return 1;
        }

        std::cout << "[*] Generating local analysis summary.\n";
        std::string localSummary = ReportGenerator::generateLocalSummary(entries);
        std::cout << "=== LOCAL SUMMARY (for your info) ===\n";
        std::cout << localSummary << "\n";

        std::cout << "[*] Building prompt for LLM.\n";
        std::string prompt = ReportGenerator::buildPromptForLLM(entries);

        std::cout << "[*] Contacting OLLAMA at " << baseUrl 
                  << " using model '" << modelName << "'...\n";

        OllamaClient client(baseUrl, modelName);
        std::string llmResponse = client.query(prompt);

        std::cout << "\n=== LLM INCIDENT SUMMARY ===\n";
        std::cout << llmResponse << "\n";

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "[ERROR] " << ex.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "[ERROR] Unknown error occurred.\n";
        return 1;
    }
}
