#include "ApplicationManager.h"
#include "Console.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <windows.h>

// Creates an application manager with the specified parameters
ApplicationManager::ApplicationManager(int frequency, int speed, int length, char epilepsyMode)
    : frequency(frequency),
    speed(speed),
    length(length),
    epilepsyMode(epilepsyMode),
    line(nullptr)
{
    // Initialize the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

// Runs the Matrix application
void ApplicationManager::run()
{
    Console::clearScreen();

    while (true)
    {
        createLine();

        while (!isLineFinished())
        {
            const auto currentTime = Line::Clock::now();

            if (line->update(currentTime))
            {
                renderNewCharacter();
                clearRemovedCharacter();
            }
            else
            {
                Sleep(1);
            }
        }

        line.reset();
    }
}

// Creates a new line at a random horizontal position
void ApplicationManager::createLine()
{
    int consoleWidth = Console::getWidth();
    int x = 0;

    if (consoleWidth > 1)
    {
        x = std::rand() % (consoleWidth - 1);
    }

    int y = Console::getHeight();

    line = std::make_unique<Line>(
        length, x, y, epilepsyMode, speed, Line::Clock::now()
    );
}

// Renders the newly generated character
void ApplicationManager::renderNewCharacter()
{
    int characterY = line->getNewCharacterY();

    if (characterY >= 0 && characterY < Console::getHeight())
    {
        Console::setCursorPosition(
            line->getNewCharacterX(),
            characterY
        );

        Console::writeCharacter(line->getNewCharacter());
    }
}

// Clears the character removed from the end
void ApplicationManager::clearRemovedCharacter()
{
    if (!line->hasRemovedCharacter())
    {
        return;
    }

    int characterY = line->getRemovedCharacterY();

    if (characterY >= 0 && characterY < Console::getHeight())
    {
        Console::clearCharacter(
            line->getRemovedCharacterX(),
            characterY
        );
    }
}

// Checks whether the line has completely left the console
bool ApplicationManager::isLineFinished() const
{
    return line->getY() + line->getLength() <= 0;
}