#include "ReportGenerator.h"
#include "Config.h"

#include <map>
#include <sstream>
#include <algorithm>

std::string ReportGenerator::generateLocalSummary(const std::vector<LogEntry>& entries) {
    std::ostringstream out;

    out << "Total log entries: " << entries.size() << "\n";

    // Count by severity buckets
    std::map<int, size_t> severityCounts;
    std::map<std::string, size_t> hostCounts;
    size_t failedLoginCount = 0;

    for (const auto& e : entries) {
        severityCounts[e.severityNumeric]++;
        hostCounts[e.host]++;

        // Simple heuristic for failed logins
        std::string lowerMsg;
        lowerMsg.reserve(e.message.size());
        for (char c : e.message) {
            lowerMsg.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }

        if (lowerMsg.find("failed login") != std::string::npos ||
            lowerMsg.find("authentication failure") != std::string::npos ||
            lowerMsg.find("invalid password") != std::string::npos) {
            failedLoginCount++;
        }
    }

    out << "Severity distribution (numeric approximate):\n";
    for (const auto& p : severityCounts) {
        out << "  Severity " << p.first << ": " << p.second << " events\n";
    }

    out << "Top hosts by event count:\n";
    // Convert hostCounts to vector for sorting
    std::vector<std::pair<std::string, size_t>> hostVec(hostCounts.begin(), hostCounts.end());
    std::sort(hostVec.begin(), hostVec.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    size_t limit = std::min<size_t>(5, hostVec.size());
    for (size_t i = 0; i < limit; ++i) {
        out << "  " << hostVec[i].first << ": " << hostVec[i].second << " events\n";
    }

    out << "Detected possible failed login events: " << failedLoginCount << "\n";

    return out.str();
}

std::string ReportGenerator::buildPromptForLLM(const std::vector<LogEntry>& entries) {
    std::ostringstream prompt;

    prompt << "You are a cybersecurity SOC analyst assistant.\n"
           << "You will be given a local pre-analysis summary and a subset of raw log entries.\n"
           << "Your tasks:\n"
           << "1. Provide a concise incident summary (3–5 bullet points).\n"
           << "2. Identify likely threats or attack patterns.\n"
           << "3. Suggest triage priorities for an analyst.\n"
           << "4. Suggest 3–5 recommended next steps (e.g., block IP, reset password, collect more logs).\n\n";

    // Add local summary
    prompt << "=== LOCAL ANALYSIS SUMMARY ===\n";
    prompt << generateLocalSummary(entries) << "\n\n";

    // Include a limited number of raw entries
    prompt << "=== SAMPLE RAW LOG ENTRIES (capped at " 
           << Config::MAX_LOG_ENTRIES_FOR_PROMPT << ") ===\n";

    size_t count = 0;
    for (const auto& e : entries) {
        if (count >= Config::MAX_LOG_ENTRIES_FOR_PROMPT) {
            prompt << "[... additional log entries truncated for brevity ...]\n";
            break;
        }
        prompt << "[" << e.timestamp << "] "
               << "host=" << e.host << " "
               << "severity=" << e.severityStr << " "
               << "msg=" << e.message << "\n";
        ++count;
    }

    prompt << "\nPlease provide your analysis now.\n";
    return prompt.str();
}
