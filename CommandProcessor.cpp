#include "CommandProcessor.h"
#include <iostream>
#include <chrono>
#include <thread>

#include "json/json.h"

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
    while (!stopProgram) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            conditionVar.wait(lock, [this] { return !commandQueue.empty() || stopProgram; });
        }

        if (stopProgram) break;

        while (!commandQueue.empty()) {
            std::string command = commandQueue.front();
            commandQueue.pop();

            if (command == "getstatus") {
                httpHandler.performGetRequest("https://jsonplaceholder.typicode.com/posts/1");
            } else if (command == "register") {

                Json::Value jsonData;
                jsonData["name"] = "John Doe";
                jsonData["age"] = 30;
                jsonData["job"] = "Developer";

                Json::StreamWriterBuilder writer;
                std::string postData = Json::writeString(writer, jsonData);
                httpHandler.performPostRequest("https://jsonplaceholder.typicode.com/posts/", postData);
            }
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
