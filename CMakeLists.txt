cmake_minimum_required(VERSION 3.10)
project(LogSummarizerLLM CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add json include path (adjust if different)
include_directories(include)

find_package(CURL REQUIRED)

add_executable(logsummarizer
    src/main.cpp
    src/Types.h
    src/Config.h
    src/LogParser.h
    src/LogParser.cpp
    src/ReportGenerator.h
    src/ReportGenerator.cpp
    src/OllamaClient.h
    src/OllamaClient.cpp
)

target_link_libraries(logsummarizer PRIVATE CURL::libcurl)
