# Project Overview
This project implements a cybersecurity analyst support tool that uses a locally hosted Large Language Model (LLM) through OLLAMA to automate log analysis and incident summarization. Developed in C++ with secure coding principles, the tool ingests raw security logs, parses and analyzes key events, and generates structured summaries to assist analysts in triage and decision-making. The system combines traditional log-processing techniques with LLM-powered reasoning to highlight potential threats, identify suspicious behavior, and provide actionable recommendations. This prototype demonstrates how AI-assisted automation can enhance SOC workflows while maintaining on-premise privacy, performance, and operational security.

## How to Compile the Program

There are two methods for compiling the program:
a. Using CMAKE and
b. running the command g++ -std=c++17 src/main.cpp  src/LogParser.cpp  src/ReportGenerator.cpp src/OllamaClient.cpp  -Iinclude -lcurl -o logsummarizer. 
For this program we compiled using CMAKE

## How to run
Run the program using the command: "./logsummarizer ../Sample_Logs.csv --model llama3 --url http://localhost:11434"
# Log_Summarizer_tools
