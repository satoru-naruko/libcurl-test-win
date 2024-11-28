#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>
#include "HttpHandler.h"

class CommandProcessor {
public:
    CommandProcessor();
    void start();
    void stop();
    void addCommand(const std::string& command);
    void processCommands();
    void timerFunction();

private:
    std::queue<std::string> commandQueue;
    std::mutex queueMutex;
    std::condition_variable conditionVar;
    bool stopProgram;
    std::thread workerThread;
    std::thread timerThread;
    HttpHandler httpHandler;
};

#endif // COMMANDPROCESSOR_H
