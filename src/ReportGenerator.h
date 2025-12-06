#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <string>
#include <vector>
#include "Types.h"

class ReportGenerator {
public:
    // Generate a human-readable local summary (stats, notable items)
    static std::string generateLocalSummary(const std::vector<LogEntry>& entries);

    // Build a prompt string to send to the LLM
    static std::string buildPromptForLLM(const std::vector<LogEntry>& entries);
};

#endif // REPORTGENERATOR_H
