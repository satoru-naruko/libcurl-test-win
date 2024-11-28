#include "CommandProcessor.h" // Include the header file for CommandProcessor
#include <iostream>
#include <string>
#include <Windows.h>

// Control-C Handler
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
    case CTRL_C_EVENT:
        std::cout << "Control-C pressed, but ignored." << std::endl;
        return TRUE;
    default:
        return FALSE;
    }
}

int main() {

    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        std::cerr << "Error: Could not set control handler." << std::endl;
        return -1;
    }

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
