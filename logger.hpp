#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#pragma once

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    Logger(const Logger&) = delete; // Delete copy constructor
    Logger& operator=(const Logger&) = delete; // Delete copy assignment operator

    void log(const std::string& message) {
        std::lock_guard<std::mutex> guard(logMutex);
        if (logFile.is_open()) {
            logFile << message << std::endl;
            // Also echo to console if needed
            std::cout << message << std::endl;
        }
    }

    // Optionally provide methods to control the logger, like opening/closing files
    void openLogFile(const std::string& fileName) {
        std::lock_guard<std::mutex> guard(logMutex);
        logFile.open(fileName, std::ios::app);
    }

    void closeLogFile() {
        std::lock_guard<std::mutex> guard(logMutex);
        if (logFile.is_open()) {
            logFile.close();
        }
    }

private:
    std::ofstream logFile;
    std::mutex logMutex; // Ensures thread-safe logging

    // Private constructor
    Logger() = default;

    // Private destructor
    ~Logger() {
        closeLogFile();
    }
};
