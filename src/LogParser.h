#ifndef LOGPARSER_H
#define LOGPARSER_H

#include <string>
#include <vector>
#include "Types.h"

class LogParser {
public:
    // Parse a CSV log file into a vector of LogEntry
    // Throws std::runtime_error on error.
    static std::vector<LogEntry> parseCsv(const std::string& filePath);
};

#endif // LOGPARSER_H
