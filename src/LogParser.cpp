#include "LogParser.h"
#include "Config.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <limits>
#include <sys/stat.h>

// Simple helper to check file size securely
static size_t getFileSize(const std::string& path) {
    struct stat st{};
    if (stat(path.c_str(), &st) != 0) {
        throw std::runtime_error("Failed to stat file: " + path);
    }
    return static_cast<size_t>(st.st_size);
}

// Split a line on comma (very basic CSV, no fancy quoting)
static std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, ',')) {
        fields.push_back(item);
    }
    return fields;
}

// Try to parse severity from string to int (e.g., "5", "LOW", "HIGH")
static int parseSeverity(const std::string& sev) {
    // Try numeric
    try {
        size_t idx = 0;
        int val = std::stoi(sev, &idx);
        if (idx == sev.size()) {
            return val;
        }
    } catch (...) {
        // ignore, fall through
    }

    // Map some typical severity words to numbers
    std::string lower;
    lower.reserve(sev.size());
    for (char c : sev) {
        lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    if (lower == "low") return 3;
    if (lower == "medium" || lower == "med") return 5;
    if (lower == "high") return 8;
    if (lower == "critical") return 10;

    return 0; // unknown
}

std::vector<LogEntry> LogParser::parseCsv(const std::string& filePath) {
    // Basic path validation: non-empty and reasonable length
    if (filePath.empty() || filePath.size() > 4096) {
        throw std::runtime_error("Invalid file path.");
    }

    // File size check to avoid huge inputs
    const size_t size = getFileSize(filePath);
    if (size > Config::MAX_LOG_FILE_BYTES) {
        throw std::runtime_error("Log file too large. Max allowed: " + 
                                 std::to_string(Config::MAX_LOG_FILE_BYTES) + " bytes.");
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open log file: " + filePath);
    }

    std::vector<LogEntry> entries;
    std::string line;
    bool isHeader = true;

    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Optional header skip (assuming first line is header)
        if (isHeader) {
            isHeader = false;
            // You can add validation of header here if needed
            continue;
        }

        const auto fields = splitCsvLine(line);
        if (fields.size() < 4) {
            // Robustness: skip malformed lines but log them
            std::cerr << "[WARN] Skipping malformed CSV line: " << line << "\n";
            continue;
        }

        LogEntry entry;
        entry.timestamp = fields[0];
        entry.host = fields[1];
        entry.severityStr = fields[2];
        entry.severityNumeric = parseSeverity(entry.severityStr);
        entry.message = fields[3];

        entries.push_back(std::move(entry));

        // Safety: stop if entries are ridiculously many (hard cap)
        if (entries.size() > 100000) {
            std::cerr << "[WARN] Too many log entries, stopping read at 100000.\n";
            break;
        }
    }

    return entries;
}
