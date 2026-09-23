#include <iostream>
#include <sstream>
#include <string>
#include "Character.h"

int main(int argc, char* argv[])
{
    int speed;
    int length;
    char epilepsyMode;

    // Display command line usage information.
    if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "/?")) {
        std::cout << "Usage: Matrix.exe [speed] [length] [epilepsy mode]\n";
        std::cout << "Speed: 1-30 characters per second\n";
        std::cout << "Line length: 1-30 characters\n";
        std::cout << "Epilepsy mode: Y/N\n";
        return 0;
    }

    // Read and validate parameters from the command line.
    if (argc == 4) {
        std::stringstream speedStream(argv[1]);
        std::stringstream lengthStream(argv[2]);
        std::string mode = argv[3];

        std::string remainingInput;

        if (speedStream >> speed &&
            !(speedStream >> remainingInput) &&
            speed >= 1 &&
            speed <= 30) {
        }
        else {
            std::cout << "Invalid speed. Speed must be between 1 and 30.\n";
            return 1;
        }

        if (lengthStream >> length &&
            !(lengthStream >> remainingInput) &&
            length >= 1 &&
            length <= 30) {
        }
        else {
            std::cout << "Invalid length. Length must be between 1 and 30.\n";
            return 1;
        }

        if (mode.length() == 1 &&
            (mode[0] == 'Y' ||
                mode[0] == 'y' ||
                mode[0] == 'N' ||
                mode[0] == 'n')) {
            epilepsyMode = mode[0];
        }
        else {
            std::cout << "Invalid epilepsy mode. Use Y or N.\n";
            return 1;
        }
    }
    else if (argc == 1) {
        std::string input;

        // Request and validate line speed.
        while (true) {
            std::cout << "Enter speed (1-30): ";
            std::getline(std::cin, input);

            std::stringstream stream(input);

            if (stream >> speed) {
                std::string remainingInput;

                if (!(stream >> remainingInput) && speed >= 1 && speed <= 30) {
                    break;
                }
            }

            std::cout << "Invalid speed. ";
        }

        // Request and validate line length.
        while (true) {
            std::cout << "Enter line length (1-30): ";
            std::getline(std::cin, input);

            std::stringstream stream(input);

            if (stream >> length) {
                std::string remainingInput;

                if (!(stream >> remainingInput) && length >= 1 && length <= 30) {
                    break;
                }
            }

            std::cout << "Invalid length. ";
        }

        // Request and validate epilepsy mode.
        while (true) {
            std::cout << "Epilepsy mode (Y/N): ";
            std::getline(std::cin, input);

            if (input.length() == 1 &&
                (input[0] == 'Y' ||
                    input[0] == 'y' ||
                    input[0] == 'N' ||
                    input[0] == 'n')) {
                epilepsyMode = input[0];
                break;
            }

            std::cout << "Invalid mode. ";
        }
    }
    else {
        std::cout << "Invalid number of parameters.\n";
        std::cout << "Use Matrix.exe --help for usage information.\n";
        return 1;
    }
}