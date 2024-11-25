#include "CommandProcessor.h"
#include <iostream>
#include <chrono>
#include <thread>

CommandProcessor::CommandProcessor() : stopProgram(false) {}

void CommandProcessor::start() {
    workerThread = std::thread(&CommandProcessor::processCommands, this);
    timerThread = std::thread(&CommandProcessor::timerFunction, this);
}

void CommandProcessor::stop() {
    stopProgram = true;
    conditionVar.notify_all();
    if (workerThread.joinable()) {
        workerThread.join();
    }
    if (timerThread.joinable()) {
        timerThread.join();
    }
}

void CommandProcessor::addCommand(const std::string& command) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        commandQueue.push(command);
    }
    conditionVar.notify_one();
}

void CommandProcessor::processCommands() {
    HttpHandler httpHandler;
    while (!stopProgram) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            conditionVar.wait(lock, [this] { return !commandQueue.empty() || stopProgram; });
        }

        if (stopProgram) break;

        while (!commandQueue.empty()) {
            std::string command = commandQueue.front();
            commandQueue.pop();

            httpHandler.performRequest(command);
        }
    }
}

void CommandProcessor::timerFunction() {
    while (!stopProgram) {
        std::this_thread::sleep_for(std::chrono::seconds(1000000));

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!stopProgram) {
                commandQueue.push("getstatus");
                std::cout << "Timer added command: getstatus" << std::endl;
            }
        }
        conditionVar.notify_one();
    }
}
