#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

// Basic log entry structure for CSV logs
// Expected columns (example):
// timestamp, host, severity, message
struct LogEntry {
    std::string timestamp;
    std::string host;
    std::string severityStr;
    int severityNumeric{0}; // parsed from severityStr if possible
    std::string message;
};

#endif // TYPES_H
