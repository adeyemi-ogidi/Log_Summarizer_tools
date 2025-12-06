# Project Overview
This project implements a cybersecurity analyst support tool that uses a locally hosted Large Language Model (LLM) through OLLAMA to automate log analysis and incident summarization. Developed in C++ with secure coding principles, the tool ingests raw security logs, parses and analyzes key events, and generates structured summaries to assist analysts in triage and decision-making. The system combines traditional log-processing techniques with LLM-powered reasoning to highlight potential threats, identify suspicious behavior, and provide actionable recommendations. This prototype demonstrates how AI-assisted automation can enhance SOC workflows while maintaining on-premise privacy, performance, and operational security.

## How to Compile the Program

There are two methods for compiling the program:
a. Using CMAKE and

b. running the command g++ -std=c++17 src/main.cpp  src/LogParser.cpp  src/ReportGenerator.cpp src/OllamaClient.cpp  -Iinclude -lcurl -o logsummarizer. 


For this program we compiled using CMAKE

## How to run
Run the program using the command: "./logsummarizer ../Sample_Logs.csv --model llama3 --url http://localhost:11434"


## Dependencies:
1. c++ compiler {g++ (GNU C++ Compiler)}
2. libcurl (HTTP Client Library)
3. nlohmann/json (JSON Parsing Library) downloaded  from https://github.com/nlohmann/json
4. OLLAMA (Local LLM Runtime)
5. Standard C++ Libraries "<iostream>,<fstream>, <sstream>, <string>, <vector>, <stdexcept>,<algorithm>"


## Parser Logic and API call logic
The logparser.cpp, logParser.h, Types.h all allowed the programm to open the input csv file, read the input line by line and also split it into timestamp, severity, message etc and also converts it to an object that the programm can process
It is referenced in the main.cpp program:   std::vector<LogEntry> entries = LogParser::parseCsv(logPath);

API LOGIC: The modules /OllamaClient.cpp and OllamaClient.h, implement API call logic to OLLAMA server via the address http://localhost:11434/api/chat using libcurl

Below is the block of code within ollamaClient.cpp that shows the API call

curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
CURLcode res = curl_easy_perform(curl);





