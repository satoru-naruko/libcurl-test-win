// libcurl-test-win.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <curl/curl.h>
#include <json/json.h>
#include <chrono>

std::queue<std::string> commandQueue;
std::mutex queueMutex;
std::condition_variable conditionVar;
bool stopProgram = false;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    size_t totalSize = size * nmemb;
    if (out) {
        out->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }
    return 0;
}

void httpWorker() {
    while (!stopProgram) {
        std::unique_lock<std::mutex> lock(queueMutex);
        conditionVar.wait(lock, [] { return !commandQueue.empty() || stopProgram; });

        if (stopProgram) break;

        while (!commandQueue.empty()) {
            std::string command = commandQueue.front();
            commandQueue.pop();
            lock.unlock();

            CURL* curl;
            CURLcode res;
            std::string response;

            curl = curl_easy_init();
            if (curl) {
                std::string url = "https://api.example.com/data?command=" + command;
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

                res = curl_easy_perform(curl);
                if (res != CURLE_OK) {
                    std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
                }
                else {
                    Json::CharReaderBuilder readerBuilder;
                    Json::Value jsonData;
                    std::string errors;

                    std::istringstream responseStream(response);
                    if (!Json::parseFromStream(readerBuilder, responseStream, &jsonData, &errors)) {
                        std::cerr << "JSON parsing error: " << errors << std::endl;
                    }
                    else {
                        std::cout << "Received JSON data: " << jsonData.toStyledString() << std::endl;
                    }
                }

                curl_easy_cleanup(curl);
            }

            lock.lock();
        }
    }
}

void timerThread() {
    while (!stopProgram) {
        std::this_thread::sleep_for(std::chrono::seconds(5));

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

int main() {
    std::thread workerThread(httpWorker);
    std::thread timer(timerThread);

    std::string input;
    while (true) {
        //std::cout << "Enter a command (or 'exit' to quit): ";
        //std::getline(std::cin, input);

        //if (input == "exit") {
        //    {
        //        std::lock_guard<std::mutex> lock(queueMutex);
        //        stopProgram = true;
        //    }
        //    conditionVar.notify_all();
        //    break;
        //}

        //{
        //    std::lock_guard<std::mutex> lock(queueMutex);
        //    commandQueue.push(input);
        //}
        //conditionVar.notify_one();
        Sleep(10);
    }

    timer.join();
    workerThread.join();
    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
