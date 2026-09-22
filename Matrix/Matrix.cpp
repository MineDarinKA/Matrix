#include <iostream>
#include <sstream>
#include <string>

int main()
{
    int speed;
    int length;
    char epilepsyMode;

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