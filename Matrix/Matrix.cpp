#include <iostream>
#include <sstream>
#include <string>
#include "ApplicationSettings.h"
#include "ApplicationManager.h"

int main(int argc, char* argv[])
{
    int frequency;
    int speed;
    int length;
    char epilepsyMode;

    int explosionProbability;
    int minExplosionRadius;
    int maxExplosionRadius;

    // Display command line usage information
    if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "/?"))
    {
        std::cout << "Usage: Matrix.exe [frequency] [speed] [length] [epilepsy mode] [explosion probability] [min radius] [max radius]\n";
        std::cout << "Frequency: 1-30 lines per second\n";
        std::cout << "Speed: 1-30 characters per second\n";
        std::cout << "Line length: 1-30 characters\n";
        std::cout << "Epilepsy mode: Y/N\n";
        std::cout << "Explosion probability: 1-1000 (out of 1000 per step)\n";
        std::cout << "Minimum explosion radius: 1-10\n";
        std::cout << "Maximum explosion radius: 1-10 (not less than minimum)\n";
        return 0;
    }

    // Read and validate parameters from the command line
    if (argc == 8)
    {
        std::stringstream frequencyStream(argv[1]);
        std::stringstream speedStream(argv[2]);
        std::stringstream lengthStream(argv[3]);
        std::string mode = argv[4];

        std::stringstream probabilityStream(argv[5]);
        std::stringstream minRadiusStream(argv[6]);
        std::stringstream maxRadiusStream(argv[7]);

        std::string remainingInput;

        if (!(frequencyStream >> frequency) ||
            (frequencyStream >> remainingInput) ||
            frequency < 1 ||
            frequency > 30)
        {
            std::cout << "Invalid frequency.\n";
            return 1;
        }

        if (speedStream >> speed &&
            !(speedStream >> remainingInput) &&
            speed >= 1 &&
            speed <= 30)
        {
        }
        else
        {
            std::cout << "Invalid speed.\n";
            return 1;
        }

        if (lengthStream >> length &&
            !(lengthStream >> remainingInput) &&
            length >= 1 &&
            length <= 30)
        {
        }
        else
        {
            std::cout << "Invalid length.\n";
            return 1;
        }

        if (mode.length() == 1 &&
            (mode[0] == 'Y' ||
                mode[0] == 'y' ||
                mode[0] == 'N' ||
                mode[0] == 'n'))
        {
            epilepsyMode = mode[0];
        }
        else
        {
            std::cout << "Invalid epilepsy mode. Use Y or N.\n";
            return 1;
        }

        // Validate explosion probability
        if (!(probabilityStream >> explosionProbability) ||
            (probabilityStream >> remainingInput) ||
            explosionProbability < 1 ||
            explosionProbability > 1000)
        {
            std::cout << "Invalid explosion probability.\n";
            return 1;
        }

        // Validate minimum explosion radius
        if (!(minRadiusStream >> minExplosionRadius) ||
            (minRadiusStream >> remainingInput) ||
            minExplosionRadius < 1 ||
            minExplosionRadius > 10)
        {
            std::cout << "Invalid minimum explosion radius.\n";
            return 1;
        }

        // Validate maximum explosion radius
        if (!(maxRadiusStream >> maxExplosionRadius) ||
            (maxRadiusStream >> remainingInput) ||
            maxExplosionRadius < minExplosionRadius ||
            maxExplosionRadius > 10)
        {
            std::cout << "Invalid maximum explosion radius.\n";
            return 1;
        }
    }
    else if (argc == 1)
    {
        std::string input;

        // Request and validate line generation frequency
        while (true)
        {
            std::cout << "Enter frequency (1-30): ";
            std::getline(std::cin, input);

            std::stringstream stream(input);
            if (stream >> frequency)
            {
                std::string remainingInput;

                if (!(stream >> remainingInput) && frequency >= 1 && frequency <= 30)
                {
                    break;
                }
            }

            std::cout << "Invalid frequency.\n";
        }

        // Request and validate line speed
        while (true)
        {
            std::cout << "Enter speed (1-30): ";
            std::getline(std::cin, input);

            std::stringstream stream(input);

            if (stream >> speed)
            {
                std::string remainingInput;

                if (!(stream >> remainingInput) && speed >= 1 && speed <= 30)
                {
                    break;
                }
            }

            std::cout << "Invalid speed.\n";
        }

        // Request and validate line length
        while (true)
        {
            std::cout << "Enter line length (1-30): ";
            std::getline(std::cin, input);

            std::stringstream stream(input);

            if (stream >> length)
            {
                std::string remainingInput;

                if (!(stream >> remainingInput) && length >= 1 && length <= 30)
                {
                    break;
                }
            }

            std::cout << "Invalid length.\n";
        }

        // Request and validate epilepsy mode
        while (true)
        {
            std::cout << "Epilepsy mode (Y/N): ";
            std::getline(std::cin, input);

            if (input.length() == 1 &&
                (input[0] == 'Y' ||
                    input[0] == 'y' ||
                    input[0] == 'N' ||
                    input[0] == 'n'))
            {
                epilepsyMode = input[0];
                break;
            }

            std::cout << "Invalid mode.\n";
        }

        // Request and validate explosion probability
        while (true)
        {
            std::cout << "Enter explosion probability (1-1000): ";
            std::getline(std::cin, input);

            std::stringstream stream(input);

            if (stream >> explosionProbability)
            {
                std::string remainingInput;

                if (!(stream >> remainingInput) &&
                    explosionProbability >= 1 &&
                    explosionProbability <= 1000)
                {
                    break;
                }
            }

            std::cout << "Invalid explosion probability.\n";
        }

        // Request and validate minimum explosion radius
        while (true)
        {
            std::cout << "Enter minimum explosion radius (1-10): ";
            std::getline(std::cin, input);

            std::stringstream stream(input);

            if (stream >> minExplosionRadius)
            {
                std::string remainingInput;

                if (!(stream >> remainingInput) &&
                    minExplosionRadius >= 1 &&
                    minExplosionRadius <= 10)
                {
                    break;
                }
            }

            std::cout << "Invalid minimum explosion radius.\n";
        }

        // Request and validate maximum explosion radius
        while (true)
        {
            std::cout << "Enter maximum explosion radius ("
                << minExplosionRadius << "-10): ";
            std::getline(std::cin, input);

            std::stringstream stream(input);

            if (stream >> maxExplosionRadius)
            {
                std::string remainingInput;

                if (!(stream >> remainingInput) &&
                    maxExplosionRadius >= minExplosionRadius &&
                    maxExplosionRadius <= 10)
                {
                    break;
                }
            }

            std::cout << "Invalid maximum explosion radius.\n";
        }
    }
    else
    {
        std::cout << "Invalid number of parameters.\n";
        std::cout << "Use Matrix.exe --help for usage information.\n";
        return 1;
    }

    // Create and run the application
    ApplicationSettings settings{
        frequency,
        speed,
        length,
        epilepsyMode,
        explosionProbability,
        minExplosionRadius,
        maxExplosionRadius
    };

    ApplicationManager applicationManager(settings);
    applicationManager.run();

    return 0;
}