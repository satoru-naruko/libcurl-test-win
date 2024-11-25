#include "HttpHandler.h"
#include "CommandProcessor.h" // Include the header file for CommandProcessor
#include <iostream>
#include <string>
#include <Windows.h>

int main() {
    CommandProcessor commandProcessor;
    commandProcessor.start();

    std::string input;
    while (true) {
        std::cout << "Enter a command (or 'exit' to quit): ";
        std::getline(std::cin, input);

        if (input == "exit") {
            commandProcessor.stop();
            break;
		}
        else if (input == "getstatus") {
            commandProcessor.addCommand(input);
        }
		else if (input.empty()) {
			continue;
		}
        Sleep(10);
    }

    commandProcessor.stop();
    return 0;
}
